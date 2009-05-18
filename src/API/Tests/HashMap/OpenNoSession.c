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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE objHandle,  sessionHandle;
   int errcode;
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
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
                              "/api_hashmap_open_nosession",
                              strlen("/api_hashmap_open_nosession") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "API_Hashmap_OpenNoSession",
                              strlen("API_Hashmap_OpenNoSession"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoDataDefCreate( sessionHandle,
                               "API_Hashmap_OpenNoSession",
                               strlen("API_Hashmap_OpenNoSession"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateMap( sessionHandle,
                           "/api_hashmap_open_nosession/test",
                           strlen("/api_hashmap_open_nosession/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = psoCommit( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoExitSession( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * sessionHandle is a pointer to deallocated memory. We either get the
    * error or we crash!
    */
   errcode = psoHashMapOpen( sessionHandle,
                            "/api_hashmap_open_nosession/test",
                            strlen("/api_hashmap_open_nosession/test"),
                            &objHandle );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

#if defined(WIN32)
   exit(3);
#else
   abort();
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

