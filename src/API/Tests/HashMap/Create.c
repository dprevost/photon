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
   psoObjectDefinition definition;
   psoFieldDefinition fields[2];
   psoKeyFieldDefinition keyDef;
   PSO_HANDLE keyDefHandle, dataDefHandle;
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
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

   errcode = psoCreateFolder( sessionHandle,
                              "/api_hashmap_create",
                              strlen("/api_hashmap_create") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/api_hashmap_create",
                            strlen("/api_hashmap_create"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_Create",
                              strlen("API_Hashmap_Create"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_Create",
                               strlen("API_Hashmap_Create"),
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
   
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &definition,
                                 dataDefHandle,
                                 keyDefHandle );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   definition.type = PSO_HASH_MAP;   
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &definition,
                                 dataDefHandle,
                                 NULL );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &definition,
                                 NULL,
                                 keyDefHandle );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map",
                                 strlen("my_map"),
                                 &definition,
                                 dataDefHandle,
                                 keyDefHandle );
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
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map2",
                                 strlen("my_map2"),
                                 &definition,
                                 dataDefHandle,
                                 keyDefHandle );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Reopen the folder, close the process and try to act on the session */

   errcode = psoFolderOpen( sessionHandle,
                            "/api_hashmap_create",
                            strlen("/api_hashmap_create"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   psoExit();
   
   errcode = psoFolderCreateMap( folderHandle,
                                 "my_map3",
                                 strlen("my_map3"),
                                 &definition,
                                 dataDefHandle,
                                 keyDefHandle );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

