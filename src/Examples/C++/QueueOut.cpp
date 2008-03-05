/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * This program is part of a set of three, showing one possible use of queues.
 */

#include "iso_3166.h"
#include "Queue.h"
#include <time.h>

#ifndef PATH_MAX
#  define PATH_MAX 4096 /* Safe enough on most systems I would think */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   // Error recovery: we do nothing except writing an error message. 
   try {
      if ( control != NULL ) {
         control->Close();
         delete control;
         control = NULL;
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
   control->Replace( outProcessKey, strlen(outProcessKey), 
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

   if ( controlData == 2 ) return true;
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   size_t length;
   isoStruct outStruct;
   int loop = 1;
   bool bShutdown = false; // , okShutdown = 0;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif

   if ( argc < 2 ) {
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
   outQueue = new vdsQueue  ( session );

   // Initialize objects
   try {
      initObjects();
      outQueue->Open( outQueueName );
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }

   try {
      while( 1 ) {
         rc = outQueue->Pop( &outStruct, sizeof(outStruct), &length );
         if ( rc != VDS_OK ) {
            /* Nothing to do - might as well commit */
            session.Commit();
            if ( bShutdown )
               break;
#if defined(WIN32)
            Sleep(1);
#else
            nanosleep( &req, &rem );
#endif
            /*
             * We continue after we receive the shutdown to make sure that
             * there are no data left on the output queue. 
             */
            bShutdown = timetoShutdown();
            continue;
         }

         /* 
          * Why 10? It could be 100. Or 1. Not sure if it makes a big 
          * difference performance wise. 
          */
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

