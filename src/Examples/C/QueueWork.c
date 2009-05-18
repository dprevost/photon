/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#define _CRT_SECURE_NO_DEPRECATE

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
   int controlData = 2, rc;
   char msg[256];

   /* 
    * By setting the shutdown "flag" to 2, we tell the QueueOut program
    * that there will be no more data, it can shutdown.
    */
   if ( control != NULL ) {
      /* We flush it all before warning QueueOut to exit. */
      psoCommit( session );
      rc = psoHashMapReplace( control, shutdownKey, strlen(shutdownKey), 
         &controlData, sizeof(int), NULL );
      if ( rc != 0 ) {
         psoErrorMsg(session, msg, 256 );
         fprintf( stderr, "At line %d, psoHashMapReplace error: %s\n", __LINE__, msg );
      }
      else {
         /* We commit this update */
         psoCommit( session );
      }
      psoHashMapClose( control );
   }

   if ( inQueue != NULL )  psoQueueClose( inQueue );
   if ( outQueue != NULL ) psoQueueClose( outQueue );
   
   if ( session != NULL ) psoExitSession( session );
   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int initObjects()
{
   int rc;
   char msg[256];
   int controlData;

   rc = psoHashMapOpen( session, controlName, strlen(controlName), &control );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }

   controlData = 1;
   rc = psoHashMapReplace( control, workProcessKey, strlen(workProcessKey),
      &controlData, sizeof(int), NULL );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapInsert error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = psoCommit( session );
   if ( rc != 0 ) {
      psoErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, psoCommit error: %s\n", __LINE__, msg );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int timetoShutdown()
{
   int rc;
   int controlData = 0;
   unsigned int length;
   
   rc = psoHashMapGet( control, shutdownKey, strlen(shutdownKey), 
      &controlData, sizeof(int), &length );
   if ( rc == 0 && controlData == 1 ) return 1;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   char msg[256];
   unsigned int length;
   isoStruct workStruct;
   unsigned int loop = 1, i;
   int boolShutdown = 0;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif
   
   if ( argc < 2 ) {
      fprintf( stderr, "Usage: %s quasar_address\n", argv[0] );
      return 1;
   }

   /* Initialize shared memory and create our session */
   rc = psoInit( argv[1], "QueueWork" );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInit error: %d\n", __LINE__, rc );
      return 1;
   }

   rc = psoInitSession( &session );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInitSession error: %d\n", __LINE__, rc );
      return 1;
   }

   /* Initialize objects */
   rc = initObjects();
   if ( rc != 0 ) { cleanup(); return 1; }
   
   rc = psoQueueOpen( session, outQueueName, strlen(outQueueName), &outQueue );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoQueueOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   rc = psoQueueOpen( session, inQueueName, strlen(inQueueName), &inQueue );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoQueueOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }

   while( 1 ) {
      rc = psoQueuePop( inQueue, &workStruct, sizeof(workStruct), &length );
      if ( rc == PSO_IS_EMPTY || rc == PSO_ITEM_IS_IN_USE ) {
         /* Nothing to do - might as well commit */
         psoCommit( session );
         if ( boolShutdown ) break;
         /*
          * We continue after we receive the shutdown to make sure that
          * there are no data left on the input queue. 
          */
#if defined(WIN32)
		   Sleep( 1 );
#else
         nanosleep( &req, &rem );
#endif
         boolShutdown = timetoShutdown();
         continue;
      }
      else if ( rc != 0 ) {
         psoErrorMsg(session, msg, 256 );
            fprintf( stderr, "At line %d, psoQueuePop error: %s\n", __LINE__, msg );
         cleanup();
         return -1;
      }

      for ( i = 0; i < length-2; ++i ) {
         workStruct.description[i] = toupper(workStruct.description[i]);
      }
      
      rc = psoQueuePush( outQueue, &workStruct, length, NULL );
      if ( rc != 0 ) {
         psoErrorMsg(session, msg, 256 );
         fprintf( stderr, "At line %d, psoQueuePush error: %s\n", __LINE__, msg );
         cleanup();
         return -1;
      }

      if ( (loop %10) == 0 ) psoCommit( session );
      loop++;
   }

   cleanup();
   fprintf( stderr, "Done: %s\n", argv[0] );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

