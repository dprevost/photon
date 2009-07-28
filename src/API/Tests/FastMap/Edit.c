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
   PSO_HANDLE objHandle,  sessionHandle;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   char junk[12];
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;
   const char * data1 = "My Data1";
   
   memset( junk, 0, 12 );
   
   errcode = psoInit( "10701", "Edit" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandle2 );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_map_edit",
                              strlen("/api_map_edit") );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_fastmap_edit",
                              strlen("api_fastmap_edit"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoDataDefCreate( sessionHandle,
                               "api_fastmap_edit",
                               strlen("api_fastmap_edit"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandle,
                           "/api_map_edit/test",
                           strlen("/api_map_edit/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandle );
   assert_true( errcode == PSO_OK );
   
   /* Invalid arguments to tested function. */

   errcode = psoFastMapEdit( NULL,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             &objHandle );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   objHandle = (PSO_HANDLE) junk;
   errcode = psoFastMapEdit( objHandle,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             &objHandle );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandle,
                             NULL,
                             strlen("/api_map_edit/test"),
                             &objHandle );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_map_edit/test",
                             0,
                             &objHandle );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             NULL );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapEdit( sessionHandle,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   /* A second edit session ? */
   errcode = psoFastMapEdit( sessionHandle,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             &objHandle2 );
   if ( errcode != PSO_A_SINGLE_UPDATER_IS_ALLOWED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             &objHandle2 );
   if ( errcode != PSO_A_SINGLE_UPDATER_IS_ALLOWED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandle, 
                               data1, 
                               strlen(data1), 
                               data1,
                               strlen(data1),
                               dataDefHandle );
   if ( errcode != PSO_DATA_DEF_UNSUPPORTED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapOpen( sessionHandle,
                             "/api_map_edit/test",
                             strlen("/api_map_edit/test"),
                             &objHandle2 );
   assert_true( errcode == PSO_OK );

   psoFastMapClose( objHandle );
   psoRollback( sessionHandle );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_map_edit2",
                              strlen("/api_map_edit2") );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_fastmap_edit2",
                               strlen("api_fastmap_edit2"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );
   
   mapDef.flags = PSO_MULTIPLE_DATA_DEFINITIONS;
   errcode = psoCreateMap( sessionHandle,
                           "/api_map_edit2/test",
                           strlen("/api_map_edit2/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandle,
                             "/api_map_edit2/test",
                             strlen("/api_map_edit2/test"),
                             &objHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandle, 
                               data1, 
                               strlen(data1), 
                               data1,
                               strlen(data1),
                               dataDefHandle );
   assert_true( errcode == PSO_OK );

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


