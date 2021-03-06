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

/*
 * The tests in this file verify that the resources (memory) are properly
 * released when sessions have a map open for read and open for updates.
 *
 */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
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
   
   errcode = psoInitSession( &sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoInitSession( &sessionHandleRead );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &baseline1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCreateFolder( sessionHandleEdit,
                              "/api_map_tests",
                              strlen("/api_map_tests") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &baseline2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandleEdit,
                              "api_fastmap_tests",
                              strlen("api_fastmap_tests"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyFieldDefinition),
                              &keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoDataDefCreate( sessionHandleEdit,
                               "api_fastmap_tests",
                               strlen("api_fastmap_tests"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateMap( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &mapDef,
                           dataDefHandle,
                           keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCommit( sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &baseline3 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Test 1. Make sure that the temporary object is removed when the
    * edits are committed.
    */
   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes <= baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandleEdit,
                               key1,
                               strlen(key1),
                               data,
                               7,
                               NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCommit( sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n",
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numDataItem != 1 ) {
      fprintf( stderr, "Status error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * Test 2. Make sure that the temporary object is removed when the
    * edits are rollbacked.
    */
   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandleEdit,
                               key2,
                               strlen(key2),
                               data,
                               7,
                               NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoRollback( sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numDataItem != 1 ) {
      fprintf( stderr, "Status error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * Test 3. Same as test 1 but with the map being used (open in read-only
    * mode).
    */
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

    errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandleEdit,
                               key2,
                               strlen(key2),
                               data,
                               strlen(data),
                               NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCommit( sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numDataItem != 2 ) {
      fprintf( stderr, "Status error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n",
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapClose( objHandleRead1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCommit( sessionHandleRead );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleRead2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n",
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * Test 4. Same as test 2 but with the map being used (open in read-only
    * mode).
    */
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

    errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandleEdit,
                               key3,
                               strlen(key3),
                               data,
                               strlen(data),
                               NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoRollback( sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n",
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numDataItem != 2 ) {
      fprintf( stderr, "Status error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapClose( objHandleRead1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCommit( sessionHandleRead );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleRead2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * Test 5. Attempt to destroy the map while someone using it in update 
    * mode. It should fail as the engine only support one transaction per
    * object when edits are concerned.
    */
   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandleEdit,
                               key3,
                               strlen(key3),
                               data,
                               strlen(data),
                               NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoDestroyObject( sessionHandleRead,
                               "/api_map_tests/test",
                               strlen("/api_map_tests/test") );
   if ( errcode != PSO_OBJECT_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCommit( sessionHandleEdit );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Test 6. Attempt to open a map for update after it was destroyed (but
    * before the commit). It should fail as the engine only support one 
    * transaction per object for edits.
    */
   errcode = psoFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoDestroyObject( sessionHandleRead,
                               "/api_map_tests/test",
                               strlen("/api_map_tests/test") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != PSO_OBJECT_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCommit( sessionHandleRead );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandleRead1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline2.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: "PSO_SIZE_T_FORMAT" "PSO_SIZE_T_FORMAT"\n", 
	       info.allocatedSizeInBytes,
               baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

