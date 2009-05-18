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
 * This example: 
 *   We create a hash map and play with it. We do it using two sessions to
 *   show the idea behind sessions.
 */

#define _CRT_SECURE_NO_DEPRECATE

#include "iso_3166.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096 /* Safe enough on most systems I would think */
#endif

/* Some globals to make our life simpler */
PSO_HANDLE session1 = NULL, session2 = NULL;
PSO_HANDLE map1 = NULL, map2 = NULL;
const char* mapName = "MyHashMap";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   if ( map1 != NULL ) psoHashMapClose( map1 );
   if ( map2 != NULL ) psoHashMapClose( map2 );
   
   if ( session1 != NULL ) psoExitSession( session1 );
   if ( session2 != NULL ) psoExitSession( session2 );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int createMap()
{
   int rc;
   char msg[256];
   char countryCode[2];
   char description[100];
   PSO_HANDLE keyDefHandle, dataDefHandle;
   
   /*
    * The content of the hash map is simple: a fixed length key, the country 
    * code, and the country name (a variable string - max length of 100).
    */
    psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, 0 };
   
   psoKeyFieldDefinition keyDef = { "CountryCode", PSO_KEY_CHAR, 2 };
   psoFieldDefinition fieldDef  = { "CountryName", PSO_VARCHAR, {100} };

   /* If the map already exists, we remove it. */
   rc = psoDestroyObject( session1, mapName, strlen(mapName) );
   if ( rc == PSO_NO_SUCH_OBJECT || rc == PSO_OK ) {
      /*
       * We must commit the change if we just destroyed it otherwise it
       * will still exist! 
       */
      rc = psoCommit( session1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoCommit error: %s\n", __LINE__, msg );
         return -1;
      }
      
      rc = psoKeyDefCreate( session1,
                            "Country Code",
                            strlen("Country Code"),
                            PSO_DEF_PHOTON_ODBC_SIMPLE,
                            (unsigned char *)&keyDef,
                            sizeof(psoKeyFieldDefinition),
                            &keyDefHandle );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoKeyDefCreate error: %s\n", __LINE__, msg );
         return -1;
      }
      
      rc = psoDataDefCreate( session1,
                             "Country Name",
                             strlen("Country Name"),
                             PSO_DEF_PHOTON_ODBC_SIMPLE,
                             (unsigned char *)&fieldDef,
                             sizeof(psoFieldDefinition),
                             &dataDefHandle );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoDataDefCreate error: %s\n", __LINE__, msg );
         return -1;
      }

      rc = psoCreateMap( session1,
                         mapName,
                         strlen(mapName),
                         &def,
                         dataDefHandle,
                         keyDefHandle );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoCreateMap error: %s\n", __LINE__, msg );
         return -1;
      }
      /* Commit the creation of the object */
      rc = psoCommit( session1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoCommit error: %s\n", __LINE__, msg );
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
            description, strlen(description), NULL );
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
      fprintf( stderr, "At line %d, psoDestroyObject error: %s\n", __LINE__, msg );
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
   unsigned int length;
   psoObjStatus status;
   
   if ( argc < 3 ) {
      fprintf( stderr, "Usage: %s iso_3166_data_file quasar_address\n", argv[0] );
      return 1;
   }
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   /* Initialize shared memory and create our session */
   rc = psoInit( argv[2], "HashMap" );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInit error: %d\n", __LINE__, rc );
      return 1;
   }

   rc = psoInitSession( &session1 );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInitSession error: %d\n", __LINE__, rc );
      return 1;
   }
   rc = psoInitSession( &session2 );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInitSession error: %d\n", __LINE__, rc );
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
   rc = psoCommit( session1 );
   if ( rc != 0 ) {
      psoErrorMsg(session1, msg, 256 );
      fprintf( stderr, "At line %d, psoCommit error: %s\n", __LINE__, msg );
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

