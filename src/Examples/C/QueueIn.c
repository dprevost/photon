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
 *  - Create two queues (input/output) and a hash map (the hash map is used 
 *    to control the other two programs)
 *  - Read data from input file and insert it into the input queue.
 *  - At the end of a specified time, starts the shutdown process.
 */

#define _CRT_SECURE_NO_DEPRECATE

#include "iso_3166.h"
#include "Queue.h"
#include <time.h>
#include <stddef.h>

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
   if ( control != NULL ) {
      /* We flush it all before warning QueueWork to exit. */
      psoCommit( session );
      rc = psoHashMapReplace( control, shutdownKey, strlen(shutdownKey), 
         &controlData, sizeof(int), NULL );
      if ( rc != 0 ) {
         psoErrorMsg(session, msg, 256 );
         fprintf( stderr, "At line %d, psoHashMapReplace error: %s\n", __LINE__, msg );
      }
      else {
         psoCommit( session );
      }
      psoHashMapClose( control );
   }
   if ( inQueue != NULL )  psoQueueClose( inQueue );
   if ( outQueue != NULL ) psoQueueClose( outQueue );
   if ( session != NULL )  psoExitSession( session );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int initObjects()
{
   int rc;
   char msg[256];
   int controlData;
   
   psoObjectDefinition defMap = { PSO_HASH_MAP, 0, 0, 0 };
   psoObjectDefinition defQueue = { PSO_QUEUE, 0, 0, 0 };
   
   psoKeyFieldDefinition keyDef = { "ControlCode", PSO_KEY_VARCHAR, 20 };
   psoFieldDefinition fieldDef  = { "Status",      PSO_INTEGER,    {0} };

   psoFieldDefinition fieldDefQueue[2] = {
      { "CountryCode", PSO_CHAR,     {2} },
      { "CountryName", PSO_VARCHAR, {80} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   
   /* If the objects already exist, we remove them. */
   psoDestroyObject( session, inQueueName,  strlen(inQueueName)  );
   psoDestroyObject( session, outQueueName, strlen(outQueueName) );
   psoDestroyObject( session, controlName,  strlen(controlName)  );
   /* Remove the folder last (to delete a folder it must be empty) */
   psoDestroyObject( session, folderName,   strlen(folderName)   );
   /* Commit the destruction of these objects */
   rc = psoCommit( session );
   if ( rc != 0 ) {
      psoErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, psoCommit error: %s\n", __LINE__, msg );
      return -1;
   }

   /* Create the folder first, evidently */
   rc = psoCreateFolder( session, folderName, strlen(folderName) );
   if ( rc != 0 ) {
      psoErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, psoCreateFolder error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = psoKeyDefCreate( session,
                         "Country Code",
                         strlen("Country Code"),
                         PSO_DEF_PHOTON_ODBC_SIMPLE,
                         (unsigned char *)&keyDef,
                         sizeof(psoKeyFieldDefinition),
                         &keyDefHandle );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoKeyDefCreate error: %s\n", __LINE__, msg );
      return -1;
   }
      
   rc = psoDataDefCreate( session,
                          "Country Name",
                          strlen("Country Name"),
                          PSO_DEF_PHOTON_ODBC_SIMPLE,
                          (unsigned char *)&fieldDef,
                          sizeof(psoFieldDefinition),
                          &dataDefHandle );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoDataDefCreate error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = psoCreateMap( session,
                      controlName,
                      strlen(controlName),
                      &defMap,
                      dataDefHandle,
                      keyDefHandle );
   if ( rc != 0 ) {
      psoErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, psoCreateMap error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = psoCreateQueue( session,
                        inQueueName,
                        strlen(inQueueName),
                        &defQueue,
                        dataDefHandle );
   if ( rc != 0 ) {
      psoErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, psoCreateQueue error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = psoCreateQueue( session,
                        outQueueName,
                        strlen(outQueueName),
                        &defQueue,
                        dataDefHandle );
   if ( rc != 0 ) {
      psoErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, psoCreateQueue error: %s\n", __LINE__, msg );
      return -1;
   }

   rc = psoHashMapOpen( session, controlName, strlen(controlName), &control );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   /* Initialize the control object */
   controlData = 0; /* Will be set to one/two when it is time to shutdown */
   rc = psoHashMapInsert( control, shutdownKey, strlen(shutdownKey), 
      &controlData, sizeof(int), NULL );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapInsert error: %s\n", __LINE__, msg );
      return -1;
   }
   /*
    * The next two control items indicate if the other two programs are 
    * ready to use shared memory or not (otherwise you can get a bit of 
    * a problem, filling up the shared memory before they are started)
    */
   controlData = 0;
   rc = psoHashMapInsert( control, workProcessKey, strlen(workProcessKey), 
      &controlData, sizeof(int), NULL );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapInsert error: %s\n", __LINE__, msg );
      return -1;
   }
   rc = psoHashMapInsert( control, outProcessKey, strlen(outProcessKey), 
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

void waitForFriends()
{
   int rc;
   int controlData = 0;
   unsigned int length;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif
   do {
      rc = psoHashMapGet( control, workProcessKey, strlen(workProcessKey), 
         &controlData, sizeof(int), &length );
#if defined(WIN32)
      Sleep(1);
#else
      nanosleep( &req, &rem );
#endif      
   } while ( rc != 0 || controlData == 0 );

   controlData = 0;
   do {
      rc = psoHashMapGet( control, outProcessKey, strlen(outProcessKey), 
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
   
   if ( argc < 6 ) {
      fprintf( stderr, "Usage: %s iso_3166_data_file quasar_address number_of_iterations milliseconds iterations_per_cycle\n", argv[0] );
      return 1;
   }

   maxLoop = atoi(argv[3]);
   ms = atoi(argv[4]);
   cycle = atoi(argv[5]);
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   /* Initialize shared memory and create our session */
   rc = psoInit( argv[2], "QueueIn" );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInit error: %d\n", __LINE__, rc );
      return 1;
   }

   rc = psoInitSession( &session );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInitSession error: %d\n", __LINE__, rc );
      return 1;
   }

   /* Create and initialize objects */
   rc = initObjects();
   if ( rc != 0 ) { cleanup(); return 1; }
   fprintf( stderr, "Objects created\n" );

   /* Wait for the two other programs */
   waitForFriends();
   
   rc = psoQueueOpen( session, inQueueName, strlen(inQueueName), &inQueue );
   if ( rc != 0 ) {
      psoErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, psoQueueOpen error: %s\n", __LINE__, msg );
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
   while ( loop < maxLoop ) {
      /*
       * rc < 0 -> error
       * rc = 0 -> nothing read - EOF
       * rc > 0 -> new data
       */
      rc = readData( inStruct.countryCode, inStruct.description );
      if ( rc > 0 ) {
         rc = psoQueuePush( inQueue,
                            &inStruct,
                            2 + strlen( inStruct.description),
                            NULL );
         if ( rc != 0 ) {
            psoErrorMsg(session, msg, 256 );
            fprintf( stderr, "At line %d, psoQueuePush error: %s\n", __LINE__, msg );
            cleanup();
            return -1;
         }

         /* 
          * Why 10? It could be 100. Or 1. Not sure if it makes a big 
          * difference performance wise. If this code was reading from
          * non-blocking sockets in a "select loop", calling psoCommit for
          * each iteration of the loop would make sense. YMMV.
          */
         if ( (loop %10) == 0 ) psoCommit( session );

         if ( (loop % cycle) == 0 ) {
#if defined(WIN32)
            Sleep(ms);
#else
            nanosleep( &req, &rem );
#endif      
		   }

         loop++;
      }
      else if ( rc == 0 ) {
         rewind( fp );
      }
      else {
         cleanup();
         return -1;
      }
   }
   psoCommit( session );

   cleanup();
   fprintf( stderr, "Done: %s\n", argv[0] );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

