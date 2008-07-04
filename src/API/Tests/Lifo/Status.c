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
#include "API/Lifo.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE sessionHandle, objHandle;
   int errcode;
   const char * data1 = "My Data1";
   const char * data2 = "My Data2";
   const char * data3 = "My Data3";
   vdsObjStatus status;
   vdsObjectDefinition defLilo = { 
      VDS_LIFO,
      1, 
      { 0, 0, 0, 0}, 
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

   errcode = vdsCreateObject( sessionHandle,
                              "/api_lifo_sp",
                              strlen("/api_lifo_sp"),
                              &folderDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/api_lifo_sp/test",
                              strlen("/api_lifo_sp/test"),
                              &defLilo );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoOpen( sessionHandle,
                           "/api_lifo_sp/test",
                           strlen("/api_lifo_sp/test"),
                           &objHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoPush( objHandle, data1, strlen(data1) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsLifoPush( objHandle, data2, strlen(data2) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsLifoPush( objHandle, data3, strlen(data3) );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = vdsLifoStatus( NULL, &status );
   if ( errcode != VDS_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoStatus( objHandle, NULL );
   if ( errcode != VDS_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = vdsLifoStatus( objHandle, &status );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   if ( status.numDataItem != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numBlocks != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numBlockGroup != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.freeBytes == 0 || status.freeBytes >=VDSE_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Close the session and try to act on the object */

   errcode = vdsExitSession( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoStatus( objHandle, &status );
   if ( errcode != VDS_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

