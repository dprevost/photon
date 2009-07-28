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

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
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

   errcode = psoInit( "10701", "GetFirst" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_fast_map_getfirst",
                              strlen("/api_fast_map_getfirst") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_fastmap_get_first",
                              strlen("api_fastmap_get_first"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoDataDefCreate( sessionHandle,
                               "api_fastmap_get_first",
                               strlen("api_fastmap_get_first"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_getfirst/test",
                           strlen("/api_fast_map_getfirst/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_getfirst/test",
                             strlen("/api_fast_map_getfirst/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapOpen( sessionHandle,
                             "/api_fast_map_getfirst/test",
                             strlen("/api_fast_map_getfirst/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );

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
   assert_true( errcode == PSO_OK );
   if ( memcmp( buffer, data, 7 ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapClose( objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   errcode = psoFastMapGetFirst( objHandle2,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

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

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


