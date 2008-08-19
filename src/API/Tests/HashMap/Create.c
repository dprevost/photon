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
#include <photon/vds.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE sessionHandle, folderHandle;
   int errcode;
   vdsObjectDefinition def = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   vdsObjectDefinition *pDefinition;
   
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
                              "/ahmcr",
                              strlen("/ahmcr"),
                              &def );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFolderOpen( sessionHandle,
                            "/ahmcr",
                            strlen("/ahmcr"),
                            &folderHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition = (vdsObjectDefinition *)
      calloc( offsetof(vdsObjectDefinition,fields) +
              7 * sizeof(vdsFieldDefinition), 1 );
   if ( pDefinition == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Invalid definition. */
   
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->type = VDS_HASH_MAP;   
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.type = VDS_KEY_INTEGER;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pDefinition->key.type = VDS_KEY_STRING;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.type = VDS_KEY_VAR_STRING;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.length = 100;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   pDefinition->key.length = 0;
   pDefinition->key.minLength = 10;
   pDefinition->key.maxLength = 8;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.minLength = 1;
   pDefinition->key.maxLength = 0;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_NUM_FIELDS ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->numFields = 1;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_INTEGER;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].length = 5;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pDefinition->fields[0].length = 4;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_BINARY;
   pDefinition->fields[0].length = 0;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_STRING;
   pDefinition->fields[0].length = 0;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_VAR_BINARY;
   pDefinition->fields[0].minLength = 100;
   pDefinition->fields[0].maxLength =  90;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_VAR_STRING;
   pDefinition->fields[0].minLength = 100;
   pDefinition->fields[0].maxLength =  90;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_DECIMAL;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_DECIMAL;
   pDefinition->fields[0].precision = 50;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = VDS_DECIMAL;
   pDefinition->fields[0].precision = 5;
   pDefinition->fields[0].scale = 6;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_SCALE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->numFields = 2;
   pDefinition->fields[0].type = VDS_VAR_STRING;
   pDefinition->fields[0].minLength = 10;
   pDefinition->fields[1].type = VDS_BINARY;
   pDefinition->fields[1].length = 20;
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != VDS_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &def );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the folder and try to act on it */
   
   errcode = vdsFolderClose( folderHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr2",
                                    strlen("ahmcr2"),
                                    &def );
   if ( errcode != VDS_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Reopen the folder, close the process and try to act on the session */

   errcode = vdsFolderOpen( sessionHandle,
                            "/ahmcr",
                            strlen("/ahmcr"),
                            &folderHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   vdsExit();
   
   errcode = vdsFolderCreateObject( folderHandle,
                                    "ahmcr3",
                                    strlen("ahmcr3"),
                                    &def );
   if ( errcode != VDS_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

