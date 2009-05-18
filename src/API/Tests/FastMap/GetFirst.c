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
#include "API/FastMap.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE objHandle, sessionHandle, objHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   char buffer[200];
   char buffKey[50];
   uint32_t dataLength, keyLength;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
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
                              "/api_fast_map_getfirst",
                              strlen("/api_fast_map_getfirst") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_fastmap_get_first",
                              strlen("api_fastmap_get_first"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoDataDefCreate( sessionHandle,
                               "api_fastmap_get_first",
                               strlen("api_fastmap_get_first"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_getfirst/test",
                           strlen("/api_fast_map_getfirst/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_getfirst/test",
                             strlen("/api_fast_map_getfirst/test"),
                             &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapOpen( sessionHandle,
                             "/api_fast_map_getfirst/test",
                             strlen("/api_fast_map_getfirst/test"),
                             &objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoFastMapGetFirst( NULL,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 NULL,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 2,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 NULL,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 2,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 NULL,
                                 &dataLength );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle2,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_IS_EMPTY ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( buffer, data, 7 ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapClose( objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoCommit( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapGetFirst( objHandle2,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle2,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

