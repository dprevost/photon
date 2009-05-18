/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE handle, sessionHandle;
   int errcode;
   psoInfo info, info2;
   size_t allocSpace;
   int xyz = 12345;
   
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
   
   errcode = psoInitSession( &sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoGetInfo( NULL, &info );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoGetInfo( sessionHandle, NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   handle = (PSO_HANDLE) &xyz;
   errcode = psoGetInfo( handle, &info );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* End of invalid args. This call should succeed. */
   errcode = psoGetInfo( sessionHandle, &info );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   allocSpace = info.allocatedSizeInBytes;
   
   errcode = psoCreateFolder( sessionHandle,
                              "/api_session_get_info",
                              strlen("/api_session_get_info") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoGetInfo( sessionHandle, &info2 );
   if ( errcode != PSO_OK ) {
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
   
   errcode = psoCommit( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoDestroyObject( sessionHandle,
                               "/api_session_get_info",
                               strlen("/api_session_get_info") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psoGetInfo( sessionHandle, &info );
   if ( errcode != PSO_OK ) {
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
   
   psoCommit( sessionHandle );
   errcode = psoGetInfo( sessionHandle, &info2 );
   if ( errcode != PSO_OK ) {
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
   
   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoGetInfo( sessionHandle, &info );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

