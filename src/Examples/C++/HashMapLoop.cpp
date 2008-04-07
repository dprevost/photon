/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 * 
 * This code is in the public domain.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

// This example: 
//    We create a hash map and show how to iterate through it.

#include "iso_3166.h"

#ifndef PATH_MAX
#  define PATH_MAX 4096 // Safe enough on most systems I would think
#endif

// Some globals to make our life simpler. 
// Note: the destructor of these objects will cleanup/close so no need
// for explicit calls to terminate our access.
vdsProcess process;
vdsSession session;
vdsHashMap theMap( session );
string mapName = "My Hash Map Loop";

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int createMap()
{
   int rc;
   char countryCode[2];
   char description[100];
  
   // If the map already exists, we remove it.
   try { 
      session.DestroyObject( mapName );
      session.Commit();
   }
   catch ( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_NO_SUCH_OBJECT ) {
         cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
         return 1;
      }
   }

   try { 
      session.CreateObject( mapName, VDS_HASH_MAP );
      session.Commit();
      theMap.Open( mapName );
      /*
       * rc < 0 -> error
       * rc = 0 -> nothing read - EOF
       * rc > 0 -> new data
       */
      rc = readData( countryCode, description );
      while ( rc > 0 ) {
         theMap.Insert( countryCode, 2, description, strlen(description) );

         rc = readData( countryCode, description );
      }
   }
   catch ( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char *argv[] )
{
   int rc;
   char countryCode[3];
   char description[81];
   size_t keyLength, dataLength;
   
   if ( argc < 3 ) {
      cerr << "Usage: " << argv[0] << " iso_3166_data_file watchdog_address" << endl;
      return 1;
   }
   
   rc = openData( argv[1] );
   if ( rc != 0 ) return 1;
   
   // Initialize the vds and create our session
   try {
      process.Init( argv[2] );
      session.Init();
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }
   
   // Create a hash map object (and populate it)
   rc = createMap();
   if ( rc != 0 ) { return 1; }
   cout << "Map created" << endl;

   try {
      rc = theMap.GetFirst( countryCode, 2, description, 80, 
                            &keyLength, &dataLength );
      while ( rc == VDS_OK ) {
         countryCode[keyLength] = 0;
         description[dataLength] = 0;
         cout << "Country code: " << countryCode << ", country: " << 
            description << endl;

         rc = theMap.GetNext( countryCode, 2, description, 80, 
                              &keyLength, &dataLength );
      }
      if ( rc != VDS_REACHED_THE_END ) {
         cerr << "At line " << __LINE__ << ", Hash Map loop abnormal error: " <<
            rc << endl;
         return 1;
      }
   }
   catch( vdsException exc ) {
      cerr << "At line " << __LINE__ << ", " << exc.Message() << endl;
      return 1;
   }
   
   if ( fp != NULL ) fclose( fp );

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

