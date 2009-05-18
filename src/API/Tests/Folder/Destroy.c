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
   PSO_HANDLE sessionHandle, folderHandle;
   PSO_HANDLE objHandle, sessionHandle2;
   int errcode;
   
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
   errcode = psoInitSession( &sessionHandle2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateFolder( sessionHandle,
                              "/api_folder_destroy",
                              strlen("/api_folder_destroy") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_destroy",
                            strlen("/api_folder_destroy"),
                            &folderHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Destroy non-existing object. */
   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   if ( errcode != PSO_NO_SUCH_OBJECT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateFolder( folderHandle,
                                    "test1",
                                    strlen("test1") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Destroy without a commit - should fail */
   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   if ( errcode != PSO_OBJECT_IS_IN_USE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCommit( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Invalid arguments to tested function. */

   errcode = psoFolderDestroyObject( NULL,
                                     "test1",
                                     strlen("test1") );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderDestroyObject( folderHandle,
                                     NULL,
                                     strlen("test1") );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     0 );
   if ( errcode != PSO_INVALID_LENGTH) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */

   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Open on the same session - should fail */
   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_destroy/test1",
                            strlen("/api_folder_destroy/test1"),
                            &objHandle );
   if ( errcode != PSO_OBJECT_IS_DELETED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Open with a different session - should work */
   errcode = psoFolderOpen( sessionHandle2,
                            "/api_folder_destroy/test1",
                            strlen("/api_folder_destroy/test1"),
                            &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* 
    * Commit with session #2 having the object open. The object should 
    * still be in shared memory but we should be able to create a new one.
    */
   errcode = psoCommit( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   if ( errcode != PSO_NO_SUCH_OBJECT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderCreateFolder( folderHandle,
                                    "test1",
                                    strlen("test1") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFolderOpen( sessionHandle,
                            "/api_folder_destroy/test1",
                            strlen("/api_folder_destroy/test1"),
                            &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Close the process and try to act on the session */

   psoExit();
   
   errcode = psoFolderDestroyObject( folderHandle,
                                     "test1",
                                     strlen("test1") );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

