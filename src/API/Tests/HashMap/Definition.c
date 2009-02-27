/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/photon.h>
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
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   struct dummy * data1 = NULL;
   char key[] = "My Key";
   size_t lenData;

   psoObjectDefinition folderDef = { PSO_FOLDER, PSO_DEF_USER_DEFINED };
   psoObjectDefinition hashMapDef = { PSO_HASH_MAP, PSO_DEF_USER_DEFINED };
   psoKeyDefinition keyDef = { PSO_KEY_VAR_STRING, 0, 1, 0 };

   psoFieldDefinition fields[5] = {
      { "field1", PSO_INTEGER,     1, 0, 0, 0, 0 },
      { "field2", PSO_INTEGER,     4, 0, 0, 0, 0 },
      { "field3", PSO_STRING,     30, 0, 0, 0, 0 },
      { "field4", PSO_INTEGER,     2, 0, 0, 0, 0 },
      { "field5", PSO_VAR_BINARY,  0, 0, 0, 0, 0 }
   };
   
   psoFieldDefinition retFields[5];
   psoObjectDefinition retDef;
   psoKeyDefinition retKeyDef;
   
   memset( &retDef, 0, sizeof(psoObjectDefinition) );
   memset( &retFields, 0, 5*sizeof(psoFieldDefinition) );
   memset( &retKeyDef, 0, sizeof(psoKeyDefinition) );
   
   lenData = offsetof(struct dummy, bin) + 10;
   data1 = (struct dummy *)malloc( lenData );
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], 0 );
   }
   else {
      errcode = psoInit( "10701", 0 );
   }
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoInitSession( &sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle,
                              "/ahmd",
                              strlen("/ahmd"),
                              &folderDef,
                              NULL,
                              0,
                              NULL,
                              0 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle,
                              "/ahmd/test",
                              strlen("/ahmd/test"),
                              &hashMapDef,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyDefinition),
                              (unsigned char *)fields,
                              5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoHashMapOpen( sessionHandle,
                           "/ahmd/test",
                           strlen("/ahmd/test"),
                           &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoHashMapInsert( objHandle, key, strlen(key), data1, lenData );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoHashMapDefinition( NULL, &retDef,
                                   (unsigned char *)&retKeyDef, 
                                   sizeof(psoKeyDefinition),
                                   (unsigned char *)retFields,
                                   5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoHashMapDefinition( objHandle, NULL,
                                   (unsigned char *)&retKeyDef,
                                   sizeof(psoKeyDefinition),
                                   (unsigned char *)retFields,
                                   5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoHashMapDefinition( objHandle, &retDef,
                                   NULL,
                                   0,
                                   (unsigned char *)retFields,
                                   5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( &hashMapDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( fields, retFields, 5*sizeof(psoFieldDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoHashMapDefinition( objHandle, &retDef,
                                   (unsigned char *)&retKeyDef,
                                   sizeof(psoKeyDefinition),
                                   NULL,
                                   0 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   if ( memcmp( &hashMapDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( &keyDef, &retKeyDef, sizeof(psoKeyDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoHashMapDefinition( objHandle, &retDef,
                                   (unsigned char *)&retKeyDef,
                                   sizeof(psoKeyDefinition),
                                   (unsigned char *)retFields,
                                   5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   if ( memcmp( &hashMapDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( &keyDef, &retKeyDef, sizeof(psoKeyDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( fields, retFields, 5*sizeof(psoFieldDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoHashMapDefinition( objHandle, &retDef,
                                   (unsigned char *)&retKeyDef,
                                   sizeof(psoKeyDefinition),
                                   NULL,
                                   0 );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

