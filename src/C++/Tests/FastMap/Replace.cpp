/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include <photon/photon>
#include <iostream>

using namespace std;
using namespace pso;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session1, session2;
   FastMapEditor map1(session1);
   FastMap map2(session2);
   string fname = "/cpp_fastmap_replace";
   string hname = fname + "/test";

   const char * key  = "My Key";
   const char * data1 = "My Data1";
   const char * data2 = "My Data 2";
   char buffer[50];
   uint32_t length;
   psoObjectDefinition folderDef;
   psoObjectDefinition mapDef = { 
      PSO_FAST_MAP,
      1, 
      { PSO_KEY_VAR_BINARY, 0, 1, 20 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };

   memset( &folderDef, 0, sizeof folderDef );
   folderDef.type = PSO_FOLDER;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session1.Init();
      session2.Init();
      session1.CreateObject( fname, folderDef );
      session1.CreateObject( hname, mapDef );
      map1.Open( hname );
      // Insert after commit for first test below
      map1.Insert( key, 6, data1, strlen(data1) );
      map1.Close();
      session1.Commit();
      map1.Open( hname );
      map2.Open( hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

#if 0
   // Must commit the insert before replacing
   try {
      map1.Replace( key, 6, data2, strlen(data2) );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_ITEM_IS_IN_USE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      session1.Commit();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
#endif

   // Invalid arguments to tested function.

   try {
      map1.Replace( NULL, 6, data2, strlen(data2) );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map1.Replace( key, 0, data2, strlen(data2) );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map1.Replace( key, 6, NULL, strlen(data2) );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map1.Replace( key, 6, data2, 0 );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed.
   try {
      map1.Replace( key, 6, data2, strlen(data2) );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   /*
    * Additional stuff to check while the Replace() is uncommitted:
    *  - first session get new value.
    *  - second session get old value
    *  - cannot modify it from any session.
    */
   try {
      map1.Get( key, 6, buffer, 50, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( length != strlen(data2) ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( memcmp( buffer, data2, length ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   try {
      map2.Get( key, 6, buffer, 50, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( length != strlen(data1) ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( memcmp( buffer, data1, length ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   try {
      map1.Close();
      session1.Commit();
      session2.Commit();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      map2.Get( key, 6, buffer, 50, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( length != strlen(data2) ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( memcmp( buffer, data2, length ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

