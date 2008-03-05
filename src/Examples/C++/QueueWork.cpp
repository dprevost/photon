/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * This program is part of a set of three, showing one possible use of queues.
 * Specific responsabilities:
 */

#include <time.h>
#include <ctype.h>

#include "iso_3166.h"
#include "Queue.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096 /* Safe enough on most systems I would think */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   int controlData = 2;

   // Error recovery: we do nothing except writing an error message. 
   try {
      // By setting the shutdown "flag" to 2, we tell the QueueOut program
      // that there will be no more data, it can shutdown.
      if ( control != NULL ) {
         // We flush it all before warning QueueOut to exit.
         session.Commit();
         control->Replace( shutdownKey, strlen(shutdownKey), 
                                &controlData, sizeof(int) );
         // We commit this update and cleanup
         session.Commit();
         control->Close();
         delete control;
         control = NULL;
      }
   
      if ( inQueue != NULL ) {
         inQueue->Close();
         delete inQueue;
         inQueue = NULL;
      }
      if ( outQueue != NULL ) {
         outQueue->Close();
         delete outQueue;
         outQueue = NULL;
      }
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void initObjects()
{
   int controlData;

   control->Open( controlName );

   controlData = 1;
   control->Replace( workProcessKey, strlen(workProcessKey),
                     &controlData, sizeof(int) );

   session.Commit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool timetoShutdown()
{
   int controlData = 0;
   size_t length;
   
   try {
      control->Get( shutdownKey, strlen(shutdownKey), 
                    &controlData, sizeof(int), &length );
   }
   catch(...) { return false; }

   if ( controlData == 1 ) return true;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   size_t length;
   isoStruct workStruct;
   size_t loop = 1, i;
   bool bShutdown = false;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif
   
   if ( argc < 2 )
   {
      cerr << "Usage: " << argv[0] << " watchdog_address" << endl;
      return 1;
   }

   // Initialize vds and create our session
   try {
      process.Init( argv[1] );
      session.Init();
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }
   control  = new vdsHashMap( session );
   inQueue  = new vdsQueue  ( session );
   outQueue = new vdsQueue  ( session );

   // Initialize objects
   try {
      initObjects();
      outQueue->Open( outQueueName );
      inQueue->Open( inQueueName );
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }

   try {
      while( 1 ) {
         rc = inQueue->Pop( &workStruct, sizeof(workStruct), &length );
         if ( rc != VDS_OK ) {
            /* Nothing to do - might as well commit */
            session.Commit();
            if ( bShutdown )
               break;
            /*
             * We continue after we receive the shutdown to make sure that
             * there are no data left on the input queue. 
             */
#if defined(WIN32)
   		   Sleep( 1 );
#else
            nanosleep( &req, &rem );
#endif
            bShutdown = timetoShutdown();
            continue;
         }

         for ( i = 0; i < length-2; ++i )
            workStruct.description[i] = toupper(workStruct.description[i]);
      
         outQueue->Push( &workStruct, length );

         if ( (loop %10) == 0 )
            session.Commit();
         loop++;
      }
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }

   cleanup();
   cout << "Done: " << argv[0] << endl;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
