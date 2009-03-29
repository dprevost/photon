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
   PSO_HANDLE objHandle1, sessionHandle1;
   PSO_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key1  = "My Key1";
   const char * key2  = "My Key2";
   const char * data = "My Data";
   uint32_t length;
   char buffer[20];
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psoKeyDefinition keyDef = { "MyKey", PSO_KEY_VARCHAR, 10 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   psoObjectDefinition folderDef = { PSO_FOLDER, 0, 0, 0 };
   PSO_HANDLE keyDefHandle, dataDefHandle;

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
   
   errcode = psoInitSession( &sessionHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoInitSession( &sessionHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle1,
                              "/api_map_empty",
                              strlen("/api_map_empty"),
                              &folderDef,
                              NULL,
                              NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Create and populate the map.
    */
   errcode = psoKeyDefCreate( sessionHandle1,
                              "Definition",
                              strlen("Definition"),
                              PSO_DEF_PHOTON_ODBC_SIMPLE,
                              (unsigned char *)&keyDef,
                              sizeof(psoKeyDefinition),
                              &keyDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoDataDefCreate( sessionHandle1,
                               "Definition",
                               strlen("Definition"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle1,
                              "/api_map_empty/test",
                              strlen("/api_map_empty/test"),
                              &mapDef,
                              keyDefHandle,
                              dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandle1,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandle1,
                               key1,
                               strlen(key1),
                               data,
                               7 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapInsert( objHandle1,
                               key2,
                               strlen(key2),
                               data,
                               7 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoCommit( sessionHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Get both handles and do a sanity check */
   errcode = psoFastMapOpen( sessionHandle1,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoFastMapEmpty( objHandle1 ); /* read-only handle */
   if ( errcode != PSO_OBJECT_IS_READ_ONLY ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEmpty( NULL );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEmpty( sessionHandle1 );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. */
   
   /* 
    * Empty the map: 
    *   - the editor (objHandle2) does not see the keys
    *   - the reader (   "     ) - nothing has changed. 
    */
   errcode = psoFastMapEmpty( objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

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
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Commit session2 but not session 1:
    *   - the new reader (objHandle2) does not see it (it is gone)
    *   - the old reader (   "     ) - nothing has changed. 
    * Furthermore, a new reader on session 1 should also not see it.
    */
   errcode = psoFastMapClose( objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoCommit( sessionHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapOpen( sessionHandle2,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
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
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapClose( objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapOpen( sessionHandle1,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
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
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /*
    * We repeat the process with but this time, we'll 
    * use rollback on session 1 instead of commit. The result should
    * be identical.
    */
   errcode = psoFastMapClose( objHandle2 ); /* not in edit mode and with session 1 */
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapInsert( objHandle2,
                               key1,
                               strlen(key1),
                               data,
                               7 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapInsert( objHandle2,
                               key2,
                               strlen(key2),
                               data,
                               7 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapClose( objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoCommit( sessionHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoCommit( sessionHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoFastMapEdit( sessionHandle2,
                             "/api_map_empty/test",
                             strlen("/api_map_empty/test"),
                             &objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEmpty( objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

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
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapClose( objHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoCommit( sessionHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoRollback( sessionHandle1 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoFastMapGet( objHandle1, 
                            key1, 
                            strlen(key1),
                            buffer, 20, &length );
   if ( errcode != PSO_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

