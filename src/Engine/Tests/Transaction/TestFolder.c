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

#include "txTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseTx* pTx;
   vdseFolder * pFolder;
   vdseSessionContext context;
   int errcode;
   vdseFolderItem item;
   vdseTxStatus status;
   vdsObjectDefinition mapDef = { 
      VDS_HASH_MAP, 
      1, 
      { "", 0, 0, 0, 0, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   vdsObjectDefinition folderDef = { 
      VDS_FOLDER, 
      1, 
      { "", 0, 0, 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   pFolder = initFolderTest( expectedToPass, &context );
   pTx = context.pTransaction;
   
   vdseTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   errcode = vdseFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Test 1 */
   errcode = vdseFolderInsertObject( pFolder,
                                     "test2",
                                     "Test2",
                                     5,
                                     &folderDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderInsertObject( pFolder,
                                     "test3",
                                     "Test3",
                                     5,
                                     &mapDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 */
   errcode = vdseFolderInsertObject( pFolder,
                                     "test2",
                                     "Test2",
                                     5,
                                     &folderDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderInsertObject( pFolder,
                                     "test3",
                                     "Test3",
                                     5,
                                     &mapDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test2",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test3",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test2",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test3",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 */
   errcode = vdseFolderInsertObject( pFolder,
                                     "test2",
                                     "Test2",
                                     5,
                                     &folderDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderInsertObject( pFolder,
                                     "test3",
                                     "Test3",
                                     5,
                                     &mapDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderGetObject( pFolder,
                                  "test3",
                                  5,
                                  VDS_HASH_MAP,
                                  &item,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderRelease( pFolder, &item, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 */
   errcode = vdseFolderInsertObject( pFolder,
                                     "test2",
                                     "Test2",
                                     5,
                                     &folderDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderInsertObject( pFolder,
                                     "test3",
                                     "Test3",
                                     5,
                                     &mapDef,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderGetObject( pFolder,
                                  "test3",
                                  5,
                                  VDS_HASH_MAP,
                                  &item,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   vdseTxCommit( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderRelease( pFolder, &item, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderGetObject( pFolder,
                                  "test3",
                                  5,
                                  VDS_HASH_MAP,
                                  &item,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test2",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test3",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test2",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderDeleteObject( pFolder,
                                     "test3",
                                     5,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseFolderRelease( pFolder, &item, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
