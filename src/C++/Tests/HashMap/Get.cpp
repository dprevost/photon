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
   Session session;
   HashMap hashmap;
   string fname = "/cpp_hashmap_get";
   string hname = fname + "/test";

   const char * key  = "My Key";
   const char * data = "My Data";
   uint32_t length;
   char buffer[50];
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARBINARY, 20 };
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
      session.Init();
      session.CreateFolder( fname );

      DataDefinition dataDefObj( session, 
                                 "cpp_hashmap_get",
                                 PSO_DEF_PHOTON_ODBC_SIMPLE,
                                 (unsigned char *)fields,
                                 sizeof(psoFieldDefinition) );
      KeyDefinition keyDefObj( session, 
                               "cpp_hashmap_get",
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)&keyDef,
                               sizeof(psoKeyFieldDefinition) );

      session.CreateMap( hname, mapDef, dataDefObj, keyDefObj );

      hashmap.Open( session, hname );
      hashmap.Insert( key, 6, data, 7 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try { 
      hashmap.Get( NULL, 6, buffer, 50, length );
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
      hashmap.Get( key, 0, buffer, 50, length );
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
      hashmap.Get( key, 6, NULL, 50, length );
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
      hashmap.Get( key, 6, buffer, 2, length );
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
      hashmap.Get( key, 6, buffer, 50, length );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   if ( memcmp( buffer, data, 7 ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

