/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/photon.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, folderHandle;
   int errcode;
   psoObjectDefinition def = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   psoObjectDefinition *pDefinition;
   
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
                              "/ahmcr",
                              strlen("/ahmcr"),
                              &def );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/ahmcr",
                            strlen("/ahmcr"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition = (psoObjectDefinition *)
      calloc( offsetof(psoObjectDefinition,fields) +
              7 * sizeof(psoFieldDefinition), 1 );
   if ( pDefinition == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Invalid definition. */
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->type = PSO_HASH_MAP;   
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.type = PSO_KEY_INTEGER;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pDefinition->key.type = PSO_KEY_STRING;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.type = PSO_KEY_VAR_STRING;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.length = 100;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   pDefinition->key.length = 0;
   pDefinition->key.minLength = 10;
   pDefinition->key.maxLength = 8;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->key.minLength = 1;
   pDefinition->key.maxLength = 0;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_NUM_FIELDS ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->numFields = 1;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_INTEGER;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].length = 5;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pDefinition->fields[0].length = 4;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_BINARY;
   pDefinition->fields[0].length = 0;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_STRING;
   pDefinition->fields[0].length = 0;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_VAR_BINARY;
   pDefinition->fields[0].minLength = 100;
   pDefinition->fields[0].maxLength =  90;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_VAR_STRING;
   pDefinition->fields[0].minLength = 100;
   pDefinition->fields[0].maxLength =  90;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_DECIMAL;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_DECIMAL;
   pDefinition->fields[0].precision = 50;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->fields[0].type = PSO_DECIMAL;
   pDefinition->fields[0].precision = 5;
   pDefinition->fields[0].scale = 6;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_SCALE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   pDefinition->numFields = 2;
   pDefinition->fields[0].type = PSO_VAR_STRING;
   pDefinition->fields[0].minLength = 10;
   pDefinition->fields[1].type = PSO_BINARY;
   pDefinition->fields[1].length = 20;
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    pDefinition );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &def );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the folder and try to act on it */
   
   errcode = psoFolderClose( folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr2",
                                    strlen("ahmcr2"),
                                    &def );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/ahmcr",
                            strlen("/ahmcr"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   psoExit();
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr3",
                                    strlen("ahmcr3"),
                                    &def );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

