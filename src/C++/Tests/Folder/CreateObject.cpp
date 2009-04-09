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
   Folder *folder;
   string name = "/cpp_folder_create";
   string subname = "test";
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} } 
   };
   psoKeyDefinition keyDef = { "MyKey", PSO_KEY_VARBINARY, 20 };
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   DataDefinition dataDefObj;
   KeyDefinition keyDefObj;

   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   try {
      session.Init();
      session.CreateFolder( name );
      folder = new Folder( session, name );

      dataDefObj.Create( session, 
                         "Data Definition",
                         PSO_DEF_PHOTON_ODBC_SIMPLE,
                         (unsigned char *)fields,
                         sizeof(psoFieldDefinition) );
      keyDefObj.Create( session, 
                        "Key Definition",
                        PSO_DEF_PHOTON_ODBC_SIMPLE,
                        (unsigned char *)&keyDef,
                        sizeof(psoKeyDefinition) );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   // Invalid arguments to tested function.

   try {
      folder->CreateObject( "", mapDef, keyDefObj, dataDefObj );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_INVALID_LENGTH ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   mapDef.type = (psoObjectType)0;
   try {
      folder->CreateObject( subname, mapDef, keyDefObj, dataDefObj );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_WRONG_OBJECT_TYPE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   mapDef.type = PSO_HASH_MAP;

   try {
      folder->CreateObject( subname, mapDef, dataDefObj );
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_WRONG_OBJECT_TYPE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   // End of invalid args. This call should succeed.
   try {
      folder->CreateObject( subname, mapDef, keyDefObj, dataDefObj );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      dataDefObj.Close();
      folder->CreateObject( subname, mapDef, keyDefObj, dataDefObj );
   }
   catch( pso::Exception exc ) {
      if ( exc.ErrorCode() != PSO_NULL_HANDLE ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

