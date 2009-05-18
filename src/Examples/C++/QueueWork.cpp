/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

// This program is part of a set of three, showing one possible use of queues.

#define _CRT_SECURE_NO_DEPRECATE

#include <time.h>
#include <ctype.h>

#include "iso_3166.h"
#include "Queue.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096 // Safe enough on most systems I would think
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void cleanup()
{
   int controlData = 2;

   // Error recovery: we do nothing except writing an error message. 
   try {
      // By setting the shutdown "flag" to 2, we tell the QueueOut program
      // that there will be no more data, it can shutdown.

      // We flush it all before warning QueueOut to exit.
      session.Commit();
      control.Replace( shutdownKey, strlen(shutdownKey), 
                       &controlData, sizeof(int) );
      // We commit this update and cleanup
      session.Commit();

      // Close them explicitly (or we could let the destructor do it for us). 
      control.Close();
      inQueue.Close();
      outQueue.Close();
   }
   catch( pso::Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void initObjects()
{
   int controlData;

   control.Open( session, controlName );

   controlData = 1;
   control.Replace( workProcessKey, strlen(workProcessKey),
                    &controlData, sizeof(int) );

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool timetoShutdown()
{
   int controlData = 0;
   size_t length;
   
   try {
      control.Get( shutdownKey, strlen(shutdownKey), 
                   &controlData, sizeof(int), length );
   }
   catch(...) { return false; }

   if ( controlData == 1 ) return true;
   return false;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

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
   
   if ( argc < 2 ) {
      cerr << "Usage: " << argv[0] << " quasar_address" << endl;
      return 1;
   }

   // Initialize shared memory and create our session
   try {
      process.Init( argv[1], "QueueWork" );
      session.Init();
   }
   catch( pso::Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }

   // Initialize objects
   try {
      initObjects();
      outQueue.Open( session, outQueueName );
      inQueue.Open( session, inQueueName );
   }
   catch( pso::Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }

   try {
      while( 1 ) {
         rc = inQueue.Pop( &workStruct, sizeof(workStruct), length );
         if ( rc != PSO_OK ) {
            // Nothing to do - might as well commit
            session.Commit();
            if ( bShutdown )
               break;
#if defined(WIN32)
   		   Sleep( 1 );
#else
            nanosleep( &req, &rem );
#endif
            bShutdown = timetoShutdown();
            // We continue (for one loop) after we receive the shutdown to 
            // make sure that there are no data left on the input queue.
            continue;
         }

         for ( i = 0; i < length-2; ++i )
            workStruct.description[i] = toupper(workStruct.description[i]);
      
         outQueue.Push( &workStruct, length );

         if ( (loop %10) == 0 )
            session.Commit();
         loop++;
      }
   }
   catch( pso::Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }

   cleanup();
   cout << "Done: " << argv[0] << endl;
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- 

