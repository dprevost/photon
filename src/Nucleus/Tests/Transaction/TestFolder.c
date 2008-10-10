/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
   psonTx* pTx;
   psonFolder * pFolder;
   psonSessionContext context;
   bool ok;
   psonFolderItem item;
   psonTxStatus status;
   psoObjectDefinition mapDef = { 
      PSO_HASH_MAP, 
      1, 
      { 0, 0, 0, 0}, 
      { { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   psoObjectDefinition folderDef = { 
      PSO_FOLDER, 
      1, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   pFolder = initFolderTest( expectedToPass, &context );
   pTx = context.pTransaction;
   
   psonTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Test 1 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderRelease( pFolder, &item, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderRelease( pFolder, &item, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxRollback( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pFolder->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderRelease( pFolder, &item, &context );
   if ( ok != true ) {
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
