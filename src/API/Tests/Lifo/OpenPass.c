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
#include "API/Lifo.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE objHandle,  sessionHandle;
   VDS_HANDLE objHandle2, sessionHandle2;
   int errcode;
   char junk[12];
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
   
   memset( junk, 0, 12 );

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
                              "/api_lifo_op",
                              strlen("/api_lifo_op"),
                              &folderDef );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsCreateObject( sessionHandle,
                              "/api_lifo_op/test",
                              strlen("/api_lifo_op/test"),
                              &defLilo );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = vdsLifoOpen( NULL,
                           "/api_lifo_op/test",
                           strlen("/api_lifo_op/test"),
                           &objHandle );
   if ( errcode != VDS_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   objHandle = (VDS_HANDLE) junk;
   errcode = vdsLifoOpen( objHandle,
                           "/api_lifo_op/test",
                           strlen("/api_lifo_op/test"),
                           &objHandle );
   if ( errcode != VDS_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoOpen( sessionHandle,
                           NULL,
                           strlen("/api_lifo_op/test"),
                           &objHandle );
   if ( errcode != VDS_INVALID_OBJECT_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoOpen( sessionHandle,
                           "/api_lifo_op/test",
                           0,
                           &objHandle );
   if ( errcode != VDS_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoOpen( sessionHandle,
                             "/api_lifo_op/test",
                             strlen("/api_lifo_op/test"),
                             NULL );
   if ( errcode != VDS_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = vdsLifoOpen( sessionHandle,
                            "/api_lifo_op/test",
                            strlen("/api_lifo_op/test"),
                            &objHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsLifoOpen( sessionHandle2,
                            "/api_lifo_op/test",
                            strlen("/api_lifo_op/test"),
                            &objHandle2 );
   if ( errcode != VDS_OBJECT_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   vdsExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

