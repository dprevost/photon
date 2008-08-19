/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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
#include <photon/vds.h>
#include "Tests/PrintError.h"
#include "API/HashMap.h"

const bool expectedToPass = true;

struct dummy {
   char c;
   uint32_t u32;
   char str[30];
   uint16_t u16;
   unsigned char bin[1];
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE sessionHandle, objHandle;
   int errcode;
   struct dummy * data1 = NULL;
   char key[] = "My Key";
   size_t lenData, len;
   vdsObjectDefinition * pDef = NULL;
   vdsObjectDefinition * pDefHashMap = NULL;
   vdsObjectDefinition folderDef = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };

   lenData = offsetof(struct dummy, bin) + 10;
   data1 = (struct dummy *)malloc( lenData );
   
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
   
   if ( argc > 1 ) {
      errcode = vdsInit( argv[1], 0 );
   }
   else {
      errcode = vdsInit( "10701", 0 );
   }
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsInitSession( &sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/ahmd",
                              strlen("/ahmd"),
                              &folderDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/ahmd/test",
                              strlen("/ahmd/test"),
                              pDefHashMap );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapOpen( sessionHandle,
                           "/ahmd/test",
                           strlen("/ahmd/test"),
                           &objHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapInsert( objHandle, key, strlen(key), data1, lenData );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = vdsHashMapDefinition( NULL, &pDef );
   if ( errcode != VDS_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapDefinition( objHandle, NULL );
   if ( errcode != VDS_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = vdsHashMapDefinition( objHandle, &pDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   if ( memcmp( pDefHashMap, pDef, len ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = vdsExitSession( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsHashMapDefinition( objHandle, &pDef );
   if ( errcode != VDS_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

