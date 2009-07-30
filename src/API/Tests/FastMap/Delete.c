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

void setup_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   int errcode;
   PSO_HANDLE sessionHandle;
   
   errcode = psoInitSession( &sessionHandle );
   assert( errcode == PSO_OK );

   errcode = psoaDataDefDestroy( sessionHandle,
                                 "api_fastmap_delete",
                                 strlen("api_fastmap_delete") );
   assert( errcode == PSO_OK );
   errcode = psoaKeyDefDestroy( sessionHandle,
                                "api_fastmap_delete",
                                strlen("api_fastmap_delete") );
   
   assert( errcode == PSO_OK );
   errcode = psoDestroyObject( sessionHandle,
                               "/api_fastmap_delete/test",
                               strlen("/api_fastmap_delete/test") );
   assert( errcode == PSO_OK );

   errcode = psoDestroyObject( sessionHandle,
                               "/api_fastmap_delete",
                               strlen("/api_fastmap_delete") );
   fprintf( stderr, "%d\n", errcode );
   assert( errcode == PSO_OK );
   
   psoCommit( sessionHandle );
   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE objHandle1, sessionHandle1;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key1  = "My Key1";
   const char * key2  = "My Key2";
   const char * data = "My Data";
   uint32_t length;
   char buffer[20];
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;

   errcode = psoInit( "10701", "Delete" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle1 );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle1,
                              "/api_fastmap_delete",
                              strlen("/api_fastmap_delete") );
   assert_true( errcode == PSO_OK );

   /*
    * Create and populate the map.
    */
   errcode = psoKeyDefCreate( sessionHandle1,
                              "api_fastmap_delete",
                              strlen("api_fastmap_delete"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoDataDefCreate( sessionHandle1,
                               "api_fastmap_delete",
                               strlen("api_fastmap_delete"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle1,
                           "/api_fastmap_delete/test",
                           strlen("/api_fastmap_delete/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle1,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle1 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle1,
                               key1,
                               strlen(key1),
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapInsert( objHandle1,
                               key2,
                               strlen(key2),
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandle1 );
   assert_true( errcode == PSO_OK );
   
   errcode = psoCommit( sessionHandle1 );
   assert_true( errcode == PSO_OK );
   
   /* Get both handles and do a sanity check */
   errcode = psoFastMapOpen( sessionHandle1,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle1 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoFastMapDelete( objHandle1, /* read-only handle */
                               key1,
                               strlen(key1) );
   if ( errcode != PSO_OBJECT_IS_READ_ONLY ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapDelete( NULL,
                               key1,
                               strlen(key1) );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapDelete( sessionHandle1,
                               key1,
                               strlen(key1) );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapDelete( objHandle2,
                               NULL,
                               strlen(key1) );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapDelete( objHandle2,
                               key1,
                               0 );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. */
   
   /* 
    * Delete the key: 
    *   - the editor (objHandle2) does not see it (it is gone)
    *   - the reader (   "     ) - nothing has changed. 
    */
   errcode = psoFastMapDelete( objHandle2,
                               key1,
                               strlen(key1) );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapGet( objHandle2, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   assert_true( errcode == PSO_OK );

   /*
    * Commit session2 but not session 1:
    *   - the new reader (objHandle2) does not see it (it is gone)
    *   - the old reader (   "     ) - nothing has changed. 
    * Furthermore, a new reader on session 1 should also not see it.
    */
   errcode = psoFastMapClose( objHandle1 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapClose( objHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle2 );
   assert_true( errcode == PSO_OK );
#if 0
   errcode = psoFastMapOpen( sessionHandle2,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapGet( objHandle2, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapOpen( sessionHandle1,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapGet( objHandle2, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   /*
    * Commit session1 - the old reader should not see it now.
    */
   errcode = psoCommit( sessionHandle1 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * We repeat the process with the second key but this time, we'll 
    * use rollback on session 1 instead of commit. The result should
    * be identical.
    */
   errcode = psoFastMapClose( objHandle2 ); /* not in edit mode and with session 1 */
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapDelete( objHandle2,
                               key2,
                               strlen(key2) );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapGet( objHandle2, 
                            key2, 
                            strlen(key2),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapGet( objHandle1, 
                            key2, 
                            strlen(key2),
                            buffer, 20, &length );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapClose( objHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoCommit( sessionHandle2 );
   assert_true( errcode == PSO_OK );
   errcode = psoRollback( sessionHandle1 );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapGet( objHandle1, 
                            key2, 
                            strlen(key2),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Close the session and try to act on the object */
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_fastmap_delete/test",
                             strlen("/api_fastmap_delete/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );
#endif

   errcode = psoExitSession( sessionHandle2 );
   assert_true( errcode == PSO_OK );

#if 0
   errcode = psoFastMapDelete( objHandle2,
                               key1,
                               strlen(key1) );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
#endif

   errcode = psoExitSession( sessionHandle1 );
   assert_true( errcode == PSO_OK );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_pass, setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


