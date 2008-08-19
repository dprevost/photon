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
   vdsLifo queue(session);
   string fname = "/cpp_lifo_definition";
   string hname = fname + "/test";

   struct dummy data;
   size_t len;
   vdsObjectDefinition * pDef = NULL;
   vdsObjectDefinition * pDefQueue = NULL;
   vdsObjectDefinition folderDef;
   
   memset( &folderDef, 0, sizeof(folderDef) );
   folderDef.type = VDS_FOLDER;
   
   len = offsetof( vdsObjectDefinition, fields ) + 
      5 * sizeof(vdsFieldDefinition);
   pDefQueue = (vdsObjectDefinition *)calloc( len, 1 );
   pDefQueue->type = VDS_LIFO;
   pDefQueue->numFields = 5;
   pDefQueue->fields[0].type = VDS_INTEGER;
   pDefQueue->fields[1].type = VDS_INTEGER;
   pDefQueue->fields[2].type = VDS_STRING;
   pDefQueue->fields[3].type = VDS_INTEGER;
   pDefQueue->fields[4].type = VDS_VAR_BINARY;

   pDefQueue->fields[0].length = 1;
   pDefQueue->fields[1].length = 4;
   pDefQueue->fields[2].length = 30;
   pDefQueue->fields[3].length = 2;

   strcpy( pDefQueue->fields[0].name, "field1" );
   strcpy( pDefQueue->fields[1].name, "field2" );
   strcpy( pDefQueue->fields[2].name, "field3" );
   strcpy( pDefQueue->fields[3].name, "field4" );
   strcpy( pDefQueue->fields[4].name, "field5" );
   
   try {
      if ( argc > 1 ) {
         process.Init( argv[1] );
      }
      else {
         process.Init( "10701" );
      }
      session.Init();
      session.CreateObject( fname, &folderDef );
      session.CreateObject( hname, pDefQueue );
      queue.Open( hname );
      queue.Push( &data, sizeof(data) );
   }
   catch( vdsException exc ) {
      cerr << "Test failed in init phase, error = " << exc.Message() << endl;
      cerr << "Is the watchdog running?" << endl;
      return 1;
   }

   // Invalid arguments to tested function.

   try {
      queue.Definition( NULL );
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
      queue.Definition( &pDef );
   }
   catch( vdsException exc ) {
      cerr << "Test failed - line " << __LINE__ << ", error = " << exc.Message() << endl;
      return 1;
   }

   if ( memcmp( pDefQueue, pDef, len ) != 0 ) {
      cerr << "Test failed - line " << __LINE__ << endl;
      return 1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

