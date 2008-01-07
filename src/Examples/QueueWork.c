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
 * Specific responsabilities:
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
   int controlData = 2, rc;
   char msg[256];

   /* 
    * By setting the shutdown "flag" to 2, we tell the QueueOut program
    * that there will be no more data, it can shutdown.
    */
   if ( control != NULL )
   {
      /* We flush it all before warning QueueOut to exit. */
      vdsCommit( session );
      rc = vdsHashMapReplace( control, shutdownKey, strlen(shutdownKey), 
         &controlData, sizeof(int) );
      if ( rc != 0 ) 
      {
         vdsErrorMsg(session, msg, 256 );
         fprintf( stderr, "At line %d, vdsHashMapReplace error: %s\n", __LINE__, msg );
      }
      else
      /* We commit this update */
      vdsCommit( session );
      vdsHashMapClose( control );
   }

   if ( inQueue != NULL )
      vdsQueueClose( inQueue );
   if ( outQueue != NULL )
      vdsQueueClose( outQueue );
   
   if ( session != NULL )
      vdsExitSession( session );
   vdsExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int initObjects()
{
   int rc;
   char msg[256];
   int controlData;

   rc = vdsHashMapOpen( session, controlName, strlen(controlName), &control );
   if ( rc != 0 )
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsHashMapOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }

   controlData = 1;
   rc = vdsHashMapReplace( control, workProcessKey, strlen(workProcessKey),
      &controlData, sizeof(int) );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsHashMapInsert error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = vdsCommit( session );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int timetoShutdown()
{
   int rc;
   int controlData = 0, length;
   
   rc = vdsHashMapGet( control, shutdownKey, strlen(shutdownKey), 
      &controlData, sizeof(int), &length );
   if ( rc == 0 && controlData == 1 )
      return 1;

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   char msg[256];
   int length;
   isoStruct workStruct;
   int loop = 1, i;
   int boolShutdown = 0;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif
   
   if ( argc < 2 )
   {
      fprintf( stderr, "Usage: %s watchdog_address\n", argv[0] );
      return 1;
   }

   /* Initialize vds and create our session */
   rc = vdsInit( argv[1], 0 );
   if ( rc != 0 ) 
   {
      fprintf( stderr, "At line %d, vdsInit error: %d\n", __LINE__, rc );
      return 1;
   }

   rc = vdsInitSession( &session );
   if ( rc != 0 ) 
   {
      fprintf( stderr, "At line %d, vdsInitSession error: %d\n", __LINE__, rc );
      return 1;
   }

   /* Initialize objects */
   rc = initObjects();
   if ( rc != 0 ) { cleanup(); return 1; }
   
   rc = vdsQueueOpen( session, outQueueName, strlen(outQueueName), &outQueue );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsQueueOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   rc = vdsQueueOpen( session, inQueueName, strlen(inQueueName), &inQueue );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsQueueOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }

   while( 1 )
   {
      rc = vdsQueuePop( inQueue, &workStruct, sizeof(workStruct), &length );
      if ( rc == VDS_IS_EMPTY )
      {
         /* Nothing to do - might as well commit */
         vdsCommit( session );
         if ( boolShutdown )
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
         boolShutdown = timetoShutdown();
         continue;
      }
      else if ( rc != 0 ) 
      {
         vdsErrorMsg(session, msg, 256 );
            fprintf( stderr, "At line %d, vdsQueuePush error: %s\n", __LINE__, msg );
         cleanup();
         return -1;
      }

      for ( i = 0; i < length-2; ++i )
         workStruct.description[i] = toupper(workStruct.description[i]);
      
      rc = vdsQueuePush( outQueue, &workStruct, length );
      if ( rc != 0 ) 
      {
         vdsErrorMsg(session, msg, 256 );
            fprintf( stderr, "At line %d, vdsQueuePush error: %s\n", __LINE__, msg );
         cleanup();
         return -1;
      }

      if ( (loop %10) == 0 )
         vdsCommit( session );
      loop++;
   }

   cleanup();
   fprintf( stderr, "Done: %s\n", argv[0] );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
