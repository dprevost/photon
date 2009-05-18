/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
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
   FastMapEditor * map1;
   FastMap       * map2;
   string fname = "/cpp_fastmap_insert";
   string hname = fname + "/test";

   const char * key  = "My Key";
   const char * data = "My Data";
   char buffer[20], keyBuff[20];
   uint32_t length, keyLength;
   int rc;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keys = { "MyKey", PSO_KEY_VARBINARY, 20 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };

   try {
      if ( argc > 1 ) {
         process.Init( argv[1], argv[0] );
      }
      else {
         process.Init( "10701", argv[0] );
      }
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   try {
      session1.Init();
      session2.Init();
      session1.CreateFolder( fname );

      DataDefinition dataDefObj( session1, 
                                 "cpp_fastmap_insert",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 (unsigned char *)fields,
                                 sizeof(psoFieldDefinition) );
      KeyDefinition keyDefObj( session1,
                               "cpp_fastmap_insert",
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)&keys,
                               sizeof(psoKeyFieldDefinition) );
      session1.CreateMap( hname, mapDef, dataDefObj, keyDefObj );
      session1.Commit();

      map1 = new FastMapEditor( session1, hname );
      map2 = new FastMap( session2, hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      map1->Insert( NULL, 6, data, strlen(data) );
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
      map1->Insert( key, 0, data, strlen(data) );
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
      map1->Insert( key, 6, NULL, strlen(data) );
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
      map1->Insert( key, 6, data, 0 );
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
      map1->Insert( key, 6, data, strlen(data) );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   /*
    * Additional stuff to check:
    *  - cannot get access to the item from second session.
    *  - can get access to the item from first session.
    *  - cannot modify it from first session.
    */
   try {
      map2->Get( key, strlen(key), buffer, 20, length );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NO_SUCH_ITEM ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   try {
      map1->Get( key, strlen(key), buffer, 20, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      rc = map1->GetFirst( keyBuff, 20, buffer, 20, keyLength, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   try {
      rc = map2->GetFirst( keyBuff, 20, buffer, 20, keyLength, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( rc != PSO_IS_EMPTY ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   try {
      map1->Close();
      session1.Commit();
      session2.Commit();
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      map2->Get( key, 6, buffer, 50, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( length != strlen(data) ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   if ( memcmp( buffer, data, length ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

