/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/photon.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   size_t len;
   psoObjectDefinition folderDef = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   psoObjectDefinition * pDefinition = NULL;
   
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
                              "/ascq",
                              strlen("/ascq"),
                              &folderDef );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   len = offsetof(psoObjectDefinition,fields) + 
      2 * sizeof(psoFieldDefinition);
   pDefinition = (psoObjectDefinition *)malloc( len );
   memset( pDefinition, 0, len );
   pDefinition->type = PSO_QUEUE;
   pDefinition->numFields = 1;
   pDefinition->fields[0].type = PSO_INTEGER;
   pDefinition->fields[0].length = 3;
   strcpy( pDefinition->fields[0].name, "Field_1" );
   
   /* End of preparation work. */

   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = 12345;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_BINARY;
   pDefinition->fields[0].length = 0;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pDefinition->fields[0].type = PSO_STRING;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_DECIMAL;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].precision = PSO_FIELD_MAX_PRECISION+1;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].precision = PSO_FIELD_MAX_PRECISION/2;
   pDefinition->fields[0].scale = pDefinition->fields[0].precision + 1;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_SCALE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_VAR_BINARY;
   pDefinition->fields[0].maxLength = 100;
   pDefinition->fields[0].minLength = 200;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_VAR_STRING;
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_BOOLEAN;
   strcpy( pDefinition->fields[0].name, "Field 1" );
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   strcpy( pDefinition->fields[0].name, "Field-1" );
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   strcpy( pDefinition->fields[0].name, "Field_é" );
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   strcpy( pDefinition->fields[0].name, "Field_1" );
   pDefinition->numFields = 2;
   pDefinition->fields[0].type = PSO_VAR_BINARY;
   pDefinition->fields[0].maxLength = 0;
   pDefinition->fields[0].minLength = 200;
   pDefinition->fields[1].type = PSO_BOOLEAN;
   strcpy( pDefinition->fields[1].name, "Field_2" );
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_BINARY;
   pDefinition->fields[0].length = 10;
   pDefinition->fields[1].type = PSO_BOOLEAN;
   strcpy( pDefinition->fields[1].name, "Field_1" );
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_DUPLICATE_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_BINARY;
   pDefinition->fields[0].length = 10;
   pDefinition->fields[1].type = PSO_BOOLEAN;
   strcpy( pDefinition->fields[1].name, "Field_2" );
   errcode = psoCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

