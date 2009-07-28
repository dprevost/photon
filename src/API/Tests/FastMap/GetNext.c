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
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   char buffer[200];
   char buffKey[50];
   uint32_t dataLength, keyLength;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;

   errcode = psoInit( "10701", "GetNext" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_fast_map_getnext",
                              strlen("/api_fast_map_getnext") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_fastmap_get_next",
                              strlen("api_fastmap_get_next"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoDataDefCreate( sessionHandle,
                               "api_fastmap_get_next",
                               strlen("api_fastmap_get_next"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_fast_map_getnext/test",
                           strlen("/api_fast_map_getnext/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_fast_map_getnext/test",
                             strlen("/api_fast_map_getnext/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle,
                               key1,
                               7,
                               data1,
                               8,
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle,
                               key2,
                               7,
                               data2,
                               8,
                               NULL );
   assert_true( errcode == PSO_OK );

   /* No GetFirst... */
   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_INVALID_ITERATOR ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFastMapGetNext( NULL,
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

   errcode = psoFastMapGetNext( objHandle,
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

   errcode = psoFastMapGetNext( objHandle,
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

   errcode = psoFastMapGetNext( objHandle,
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

   errcode = psoFastMapGetNext( objHandle,
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

   errcode = psoFastMapGetNext( objHandle,
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

   errcode = psoFastMapGetNext( objHandle,
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

   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   /* Reset the internal iterator first... */
   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   assert_true( errcode == PSO_OK );

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapGetNext( objHandle,
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


