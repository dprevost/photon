/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

// This example: 
//    We create a hash map and play with it. We do it using two sessions to
//    show the idea behind sessions.

#define _CRT_SECURE_NO_DEPRECATE

#include "iso_3166.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096 // Safe enough on most systems I would think
#endif

// Some globals to make our life simpler
// Note: the destructor of these objects will cleanup/close so no need
// for explicit calls to terminate our access.
Process process;
Session session1, session2;
HashMap map1, map2;
string mapName = "MyHashMap";

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int createMap()
{
   int rc;
   char countryCode[2];
   char description[100];

   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, 0 };

   // If the map already exists, we remove it.
   try { 
      session1.DestroyObject( mapName );
      session1.Commit();
   }
   catch ( Exception exc ) {
      if ( exc.ErrorCode() != PSO_NO_SUCH_OBJECT ) {
         cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
         return 1;
      }
   }
   
   try {
      /*
       * We use the default key and data definition to make our life simpler.
       * See HashMapLoop.cpp for a different approach.
       */
      session1.CreateMap( mapName,
                          def,
                          "Default",
                          "Default" );
      session1.Commit();
      map1.Open( session1, mapName );
      /*
       * rc < 0 -> error
       * rc = 0 -> nothing read - EOF
       * rc > 0 -> new data
       */
      rc = readData( countryCode, description );
      while ( rc > 0 ) {
         map1.Insert( countryCode, 2, description, strlen(description) );

         rc = readData( countryCode, description );
      }
   }
   catch ( Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char *argv[] )
{
   int rc;
   char description[80];
   size_t length;
   psoObjStatus status;
   
   if ( argc < 3 ) {
      cerr << "Usage: " << argv[0] << " iso_3166_data_file quasar_address" << endl;
      return 1;
   }
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   // Initialize the shared memory and create our session
   try {
      process.Init( argv[2] );
      session1.Init();
      session2.Init();
   }
   catch( Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }
   
   // Create a hash map object (and populate it)
   rc = createMap();
   if ( rc != 0 ) { return 1; }
   cout << "Map created" << endl;
   
   try { map2.Open( session2, mapName ); }
   catch( Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }

   // The data is inserted but not committed yet - failure is expected
   rc = 0;
   try { map2.Get("FM", 2, description, 80, length ); }
   catch( Exception exc ) {
      rc = exc.ErrorCode();
//      cerr << "Code = " << exc.ErrorCode() << endl;
      if ( rc != PSO_ITEM_IS_IN_USE ) {
         cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
         return 1;
      }
   }
   if ( rc == 0 ) {
      cerr << "At line " << __LINE__ << ", unexpected success in HashMap::Get!" << endl;
      return 1;
   }
   
   try {
      session1.Commit();
      map2.Status( status );
      cout << "Number of countries in the hash map = " << status.numDataItem << endl;
      memset( description, 0, 80 );
      map2.Get( "FM", 2, description, 80, length );
      cout << "Country code: FM, country: " << description << endl;
   }
   catch( Exception exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }
   
   if ( fp != NULL ) fclose( fp );

   return 0;

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

