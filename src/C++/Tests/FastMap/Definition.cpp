/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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

const bool expectedToPass = true;

struct dummy {
   char c;
   uint32_t u32;
   char str[30];
   uint16_t u16;
   unsigned char bin[10];
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   Process process;
   Session session;
   FastMap hashmap(session);
   string fname = "/cpp_fastmap_definition";
   string hname = fname + "/test";

   size_t len;
   psoBasicObjectDef folderDef;
   ObjDefinition mapDef( 5, PSO_FAST_MAP );
   ObjDefinition returnedDef;
   
   memset( &folderDef, 0, sizeof(folderDef) );
   folderDef.type = PSO_FOLDER;
   
   try {
      mapDef.AddKey( PSO_KEY_VAR_STRING, 0, 1, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      mapDef.AddField( "field1", 6, PSO_INTEGER,    1, 0, 0, 0, 0 );
      mapDef.AddField( "field2", 6, PSO_INTEGER,    4, 0, 0, 0, 0 );
      mapDef.AddField( "field3", 6, PSO_STRING,    30, 0, 0, 0, 0 );
      mapDef.AddField( "field4", 6, PSO_INTEGER,    2, 0, 0, 0, 0 );
      mapDef.AddField( "field5", 6, PSO_VAR_BINARY, 0, 0, 0, 0, 0 );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session.CreateObject( fname, folderDef, NULL );
      session.CreateObject( hname, mapDef );
      hashmap.Open( hname );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the server running?" << endl;
      return 1;
   }

   try {
      hashmap.Definition( returnedDef );
   }
   catch( pso::Exception exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }
   
   len = sizeof( psoBasicObjectDef );
   if ( memcmp( &mapDef.GetDef(), &returnedDef.GetDef(), len ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   len = 5 * sizeof(psoFieldDefinition);
   if ( memcmp( mapDef.GetFields(), returnedDef.GetFields(), len ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

