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
#include <photon/vds.h>
#include "Tests/PrintError.h"
#include "API/Map.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE objHandle,  sessionHandle;
   VDS_HANDLE objHandle2, sessionHandle2;
   int errcode;
   const char * key  = "My Key";
   const char * data = "My Data";
   char buffer[20];
   size_t length;
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
   
   errcode = vdsInitSession( &sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsInitSession( &sessionHandle2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/amip",
                              strlen("/amip"),
                              &folderDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/amip/test",
                              strlen("/amip/test"),
                              &mapDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsFastMapEdit( sessionHandle,
                             "/amip/test",
                             strlen("/amip/test"),
                             &objHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsFastMapOpen( sessionHandle2,
                             "/amip/test",
                             strlen("/amip/test"),
                             &objHandle2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = vdsFastMapInsert( NULL,
                               key,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( sessionHandle,
                               key,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandle,
                               NULL,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandle,
                               key,
                               0,
                               data,
                               7 );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandle,
                               key,
                               6,
                               NULL,
                               7 );
   if ( errcode != VDS_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               0 );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* 1 under the data minimum length */
   errcode = vdsFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               3 );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* 1 over the data maximum length */
   errcode = vdsFastMapInsert( objHandle,
                               key,
                               6,
                               "12345678901", 
                               11 );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   // 1 under the key minimum length
   errcode = vdsFastMapInsert( objHandle,
                               key,
                               3,
                               data,
                               7 );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* 1 over the key maximum length */
   errcode = vdsFastMapInsert( objHandle,
                               "12345678901",
                               11,
                               data, 
                               7 );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* read-only handle */
   errcode = vdsFastMapInsert( objHandle2,
                               key,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_OBJECT_IS_READ_ONLY ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   /* End of invalid args. This call should succeed. */
   errcode = vdsFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    *  - cannot get access to the item from second session.
    *  - can get access to the item from first session.
    */
   errcode = vdsFastMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsFastMapGet( objHandle, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsFastMapClose( objHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsFastMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_NO_SUCH_ITEM ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsRollback( sessionHandle2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsFastMapGet( objHandle2, 
                            key, 
                            6,
                            buffer, 20, &length );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = vdsFastMapEdit( sessionHandle,
                             "/amip/test",
                             strlen("/amip/test"),
                             &objHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsExitSession( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsFastMapInsert( objHandle,
                               key,
                               6,
                               data,
                               7 );
   if ( errcode != VDS_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

