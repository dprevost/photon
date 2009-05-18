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
 */

#define _CRT_SECURE_NO_DEPRECATE

#include "iso_3166.h"
#include "Queue.h"
#include <time.h>

#ifndef PATH_MAX
#  define PATH_MAX 4096 /* Safe enough on most systems I would think */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   if ( control != NULL )  psoHashMapClose( control );
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
   rc = psoHashMapReplace( control, outProcessKey, strlen(outProcessKey), 
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
   if ( rc == 0 && controlData == 2 ) return 1;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   char msg[256];
   unsigned int length;
   isoStruct outStruct;
   int loop = 1;
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
   rc = psoInit( argv[1], "QueueOut" );
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

   while( 1 ) {
      rc = psoQueuePop( outQueue, &outStruct, sizeof(outStruct), &length );
      if ( rc == PSO_IS_EMPTY || rc == PSO_ITEM_IS_IN_USE ) {
         /* Nothing to do - might as well commit */
         psoCommit( session );
         if ( boolShutdown ) break;
#if defined(WIN32)
         Sleep(1);
#else
         nanosleep( &req, &rem );
#endif

         /*
          * We continue after we receive the shutdown to make sure that
          * there are no data left on the output queue. 
          */
         boolShutdown = timetoShutdown();
         continue;
      }
      else if ( rc != 0 ) {
         psoErrorMsg(session, msg, 256 );
         fprintf( stderr, "At line %d, psoQueuePop error: %s\n", __LINE__, msg );
         cleanup();
         return -1;
      }

      /* 
       * Why 10? It could be 100. Or 1. Not sure if it makes a big 
       * difference performance wise. 
       */
      if ( (loop %10) == 0 ) psoCommit( session );
      loop++;
   }

   cleanup();
   fprintf( stderr, "Done: %s\n", argv[0] );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

