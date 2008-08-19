/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
#include <photon/vds>
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
   vdsProcess process;
   vdsSession session;
   vdsHashMap hashmap(session);
   string fname = "/cpp_hashmap_definition";
   string hname = fname + "/test";

   struct dummy data;
   char key[] = "My Key";
   size_t len;
   vdsObjectDefinition * pDef = NULL;
   vdsObjectDefinition * pDefHashMap = NULL;
   vdsObjectDefinition folderDef;
   
   memset( &folderDef, 0, sizeof(folderDef) );
   folderDef.type = VDS_FOLDER;
   
   len = offsetof( vdsObjectDefinition, fields ) + 
      5 * sizeof(vdsFieldDefinition);
   pDefHashMap = (vdsObjectDefinition *)calloc( len, 1 );
   pDefHashMap->type = VDS_HASH_MAP;
   pDefHashMap->numFields = 5;
   pDefHashMap->fields[0].type = VDS_INTEGER;
   pDefHashMap->fields[1].type = VDS_INTEGER;
   pDefHashMap->fields[2].type = VDS_STRING;
   pDefHashMap->fields[3].type = VDS_INTEGER;
   pDefHashMap->fields[4].type = VDS_VAR_BINARY;

   pDefHashMap->fields[0].length = 1;
   pDefHashMap->fields[1].length = 4;
   pDefHashMap->fields[2].length = 30;
   pDefHashMap->fields[3].length = 2;

   strcpy( pDefHashMap->fields[0].name, "field1" );
   strcpy( pDefHashMap->fields[1].name, "field2" );
   strcpy( pDefHashMap->fields[2].name, "field3" );
   strcpy( pDefHashMap->fields[3].name, "field4" );
   strcpy( pDefHashMap->fields[4].name, "field5" );
   
   pDefHashMap->key.type = VDS_KEY_VAR_STRING;
   pDefHashMap->key.minLength = 1;
   pDefHashMap->key.maxLength = 0;
   
   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session.CreateObject( fname, &folderDef );
      session.CreateObject( hname, pDefHashMap );
      hashmap.Open( hname );
      hashmap.Insert( key, strlen(key), &data, sizeof(data) );
   }
   catch( vdsException exc ) {
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
   catch( vdsException exc ) {
      if ( exc.ErrorCode() != VDS_NULL_POINTER ) {
         cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
         return 1;
      }
   }

   // End of invalid args. This call should succeed.
   try {
      hashmap.Definition( &pDef );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   if ( memcmp( pDefHashMap, pDef, len ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

