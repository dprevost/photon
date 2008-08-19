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
 * This example: 
 *   We create a hash map and play with it. We do it using two sessions to
 *   show the idea behind sessions.
 */

#include "iso_3166.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096 /* Safe enough on most systems I would think */
#endif

/* Some globals to make our life simpler */
PSO_HANDLE session1 = NULL, session2 = NULL;
PSO_HANDLE map1 = NULL, map2 = NULL;
const char* mapName = "My Hash Map";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   if ( map1 != NULL ) psoHashMapClose( map1 );
   if ( map2 != NULL ) psoHashMapClose( map2 );
   
   if ( session1 != NULL ) vdsExitSession( session1 );
   if ( session2 != NULL ) vdsExitSession( session2 );

   vdsExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int createMap()
{
   int rc;
   char msg[256];
   char countryCode[2];
   char description[100];
  
   /* If the map already exists, we remove it. */
   rc = vdsDestroyObject( session1, mapName, strlen(mapName) );
   if ( rc == PSO_NO_SUCH_OBJECT || rc == PSO_OK ) {
      /*
       * We must commit the change if we just destroyed it otherwise it
       * will still exist! 
       */
      rc = vdsCommit( session1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
         return -1;
      }
      
      rc = vdsCreateObject( session1, mapName, strlen(mapName), PSO_HASH_MAP );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, vdsCreateObject error: %s\n", __LINE__, msg );
         return -1;
      }
      /* Commit the creation of the object */
      rc = vdsCommit( session1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
         return -1;
      }
      rc = psoHashMapOpen( session1, mapName, strlen(mapName), &map1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoHashMapOpen error: %s\n", __LINE__, msg );
         return -1;
      }
      /*
       * rc < 0 -> error
       * rc = 0 -> nothing read - EOF
       * rc > 0 -> new data
       */
      rc = readData( countryCode, description );
      while ( rc > 0 ) {
         rc = psoHashMapInsert( map1, countryCode, 2, 
            description, strlen(description) );
         if ( rc != 0 ) {
            psoErrorMsg(session1, msg, 256 );
            fprintf( stderr, "At line %d, psoHashMapInsert error: %s\n", __LINE__, msg );
            return -1;
         }

         rc = readData( countryCode, description );
      }
   }
   else { /* A problem when calling destroy */

      psoErrorMsg(session1, msg, 256 );
      fprintf( stderr, "At line %d, vdsDestroyObject error: %s\n", __LINE__, msg );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   char description[80];
   char msg[256];
   size_t length;
   psoObjStatus status;
   
   if ( argc < 3 ) {
      fprintf( stderr, "Usage: %s iso_3166_data_file watchdog_address\n", argv[0] );
      return 1;
   }
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   /* Initialize vds and create our session */
   rc = vdsInit( argv[2], 0 );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, vdsInit error: %d\n", __LINE__, rc );
      return 1;
   }

   rc = vdsInitSession( &session1 );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, vdsInitSession error: %d\n", __LINE__, rc );
      return 1;
   }
   rc = vdsInitSession( &session2 );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, vdsInitSession error: %d\n", __LINE__, rc );
      return 1;
   }
   
   /* Create a hash map object */
   rc = createMap();
   if ( rc != 0 ) { cleanup(); return 1; }
   fprintf( stderr, "Map created\n" );
   
   rc = psoHashMapOpen( session2, mapName, strlen(mapName), &map2 );
   if ( rc != 0 ) {
      psoErrorMsg(session2, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapOpen error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   
   /* The data is inserted but not committed yet - failure is expected */
   rc = psoHashMapGet( map2, "FM", 2, description, 80, &length );
   if ( rc == 0 ) {
      fprintf( stderr, "At line %d, unexpected success in psoHashMapGet! \n", __LINE__ );
      cleanup();
      return -1;
   }
   if ( rc != PSO_ITEM_IS_IN_USE ) {
      psoErrorMsg(session2, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapGet error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   rc = vdsCommit( session1 );
   if ( rc != 0 ) {
      psoErrorMsg(session1, msg, 256 );
      fprintf( stderr, "At line %d, vdsCommit error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   
   psoHashMapStatus( map2, &status );
   fprintf( stderr, "Number of countries in the hash map = %d\n", 
      status.numDataItem );

   rc = psoHashMapGet( map2, "FM", 2, description, 80, &length );
   if ( rc != 0 ) {
      psoErrorMsg(session2, msg, 256 );
      fprintf( stderr, "At line %d, psoHashMapGet error: %s\n", __LINE__, msg );
      cleanup();
      return -1;
   }
   fprintf( stderr, "Country code: FM, country: %s\n", description );
   
   if ( fp != NULL ) fclose( fp );

   cleanup();
   
   return 0;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

