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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, folderHandle;
   int errcode;
   psoObjectDefinition def = { PSO_FOLDER, 0 };
   psoObjectDefinition definition;
   psoFieldDefinition fields[2];
   
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
                              "/aqcr",
                              strlen("/aqcr"),
                              &def,
                              NULL,
                              NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/aqcr",
                            strlen("/aqcr"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid definition. */
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   memset( fields, 0, 2*sizeof(psoFieldDefinition) );
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   definition.type = PSO_QUEUE;   
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_NUM_FIELDS ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   definition.numFields = 1;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_INTEGER;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].length = 5;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   fields[0].length = 4;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_BINARY;
   fields[0].length = 0;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_STRING;
   fields[0].length = 0;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_VAR_BINARY;
   fields[0].minLength = 100;
   fields[0].maxLength =  90;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_VAR_STRING;
   fields[0].minLength = 100;
   fields[0].maxLength =  90;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_DECIMAL;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_DECIMAL;
   fields[0].precision = 50;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_PRECISION ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   fields[0].type = PSO_DECIMAL;
   fields[0].precision = 5;
   fields[0].scale = 6;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_SCALE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   definition.numFields = 2;
   fields[0].type = PSO_VAR_STRING;
   fields[0].minLength = 10;
   fields[1].type = PSO_BINARY;
   fields[1].length = 20;
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &definition,
                                    NULL,
                                    fields );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr",
                                    strlen("aqcr"),
                                    &def,
                                    NULL,
                                    NULL );
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
                                    "aqcr2",
                                    strlen("aqcr2"),
                                    &def,
                                    NULL,
                                    NULL );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/aqcr",
                            strlen("/aqcr"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   psoExit();
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "aqcr3",
                                    strlen("aqcr3"),
                                    &def,
                                    NULL,
                                    NULL );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

