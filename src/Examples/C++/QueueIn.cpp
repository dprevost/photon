/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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
#  define PATH_MAX 4096 // Safe enough on most systems I would think
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void cleanup()
{
   int controlData = 1;

   // Error recovery: we do nothing except writing an error message. 
   try {
      // By setting the shutdown "flag" to 1, we tell the QueueWork program 
      // that there will be no more data, it can shutdown (the QueueWork 
      // program will then tell the QueueOut program by setting it to two).

      // We flush it all before warning QueueWork to exit.
      session.Commit();
      control.Replace( shutdownKey, strlen(shutdownKey), 
                       &controlData, sizeof(int) );
      // We commit this update and cleanup
      session.Commit();
      
      // Close them explicitly (or we could let the destructor do it for us). 
      control.Close();
      inQueue.Close();
   }
   catch( psoException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void initObjects()
{
   int controlData;
   size_t len;
   
   psoObjectDefinition defFolder;
   memset( &defFolder, 0, sizeof(psoObjectDefinition) );
   defFolder.type = PSO_FOLDER;

   psoObjectDefinition defMap = { 
      PSO_HASH_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 20}, 
      { { "Status", PSO_INTEGER, 4, 0, 0, 0, 0} } 
   };
   psoObjectDefinition * pDefQueue = NULL;

   len = offsetof( psoObjectDefinition, fields ) + 
      2 * sizeof(psoFieldDefinition);
   pDefQueue = (psoObjectDefinition *)calloc( len, 1 );
   pDefQueue->type = PSO_QUEUE;
   pDefQueue->numFields = 2;
   pDefQueue->fields[0].type = PSO_STRING;
   pDefQueue->fields[1].type = PSO_VAR_STRING;

   pDefQueue->fields[0].length = 2;
   pDefQueue->fields[1].minLength = 1;
   pDefQueue->fields[1].maxLength = 80;

   strcpy( pDefQueue->fields[0].name, "Countrycode" );
   strcpy( pDefQueue->fields[1].name, "CountryName" );

   // If the objects already exist, we remove them.
   try {
      session.DestroyObject( inQueueName );
      session.DestroyObject( outQueueName );
      session.DestroyObject( controlName );
      // Remove the folder last (to delete a folder it must be empty)
      session.DestroyObject( folderName );
      // Commit the destruction of these objects
      session.Commit();
   }
   // Do nothing - if a strange error occured it will show up in the next step
   catch(...) {} 
   
   // Create the folder first, evidently
   session.CreateObject( folderName, &defFolder );
   session.CreateObject( controlName, &defMap );
   session.CreateObject( inQueueName, pDefQueue );
   session.CreateObject( outQueueName, pDefQueue );

   control.Open( controlName );
   // Initialize the control object
   controlData = 0; // Will be set to one/two when it is time to shutdown
   control.Insert( shutdownKey, strlen(shutdownKey), 
                   &controlData, sizeof(int) );

   // The next two control items indicate if the other two programs are 
   // ready to use shared memory or not (otherwise you can get a bit of 
   // a problem, filling up the shared memory before they are started)
   controlData = 0;
   control.Insert( workProcessKey, strlen(workProcessKey), 
                   &controlData, sizeof(int) );
   control.Insert( outProcessKey, strlen(outProcessKey), 
                   &controlData, sizeof(int) );

   session.Commit();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void waitForFriends()
{
   int controlData = 0;
   size_t length;
#if ! defined(WIN32)
   struct timespec req, rem;
   
   req.tv_sec = 0;
   req.tv_nsec = 1000000;
#endif

   do {
      try {
         control.Get( workProcessKey, strlen(workProcessKey), 
                      &controlData, sizeof(int), length );
      }
      catch(...) {}

#if defined(WIN32)
      Sleep(1);
#else
      nanosleep( &req, &rem );
#endif
   } while ( controlData == 0 );

   controlData = 0;
   do {
      try {
         control.Get( outProcessKey, strlen(outProcessKey), 
                      &controlData, sizeof(int), length );
      }
      catch(...) {}

#if defined(WIN32)
      Sleep(1);
#else
      nanosleep( &req, &rem );
#endif      
   } while ( controlData == 0 );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char *argv[] )
{
   int rc;
   isoStruct inStruct;
   int loop = 1, maxLoop, cycle, ms;
#if ! defined(WIN32)
   struct timespec req, rem;
#endif
   
   if ( argc < 6 ) {
      cerr << "Usage: " << argv[0] << " iso_3166_data_file quasar_address" <<
         "number_of_iterations milliseconds iterations_per_cycle" << endl;
      return 1;
   }

   maxLoop = atoi(argv[3]);
   ms = atoi(argv[4]);
   cycle = atoi(argv[5]);
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   // Initialize the shared memory and create our session
   try {
      process.Init( argv[2] );
      session.Init();
   }
   catch( psoException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }

   // Create and initialize objects
   try {
      initObjects();
      inQueue.Open( inQueueName );
   }
   catch( psoException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }
   cout << "Objects created" << endl;

   // Wait for the two other programs
   waitForFriends();
   
   /* 
    * We reuse the same data records (from the iso file) to populate our
    * queue. We could have use random data instead but... the readData()
    * function wa already written for other examples...
    */
#if ! defined(WIN32)
   req.tv_sec = 0;
   req.tv_nsec = ms *1000000;
#endif

   try { 
      while ( loop < maxLoop ) {
         /*
          * rc < 0 -> error
          * rc = 0 -> nothing read - EOF
          * rc > 0 -> new data
          */
         rc = readData( inStruct.countryCode, inStruct.description );
         if ( rc > 0 ) {

            inQueue.Push( &inStruct, 2 + strlen( inStruct.description) );
            /* 
             * Why 10? It could be 100. Or 1. Not sure if it makes a big 
             * difference performance wise. If this code was reading from
             * non-blocking sockets in a "select loop", calling Commit for
             * each iteration of the loop would make sense. YMMV.
             */
            if ( (loop %10) == 0 )
               session.Commit();

            if ( (loop %cycle) == 0 ) {
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
         else {
            cleanup();
            return 1;
         }
      }
      session.Commit();
   }
   catch( psoException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return 1;
   }
   
   cleanup();
   cout << "Done: " << argv[0] << endl;

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

