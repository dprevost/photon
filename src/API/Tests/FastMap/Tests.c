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
#include "API/FastMap.h"

/*
 * The tests in this file verify that the resources (memory) are properly
 * released when sessions have a map open for read and open for updates.
 *
 */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE objHandleEdit, objHandleRead1, objHandleRead2;
   PSO_HANDLE sessionHandleEdit, sessionHandleRead;
   int errcode;
   const char * key1  = "My Key 1";
   const char * key2  = "My Key 2";
   const char * key3  = "My Key 3";
   const char * data = "My Data";
   psoInfo baseline1, baseline2, baseline3, info;
   psoObjStatus status;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyFieldDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE keyDefHandle, dataDefHandle;

   errcode = psoInit( "10701", "Tests" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandleEdit );
   assert_true( errcode == PSO_OK );
   errcode = psoInitSession( &sessionHandleRead );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &baseline1 );
   assert_true( errcode == PSO_OK );
   
   errcode = psoCreateFolder( sessionHandleEdit,
                              "/api_map_tests",
                              strlen("/api_map_tests") );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &baseline2 );
   assert_true( errcode == PSO_OK );

   errcode = psoKeyDefCreate( sessionHandleEdit,
                              "api_fastmap_tests",
                              strlen("api_fastmap_tests"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   assert_true( errcode == PSO_OK );
   errcode = psoDataDefCreate( sessionHandleEdit,
                               "api_fastmap_tests",
                               strlen("api_fastmap_tests"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCreateMap( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoCommit( sessionHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &baseline3 );
   assert_true( errcode == PSO_OK );

   /*
    * Test 1. Make sure that the temporary object is removed when the
    * edits are committed.
    */
   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_false( info.allocatedSizeInBytes <= baseline3.allocatedSizeInBytes );

   errcode = psoFastMapInsert( objHandleEdit,
                               key1,
                               strlen(key1),
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandleEdit );
   assert_true( errcode == PSO_OK );
   
   errcode = psoCommit( sessionHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   assert_true( errcode == PSO_OK );
   assert_true( status.numDataItem == 1 );
   
   /*
    * Test 2. Make sure that the temporary object is removed when the
    * edits are rollbacked.
    */
   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandleEdit,
                               key2,
                               strlen(key2),
                               data,
                               7,
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandleEdit );
   assert_true( errcode == PSO_OK );
   
   errcode = psoRollback( sessionHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   assert_true( errcode == PSO_OK );
   assert_true( status.numDataItem == 1 );
   
   /*
    * Test 3. Same as test 1 but with the map being used (open in read-only
    * mode).
    */
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   assert_true( errcode == PSO_OK );

    errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead2 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandleEdit,
                               key2,
                               strlen(key2),
                               data,
                               strlen(data),
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandleEdit );
   assert_true( errcode == PSO_OK );
   
   errcode = psoCommit( sessionHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_false( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   assert_true( errcode == PSO_OK );
   assert_true( status.numDataItem == 2 );
   
   errcode = psoFastMapClose( objHandleRead1 );
   assert_true( errcode == PSO_OK );
   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_false( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   errcode = psoCommit( sessionHandleRead );
   assert_true( errcode == PSO_OK );
   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_false( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );

   errcode = psoFastMapClose( objHandleRead2 );
   assert_true( errcode == PSO_OK );
   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   /*
    * Test 4. Same as test 2 but with the map being used (open in read-only
    * mode).
    */
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   assert_true( errcode == PSO_OK );
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead2 );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandleEdit,
                               key3,
                               strlen(key3),
                               data,
                               strlen(data),
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandleEdit );
   assert_true( errcode == PSO_OK );
   
   errcode = psoRollback( sessionHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   assert_true( errcode == PSO_OK );
   assert_true( status.numDataItem == 2 );
   
   errcode = psoFastMapClose( objHandleRead1 );
   assert_true( errcode == PSO_OK );
   
   errcode = psoCommit( sessionHandleRead );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapClose( objHandleRead2 );
   assert_true( errcode == PSO_OK );
   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );
   
   /*
    * Test 5. Attempt to destroy the map while someone using it in update 
    * mode. It should fail as the engine only support one transaction per
    * object when edits are concerned.
    */
   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapInsert( objHandleEdit,
                               key3,
                               strlen(key3),
                               data,
                               strlen(data),
                               NULL );
   assert_true( errcode == PSO_OK );

   errcode = psoDestroyObject( sessionHandleRead,
                               "/api_map_tests/test",
                               strlen("/api_map_tests/test") );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );

   errcode = psoFastMapClose( objHandleEdit );
   assert_true( errcode == PSO_OK );
   
   errcode = psoCommit( sessionHandleEdit );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );

   /*
    * Test 6. Attempt to open a map for update after it was destroyed (but
    * before the commit). It should fail as the engine only support one 
    * transaction per object for edits.
    */
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   assert_true( errcode == PSO_OK );

   errcode = psoDestroyObject( sessionHandleRead,
                               "/api_map_tests/test",
                               strlen("/api_map_tests/test") );
   assert_true( errcode == PSO_OK );

   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );

   errcode = psoCommit( sessionHandleRead );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes );

   errcode = psoFastMapClose( objHandleRead1 );
   assert_true( errcode == PSO_OK );

   errcode = psoGetInfo( sessionHandleEdit, &info );
   assert_true( errcode == PSO_OK );
   assert_true( info.allocatedSizeInBytes == baseline2.allocatedSizeInBytes );

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

