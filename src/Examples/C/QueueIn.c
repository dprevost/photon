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
 *  - Create two queues (input/output) and a hash map (the hash map is used 
 *    to control the other two programs)
 *  - Read data from input file and insert it into the input queue.
 *  - At the end of a specified time, starts the shutdown process.
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
   int controlData = 1, rc;
   char msg[256];
   
   /* 
    * By setting the shutdown "flag" to 1, we tell the QueueWork program 
    * that there will be no more data, it can shutdown (the QueueWork program
    * will then tell the QueueOut program by setting it to two).
    */
   if ( control != NULL )
   {
      /* We flush it all before warning QueueWork to exit. */
      vdsCommit( session );
      rc = vdsHashMapReplace( control, shutdownKey, strlen(shutdownKey), 
         &controlData, sizeof(int) );
      if ( rc != 0 ) 
      {
         vdsErrorMsg(session, msg, 256 );
         fprintf( stderr, "At line %d, vdsHashMapReplace error: %s\n", __LINE__, msg );
      }
      else
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

   /* If the objects already exist, we remove them. */
   vdsDestroyObject( session, inQueueName,  strlen(inQueueName)  );
   vdsDestroyObject( session, outQueueName, strlen(outQueueName) );
   vdsDestroyObject( session, controlName,  strlen(controlName)  );
   /* Remove th folder last (to delete a folder it must be empty) */
   vdsDestroyObject( session, folderName,   strlen(folderName)   );
   /* Commit the destruction of these objects */
   rc = vdsCommit( session );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
      return -1;
   }

   /* Create the folder first, evidently */
   rc = vdsCreateObject( session, folderName, strlen(folderName), VDS_FOLDER );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCreateObject error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = vdsCreateObject( session, controlName, strlen(controlName), VDS_HASH_MAP );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCreateObject error: %s\n", __LINE__, msg );
      return -1;
   }
   rc = vdsCreateObject( session, inQueueName, strlen(inQueueName), VDS_QUEUE );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCreateObject error: %s\n", __LINE__, msg );
      return -1;
   }
   rc = vdsCreateObject( session, outQueueName, strlen(outQueueName), VDS_QUEUE );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCreateObject error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = vdsHashMapOpen( session, controlName, strlen(controlName), &control );
   if ( rc != 0 )
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsHashMapOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   /* Initialize the control object */
   controlData = 0; /* Will be set to one/two when it is time to shutdown */
   rc = vdsHashMapInsert( control, shutdownKey, strlen(shutdownKey), 
      &controlData, sizeof(int) );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsHashMapInsert error: %s\n", __LINE__, msg );
      return -1;
   }
   /*
    * The next two control items indicate if the other two programs are 
    * ready to use the VDS or not (otherwise you can get a bit of a problem
    * filling up the shared memory.
    */
   controlData = 0;
   rc = vdsHashMapInsert( control, workProcessKey, strlen(workProcessKey), 
      &controlData, sizeof(int) );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsHashMapInsert error: %s\n", __LINE__, msg );
      return -1;
   }
   rc = vdsHashMapInsert( control, outProcessKey, strlen(outProcessKey), 
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

void waitForFriends()
{
   int rc;
   int controlData = 0;
   size_t length;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif
   do 
   {
      rc = vdsHashMapGet( control, workProcessKey, strlen(workProcessKey), 
         &controlData, sizeof(int), &length );
#if defined(WIN32)
	  Sleep(1);
#else
      nanosleep( &req, &rem );
#endif      
   } while ( rc != 0 || controlData == 0 );

   controlData = 0;
   do 
   {
      rc = vdsHashMapGet( control, outProcessKey, strlen(outProcessKey), 
         &controlData, sizeof(int), &length );
#if defined(WIN32)
	  Sleep(1);
#else
      nanosleep( &req, &rem );
#endif      
   } while ( rc != 0 || controlData == 0 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   char msg[256];
   isoStruct inStruct;
   int loop = 1, maxLoop, cycle, ms;
#if ! defined(WIN32)
   struct timespec req, rem;
#endif
   
   if ( argc < 6 )
   {
      fprintf( stderr, "Usage: %s iso_3166_data_file watchdog_address number_of_iterations milliseconds iterations_per_cycle\n", argv[0] );
      return 1;
   }

   maxLoop = atoi(argv[3]);
   ms = atoi(argv[4]);
   cycle = atoi(argv[5]);
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   /* Initialize vds and create our session */
   rc = vdsInit( argv[2], 0 );
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

   /* Create and initialize objects */
   rc = initObjects();
   if ( rc != 0 ) { cleanup(); return 1; }
   fprintf( stderr, "Objects created\n" );

   /* Wait for the two other programs */
   waitForFriends();
   
   rc = vdsQueueOpen( session, inQueueName, strlen(inQueueName), &inQueue );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsQueueOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }

   /* 
    * We reuse the same data records (from the iso file) to populate our
    * queue. We could have use random data instead but... the readData()
    * function wa already written for other examples...
    */
#if ! defined(WIN32)
   req.tv_sec = 0;
   req.tv_nsec = ms *1000000;
#endif
   while ( loop < maxLoop )
   {
      /*
       * rc < 0 -> error
       * rc = 0 -> nothing read - EOF
       * rc > 0 -> new data
       */
      rc = readData( inStruct.countryCode, inStruct.description );
      if ( rc > 0 )
      {
         rc = vdsQueuePush( inQueue, &inStruct, 2 + strlen( inStruct.description) );
         if ( rc != 0 ) 
         {
            vdsErrorMsg(session, msg, 256 );
            fprintf( stderr, "At line %d, vdsQueuePush error: %s\n", __LINE__, msg );
            cleanup();
            return -1;
         }

         /* 
          * Why 10? It could be 100. Or 1. Not sure if it makes a big 
          * difference performance wise. If this code was reading from
          * non-blocking sockets in a "select loop", calling vdsCommit for
          * each iteration of the loop would make sense. YMMV.
          */
         if ( (loop %10) == 0 )
            vdsCommit( session );

         if ( (loop %cycle) == 0 )
		 {
#if defined(WIN32)
	        Sleep(ms);
#else
            nanosleep( &req, &rem );
#endif      
		 }

         loop++;
      }
      else if ( rc == 0 )
         rewind( fp );
      else
      {
         cleanup();
         return -1;
      }
   }
   vdsCommit( session );

   cleanup();
   fprintf( stderr, "Done: %s\n", argv[0] );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

