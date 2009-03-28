/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
   psoObjectDefinition def = { PSO_FOLDER, PSO_DEF_NONE, PSO_DEF_NONE };
   psoObjectDefinition definition;
   psoKeyDefinition keyDef = { "MyKey", PSO_KEY_LONGVARCHAR, 0 };

   psoFieldDefinition fields[5] = {
      { "field1", PSO_TINYINT,   {0} },
      { "field2", PSO_INTEGER,   {0} },
      { "field3", PSO_CHAR,      {30} },
      { "field4", PSO_SMALLINT,  {0} },
      { "field5", PSO_LONGVARBINARY, {0} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;

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
                              "/ammcr",
                              strlen("/ammcr"),
                              &def,
                              NULL,
                              NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/ammcr",
                            strlen("/ammcr"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "Definition",
                              strlen("Definition"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyDefinition),
                              &keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoDataDefCreate( sessionHandle,
                               "Definition",
                               strlen("Definition"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid definition. */
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );

   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &definition,
                                    keyDefHandle,
                                    dataDefHandle );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   definition.type = PSO_FAST_MAP;
   definition.fieldDefType = PSO_DEF_USER_DEFINED;
   definition.keyDefType   = PSO_DEF_USER_DEFINED;

   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &definition,
                                    NULL,
                                    dataDefHandle );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &definition,
                                    keyDefHandle,
                                    NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &definition,
                                    keyDefHandle,
                                    keyDefHandle );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr",
                                    strlen("ahmcr"),
                                    &definition,
                                    keyDefHandle,
                                    dataDefHandle );
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
                                    &def,
                                    keyDefHandle,
                                    dataDefHandle );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/ammcr",
                            strlen("/ammcr"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   psoExit();
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "ahmcr3",
                                    strlen("ahmcr3"),
                                    &def,
                                    keyDefHandle,
                                    dataDefHandle );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

