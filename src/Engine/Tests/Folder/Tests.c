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
   vdseFolder* pFolder1, *pFolder2;

   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   vdseObjectDescriptor* pDescriptor = NULL;
   vdseFolderItem folderItem;
   
   /* Create "/" */
   pFolder1 = initFolderTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseFolderInit( pFolder1, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Create "/Test2" */   
   errcode = vdseFolderInsertObject( pFolder1,
                                     "test2",
                                     "Test2",
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Try to create "/Test2" again - must fail */   
   errcode = vdseFolderInsertObject( pFolder1,
                                     "test2",
                                     "Test5",
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( vdscGetLastError( &context.errorHandler ) != VDS_OBJECT_ALREADY_PRESENT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Create "/Test3" */   
   errcode = vdseFolderInsertObject( pFolder1,
                                     "test3",
                                     "Test3",
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Get "/Test2" */   
   errcode = vdseFolderGetObject( pFolder1,
                                  "test2",
                                  5,
                                  VDS_FOLDER,
                                  &folderItem,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );   
   }
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, vdseObjectDescriptor );
   GET_PTR( pFolder2, pDescriptor->offset, vdseFolder );

   /* Create "/Test2/Test4" from "/Test2" */   
   errcode = vdseFolderInsertObject( pFolder2,
                                     "test4",
                                     "Test4",
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Create "/Test2/Test2" */   
   errcode = vdseFolderInsertObject( pFolder2,
                                     "test2",
                                     "Test2",
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTxCommit( (vdseTx *)context.pTransaction, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Try to delete "/Test2" - should fail (not empty) */
   errcode = vdseFolderDeleteObject( pFolder1,
                                     "test2",
                                     5,
                                     &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( vdscGetLastError( &context.errorHandler ) != VDS_FOLDER_IS_NOT_EMPTY ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Try to delete "/Test55" - should fail (no such object) */
   errcode = vdseFolderDeleteObject( pFolder1,
                                     "test55",
                                     6,
                                     &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( vdscGetLastError( &context.errorHandler ) != VDS_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Get "/Test2/Test4" from "/" */   
   errcode = vdseFolderGetObject( pFolder1,
                                  "test2/test4",
                                  11,
                                  VDS_FOLDER,
                                  &folderItem,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );   
   }
   
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, vdseObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, 
                "Test4", 
                5*sizeof(vdsChar_T) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Create "/Test2/Test4/Test5 from "/" */
   
   errcode = vdseFolderInsertObject( pFolder1,
                                     "test2/test4/test5",
                                     "Test2/Test4/Test5",
                                     17,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTxCommit( (vdseTx *)context.pTransaction, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Delete "/Test2/Test4/Test6" - must fail (no such object) */
   errcode = vdseFolderDeleteObject( pFolder1,
                                     "test2/test4/test6",
                                     17,
                                     &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( vdscGetLastError( &context.errorHandler ) != VDS_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Delete "/Test2/Test5/Test5" - must fail (no such folder) */
   errcode = vdseFolderDeleteObject( pFolder1,
                                     "test2/test5/test5",
                                     17,
                                     &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( vdscGetLastError( &context.errorHandler ) != VDS_NO_SUCH_FOLDER ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Delete "/Test2/Test4/Test5" */
   errcode = vdseFolderDeleteObject( pFolder1,
                                     "test2/test4/test5",
                                     17,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   vdseTxCommit( context.pTransaction, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
