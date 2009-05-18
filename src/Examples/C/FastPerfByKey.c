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
PSO_HANDLE session1 = NULL;
PSO_HANDLE map1 = NULL;
const char* mapName = "MyHashMapLoop";

char keys[244][100];

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   if ( map1 != NULL ) psoHashMapClose( map1 );
   
   if ( session1 != NULL ) psoExitSession( session1 );

   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int createMap()
{
   int rc, count = 0;
   char msg[256];
   struct dataStruct {
      char countryCode[2];
      char description[100];
   } data;
   PSO_HANDLE keyDefHandle, dataDefHandle;
   
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
   
   /*
    * The content of the hash map is simple: a fixed length key, the country 
    * code, and the country name (a variable string - max length of 100).
    */
   psoKeyFieldDefinition keyDef = { "CountryName", PSO_KEY_VARCHAR, 100 };
   psoFieldDefinition fieldDef[2] = { 
      { "CountryCode", PSO_CHAR, {2} },
      { "CountryName", PSO_VARCHAR, {100} }
   };

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
                            "Country Name",
                            strlen("Country Name"),
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
                             "Country Data",
                             strlen("Country Data"),
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

      rc = psoFastMapEdit( session1, mapName, strlen(mapName), &map1 );
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
      rc = readData( data.countryCode, data.description );
      while ( rc > 0 ) {
         rc = psoFastMapInsert( map1, data.description, strlen(data.description), 
            &data, sizeof(struct dataStruct), NULL );
         if ( rc != 0 ) {
            psoErrorMsg(session1, msg, 256 );
            fprintf( stderr, "At line %d, psoHashMapInsert error: %s\n", __LINE__, msg );
            return -1;
         }
         strcpy( keys[count], data.description );
         count++;
         
         rc = readData( data.countryCode, data.description );
      }
      rc = psoFastMapClose( map1 );
      rc = psoCommit( session1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoCommit error: %s\n", __LINE__, msg );
         return -1;
      }
      
      rc = psoFastMapOpen( session1, mapName, strlen(mapName), &map1 );
      if ( rc != 0 ) {
         psoErrorMsg(session1, msg, 256 );
         fprintf( stderr, "At line %d, psoHashMapOpen error: %s\n", __LINE__, msg );
         return -1;
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
   struct dataStruct {
      char countryCode[2];
      char description[100];
   } data;
//   char countryCode[3];
//   char description[81];
//   char msg[256];
   unsigned int dataLength;
   int i, j, k = 0;
   
   if ( argc < 3 ) {
      fprintf( stderr, "Usage: %s iso_3166_data_file quasar_address\n", argv[0] );
      return 1;
   }
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   /* Initialize shared memory and create our session */
   rc = psoInit( argv[2], "FastPerfByKey" );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInit error: %d\n", __LINE__, rc );
      return 1;
   }

   rc = psoInitSession( &session1 );
   if ( rc != 0 ) {
      fprintf( stderr, "At line %d, psoInitSession error: %d\n", __LINE__, rc );
      return 1;
   }
   
   /* Create a hash map object */
   rc = createMap();
   if ( rc != 0 ) { cleanup(); return 1; }
   fprintf( stderr, "Map created\n" );
   
   for ( i = 0, k = 0; i < 100000; ++i ) {
      for ( j = 0; j < 244; ++j ) {
         rc = psoFastMapGet( map1, keys[j], strlen(keys[j]),
                             &data, sizeof(struct dataStruct), &dataLength );
         if ( rc != 0 ) {
            fprintf( stderr, "At line %d, psoFastMapGetByKey error: %d\n", __LINE__, rc );
            return 1;
         }
         ++k;
      }
   }
   
   fprintf( stderr, "Num of loops = %d\n", k );
   
   if ( fp != NULL ) fclose( fp );

   cleanup();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

