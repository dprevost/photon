/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   VDS_HANDLE handle, sessionHandle;
   int errcode;
   vdsInfo info, info2;
   size_t allocSpace;
   int xyz = 12345;
   
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

   errcode = vdsGetInfo( NULL, &info );
   if ( errcode != VDS_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsGetInfo( sessionHandle, NULL );
   if ( errcode != VDS_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   handle = (VDS_HANDLE) &xyz;
   errcode = vdsGetInfo( handle, &info );
   if ( errcode != VDS_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsGetInfo( sessionHandle, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   allocSpace = info.allocatedSizeInBytes;
   
   errcode = vdsCreateObject( sessionHandle,
                              "/asgi",
                              strlen("/asgi"),
                              VDS_FOLDER );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsGetInfo( sessionHandle, &info2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes >= info2.allocatedSizeInBytes ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numObjects+1 != info2.numObjects ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numGroups+1 != info2.numGroups ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numMallocs+1 != info2.numMallocs ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numFrees != info2.numFrees ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdsCommit( sessionHandle );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdsDestroyObject( sessionHandle,
                               "/asgi",
                               strlen("/asgi") );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdsGetInfo( sessionHandle, &info );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.allocatedSizeInBytes < info2.allocatedSizeInBytes ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numObjects != info2.numObjects ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numGroups != info2.numGroups ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numMallocs != info2.numMallocs ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numFrees != info2.numFrees ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdsCommit( sessionHandle );
   errcode = vdsGetInfo( sessionHandle, &info2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info2.allocatedSizeInBytes != allocSpace ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numObjects != info2.numObjects+1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numGroups != info2.numGroups+1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numMallocs != info2.numMallocs ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( info.numFrees+1 != info2.numFrees ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdsExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

