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

#include "folderTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseFolder * pFolder;
   vdseSessionContext context;
   int errcode;
   vdsObjectDefinition def = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   pFolder = initTopFolderTest( expectedToPass, &context );

   errcode = vdseFolderCreateObject( pFolder,
                                     "Test1",
                                     strlen("Test1"),
                                     &def,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Using the topfolder to create a grandchild! */
   errcode = vdseTopFolderCreateObject( pFolder,
                                        "Test1/Test2",
                                        strlen("Test1/Test2"),
                                        &def,
                                        &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTxCommit( (vdseTx *)context.pTransaction, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDestroyObject( pFolder,
                                      "Test1",
                                      strlen("Test1"),
                                      &context );
                                         
   if ( vdscGetLastError(&context.errorHandler) != VDS_FOLDER_IS_NOT_EMPTY ) {
      if ( errcode != 0 ) {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      else {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }

   errcode = vdseTopFolderDestroyObject( pFolder,
                                         "Test1/Test2",
                                         strlen("Test1/Test2"),
                                         &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDestroyObject( pFolder,
                                      "Test2",
                                      strlen("Test2"),
                                      &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = vdseFolderDestroyObject( pFolder,
                                      "Test1",
                                      VDS_MAX_NAME_LENGTH+1,
                                      &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = vdseFolderDestroyObject( pFolder,
                                      "Test1",
                                      0,
                                      &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = vdseFolderDestroyObject( pFolder,
                                      "Test1",
                                      strlen("Test1"),
                                      &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Calling destroy on the same object, twice */
   errcode = vdseFolderDestroyObject( pFolder,
                                      "Test1",
                                      strlen("Test1"),
                                      &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_OBJECT_IS_IN_USE ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
