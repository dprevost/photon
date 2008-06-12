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
#include <vdsf/vds.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE sessionHandle;
   int errcode;
   size_t len;
   vdsObjectDefinition folderDef = { 
      VDS_FOLDER, 
      0, 
      { "", 0, 0, 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   vdsObjectDefinition * pDefinition = NULL;
   
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
                              "/ascq",
                              strlen("/ascq"),
                              &folderDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   len = offsetof(vdsObjectDefinition,fields) + 
      2 * sizeof(vdsFieldDefinition);
   pDefinition = (vdsObjectDefinition *)malloc( len );
   memset( pDefinition, 0, len );
   pDefinition->type = VDS_QUEUE;
   pDefinition->numFields = 1;
   pDefinition->fields[0].type = VDS_INTEGER;
   pDefinition->fields[0].length = 3;
   strcpy( pDefinition->fields[0].name, "Field_1" );
   
   /* End of preparation work. */

   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = 12345;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_BINARY;
   pDefinition->fields[0].length = 0;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pDefinition->fields[0].type = VDS_STRING;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_DECIMAL;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].precision = VDS_FIELD_MAX_PRECISION+1;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].precision = VDS_FIELD_MAX_PRECISION/2;
   pDefinition->fields[0].scale = pDefinition->fields[0].precision + 1;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_SCALE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_VAR_BINARY;
   pDefinition->fields[0].maxLength = 100;
   pDefinition->fields[0].minLength = 200;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_VAR_STRING;
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_BOOLEAN;
   strcpy( pDefinition->fields[0].name, "Field 1" );
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   strcpy( pDefinition->fields[0].name, "Field-1" );
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   strcpy( pDefinition->fields[0].name, "Field_é" );
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   strcpy( pDefinition->fields[0].name, "Field_1" );
   pDefinition->numFields = 2;
   pDefinition->fields[0].type = VDS_VAR_BINARY;
   pDefinition->fields[0].maxLength = 0;
   pDefinition->fields[0].minLength = 200;
   pDefinition->fields[1].type = VDS_BOOLEAN;
   strcpy( pDefinition->fields[1].name, "Field_2" );
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_BINARY;
   pDefinition->fields[0].length = 10;
   pDefinition->fields[1].type = VDS_BOOLEAN;
   strcpy( pDefinition->fields[1].name, "Field_1" );
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_DUPLICATE_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_BINARY;
   pDefinition->fields[0].length = 10;
   pDefinition->fields[1].type = VDS_BOOLEAN;
   strcpy( pDefinition->fields[1].name, "Field_2" );
   errcode = vdsCreateObject( sessionHandle,
                              "/ascq/test",
                              strlen("/ascq/test"),
                              pDefinition );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

