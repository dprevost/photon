/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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
#include <vdsf/vds.h>
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
   VDS_HANDLE objHandleEdit, objHandleRead1, objHandleRead2;
   VDS_HANDLE sessionHandleEdit, sessionHandleRead;
   int errcode;
   const char * key1  = "My Key 1";
   const char * key2  = "My Key 2";
   const char * key3  = "My Key 3";
   const char * data = "My Data";
   vdsInfo baseline1, baseline2, baseline3, info;
   vdsObjStatus status;
   vdsObjectDefinition mapDef = { 
      VDS_FAST_MAP, 
      1, 
      { VDS_KEY_VAR_STRING, 0, 4, 10 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   vdsObjectDefinition folderDef = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };

   if ( argc > 1 ) {
      errcode = vdsInit( argv[1], 0 );
   }
   else {
      errcode = vdsInit( "10701", 0 );
   }
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsInitSession( &sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsInitSession( &sessionHandleRead );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &baseline1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCreateObject( sessionHandleEdit,
                              "/api_map_tests",
                              strlen("/api_map_tests"),
                              &folderDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &baseline2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandleEdit,
                              "/api_map_tests/test",
                              strlen("/api_map_tests/test"),
                              &mapDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCommit( sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &baseline3 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Test 1. Make sure that the temporary object is removed when the
    * edits are committed.
    */
   errcode = vdsFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes <= baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandleEdit,
                               key1,
                               strlen(key1),
                               data,
                               7 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCommit( sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != VDS_OK ) {
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
   errcode = vdsFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandleEdit,
                               key2,
                               strlen(key2),
                               data,
                               7 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsRollback( sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != VDS_OK ) {
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
   errcode = vdsFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

    errcode = vdsFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandleEdit,
                               key2,
                               strlen(key2),
                               data,
                               strlen(data) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCommit( sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numDataItem != 2 ) {
      fprintf( stderr, "Status error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsFastMapClose( objHandleRead1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCommit( sessionHandleRead );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes == baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleRead2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
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
   errcode = vdsFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

    errcode = vdsFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandleEdit,
                               key3,
                               strlen(key3),
                               data,
                               strlen(data) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsRollback( sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsGetStatus( sessionHandleEdit,
                           "/api_map_tests/test",
                           strlen("/api_map_tests/test"),
                           &status );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numDataItem != 2 ) {
      fprintf( stderr, "Status error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsFastMapClose( objHandleRead1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCommit( sessionHandleRead );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleRead2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
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
   errcode = vdsFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandleEdit,
                               key3,
                               strlen(key3),
                               data,
                               strlen(data) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsDestroyObject( sessionHandleRead,
                               "/api_map_tests/test",
                               strlen("/api_map_tests/test") );
   if ( errcode != VDS_OBJECT_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCommit( sessionHandleEdit );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
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
   errcode = vdsFastMapOpen( sessionHandleRead,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleRead1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsDestroyObject( sessionHandleRead,
                               "/api_map_tests/test",
                               strlen("/api_map_tests/test") );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapEdit( sessionHandleEdit,
                             "/api_map_tests/test",
                             strlen("/api_map_tests/test"),
                             &objHandleEdit );
   if ( errcode != VDS_OBJECT_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCommit( sessionHandleRead );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline3.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapClose( objHandleRead1 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandleEdit, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes != baseline2.allocatedSizeInBytes ) {
      fprintf( stderr, "Baseline error: %d %d\n", info.allocatedSizeInBytes,
         baseline3.allocatedSizeInBytes );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

