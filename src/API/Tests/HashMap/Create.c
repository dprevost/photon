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
   psoKeyDefinition keyDef;
   
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
                              "/api_hashmap_create",
                              strlen("/api_hashmap_create"),
                              &def,
                              NULL,
                              0,
                              NULL,
                              0 );
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

   /* Invalid definition. */
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   memset( &keyDef, 0, sizeof(psoKeyDefinition) );
   memset( fields, 0, 2*sizeof(psoFieldDefinition) );
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "my_map",
                                    strlen("my_map"),
                                    &definition,
                                    (unsigned char *)&keyDef,
                                    sizeof(psoKeyDefinition),
                                    (unsigned char *)fields,
                                    2*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   definition.type = PSO_HASH_MAP;   
   errcode = psoFolderCreateObject( folderHandle,
                                    "my_map",
                                    strlen("my_map"),
                                    &definition,
                                    (unsigned char *)&keyDef,
                                    sizeof(psoKeyDefinition),
                                    NULL,
                                    0 );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateObject( folderHandle,
                                    "my_map",
                                    strlen("my_map"),
                                    &definition,
                                    NULL,
                                    0,
                                    (unsigned char *)fields,
                                    2*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* End of invalid args. This call should succeed. */
   errcode = psoFolderCreateObject( folderHandle,
                                    "my_map",
                                    strlen("my_map"),
                                    &def,
                                    (unsigned char *)&keyDef,
                                    sizeof(psoKeyDefinition),
                                    NULL,
                                    0 );
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
                                    "my_map2",
                                    strlen("my_map2"),
                                    &def,
                                    (unsigned char *)&keyDef,
                                    sizeof(psoKeyDefinition),
                                    (unsigned char *)fields,
                                    2*sizeof(psoFieldDefinition) );
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
   
   errcode = psoFolderCreateObject( folderHandle,
                                    "my_map3",
                                    strlen("my_map3"),
                                    &def,
                                    (unsigned char *)&keyDef,
                                    sizeof(psoKeyDefinition),
                                    (unsigned char *)fields,
                                    2*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

