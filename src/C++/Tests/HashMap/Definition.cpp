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
   psoProcess process;
   psoSession session;
   psoHashMap hashmap(session);
   string fname = "/cpp_hashmap_definition";
   string hname = fname + "/test";

   struct dummy data;
   char key[] = "My Key";
   size_t len;
   psoObjectDefinition * pDef = NULL;
   psoObjectDefinition folderDef;
   psoDefinition mapDef( 5, PSO_HASH_MAP );
   
   memset( &folderDef, 0, sizeof(folderDef) );
   folderDef.type = PSO_FOLDER;
   
   try {
      mapDef.AddKey( PSO_KEY_VAR_STRING, 0, 1, 0 );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   try {
      mapDef.ObjectType( PSO_HASH_MAP );
      mapDef.AddField( "field1", 6, PSO_INTEGER,    1, 0, 0, 0, 0 );
      mapDef.AddField( "field2", 6, PSO_INTEGER,    4, 0, 0, 0, 0 );
      mapDef.AddField( "field3", 6, PSO_STRING,    30, 0, 0, 0, 0 );
      mapDef.AddField( "field4", 6, PSO_INTEGER,    2, 0, 0, 0, 0 );
      mapDef.AddField( "field5", 6, PSO_VAR_BINARY, 0, 0, 0, 0, 0 );
   }
   catch( psoException exc ) {
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
      session.CreateObject( fname, folderDef );
      session.CreateObject( hname, mapDef.GetDef() );
      hashmap.Open( hname );
      hashmap.Insert( key, strlen(key), &data, sizeof(data) );
   }
   catch( psoException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      hashmap.Definition( NULL );
      // Should never come here
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }
   catch( psoException exc ) {
      if ( exc.ErrorCode() != PSO_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed.
   try {
      hashmap.Definition( &pDef );
   }
   catch( psoException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   len = offsetof( psoObjectDefinition, fields ) + 5 * sizeof(psoFieldDefinition);
   if ( memcmp( &mapDef.GetDef(), pDef, len ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

