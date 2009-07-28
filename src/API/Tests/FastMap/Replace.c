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
   PSO_HANDLE objHandle, sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data1 = "My Data1";
   const char * data2 = "My Data 2";
   char buffer[20];
   uint32_t length;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;

   errcode = psoInit( "10701", "Replace" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_fast_map_replace",
                              strlen("/api_fast_map_replace") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_fastmap_replace",
                              strlen("api_fastmap_replace"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoDataDefCreate( sessionHandle,
                               "api_fastmap_replace",
                               strlen("api_fastmap_replace"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_replace/test",
                           strlen("/api_fast_map_replace/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_replace/test",
                             strlen("/api_fast_map_replace/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle,
                               key,
                               6,
                               data1,
                               strlen(data1),
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_replace/test",
                             strlen("/api_fast_map_replace/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapOpen( sessionHandle2,
                             "/api_fast_map_replace/test",
                             strlen("/api_fast_map_replace/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFastMapReplace( NULL,
                                key,
                                6,
                                data2,
                                strlen(data2),
                                NULL );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapReplace( objHandle,
                                NULL,
                                6,
                                data2,
                                strlen(data2),
                                NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapReplace( objHandle,
                                key,
                                0,
                                data2,
                                strlen(data2),
                                NULL );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapReplace( objHandle,
                                key,
                                6,
                                NULL,
                                strlen(data2),
                                NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                0,
                                NULL );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                strlen(data2),
                                NULL );
   assert_true( errcode == PSO_OK );

   /*
    * Additional stuff to check while the Replace() is uncommitted:
    *  - first session get new value.
    *  - second session get old value
    *  - cannot modify it from any session.
    */
   errcode = psoFastMapGet( objHandle,
                            key,
                            6,
                            buffer,
                            20,
                            &length );
   assert_true( errcode == PSO_OK );
   if ( length != strlen(data2) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( buffer, data2, length ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            20,
                            &length );
   assert_true( errcode == PSO_OK );
   if ( length != strlen(data1) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( buffer, data1, length ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            20,
                            &length );
   assert_true( errcode == PSO_OK );
   if ( length != strlen(data1) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( buffer, data1, length ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCommit( sessionHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapGet( objHandle2,
                            key,
                            6,
                            buffer,
                            20,
                            &length );
   assert_true( errcode == PSO_OK );
   if ( length != strlen(data2) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( buffer, data2, length ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_replace/test",
                             strlen("/api_fast_map_replace/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapReplace( objHandle,
                                key,
                                6,
                                data2,
                                strlen(data2),
                                NULL );
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


