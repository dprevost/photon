/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include <photon/photon.h>
#include "Tests/PrintError.h"
#include "API/Map.h"

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
   psoObjectDefinition mapDef = { 
      PSO_FAST_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 4, 10 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   psoObjectDefinition folderDef = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };

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
   
   errcode = psoCreateObject( sessionHandleEdit,
                              "/api_map_tests",
                              strlen("/api_map_tests"),
                              &folderDef );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandleEdit, &baseline2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandleEdit,
                              "/api_map_tests/test",
                              strlen("/api_map_tests/test"),
                              &mapDef );
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandleEdit,
                               key1,
                               strlen(key1),
                               data,
                               7 );
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Status error: %d %d\n", info.allocatedSizeInBytes,
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
                               7 );
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Status error: %d %d\n", info.allocatedSizeInBytes,
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
                               strlen(data) );
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Status error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
                               strlen(data) );
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Status error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
                               strlen(data) );
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
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
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

