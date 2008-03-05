/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
vdsProcess process;
vdsSession session1, session2;
vdsHashMap * map1 = NULL, *map2 = NULL;
string mapName = "My Hash Map";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void cleanup()
{
   if ( map1 ) {
      map1->Close();
      delete map1;
      map1 = NULL;
   }
   if ( map2 ) {
      map2->Close();
      delete map2;
      map2 = NULL;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int createMap()
{
   int rc;
   char countryCode[2];
   char description[100];

   // If the map already exists, we remove it.
   try { 
      session1.DestroyObject( mapName );
      session1.Commit();
   }
   catch ( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_NO_SUCH_OBJECT ) {
         cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
         return -1;
      }
   }
   
   try { 
      session1.CreateObject( mapName, VDS_HASH_MAP );
      session1.Commit();
      map1->Open( mapName );
      /*
       * rc < 0 -> error
       * rc = 0 -> nothing read - EOF
       * rc > 0 -> new data
       */
      rc = readData( countryCode, description );
      while ( rc > 0 )
      {
         map1->Insert( countryCode, 2, description, strlen(description) );

         rc = readData( countryCode, description );
      }
   }
   catch ( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char *argv[] )
{
   int rc;
   char description[80];
   size_t length;
   vdsObjStatus status;
   
   if ( argc < 3 )
   {
      fprintf( stderr, "Usage: %s iso_3166_data_file watchdog_address\n", argv[0] );
      return 1;
   }
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   /* Initialize vds and create our session */
   try {
      process.Init( argv[2] );
      session1.Init();
      session2.Init();
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }
   map1 = new vdsHashMap( session1 );
   map2 = new vdsHashMap( session2 );
   
   /* Create a hash map object */
   rc = createMap();
   if ( rc != 0 ) { cleanup(); return 1; }
   cout << "Map created" << endl;
   
   try { map2->Open( mapName ); }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return -1;
   }

   /* The data is inserted but not committed yet - failure is expected */
   rc = 0;
   try { map2->Get("FM", 2, description, 80, &length ); }
   catch( vdsException exc ) {
      rc = exc.ErrorCode();
      cerr << "Code = " << exc.ErrorCode() << endl;
      if ( rc != VDS_ITEM_IS_IN_USE ) {
         cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
         cleanup();
         return -1;
      }
   }
   if ( rc == 0 ) {
      cerr << "At line " << __LINE__ << ", unexpected success in HashMap::Get!" << endl;
      cleanup();
      return -1;
   }
   
   try {
      session1.Commit();
      map2->Status( &status );
      cout << "Number of countries in the hash map = " << status.numDataItem << endl;
      memset( description, 0, 80 );
      map2->Get( "FM", 2, description, 80, &length );
      cout << "Country code: FM, country: " << description << endl;
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      cleanup();
      return -1;
   }
   
   if ( fp != NULL )
      fclose( fp );

   cleanup();
   
   return 0;

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

