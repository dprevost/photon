/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PATH_MAX
#  define PATH_MAX 4096 /* Safe enough on most systems I would think */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   if ( control != NULL )
      vdsHashMapClose( control );
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
   /* Folder at the end (must be empty) */
   vdsDestroyObject( session, folderName,   strlen(folderName)   );
   /* Commit the destruction of the objects */
   rc = vdsCommit( session );
   if ( rc != 0 ) 
   {
      vdsErrorMsg( session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
      return -1;
   }

   /* Folder must be first */
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
   controlData = 0; /* Will be set to one when it is time to shutdown */
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
    * still using the shared objects or not (they use it to store their
    * pids?).
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

int main( int argc, char *argv[] )
{
   int rc;
   char countryCode[2];
   char description[80];
   char msg[256];
   int length;
   vdsObjStatus status;
   int controlData;
   
   if ( argc < 3 )
   {
      fprintf( stderr, "Usage: %s iso_3166_data_file watchdog_address\n", argv[0] );
      return 1;
   }
   
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

   rc = vdsQueueOpen( session, inQueueName, strlen(inQueueName), &inQueue );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsQueueOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   
   
#if 0      


   rc = vdsCommit( session1 );
   if ( rc != 0 ) 
   {
      vdsErrorMsg(session, msg, 256 );
      fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   
   if ( fp != NULL )
      fclose( fp );
#endif

   cleanup();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
