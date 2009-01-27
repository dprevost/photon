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

#include "folderTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonFolder* pFolder;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   psonFolderItem folderItem;
   psonObjectDescriptor * pDescriptor;
   psonTxStatus * txItemStatus;
   psonTreeNode * pNode;
   psoBasicObjectDef def = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}
   };

   pFolder = initFolderTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                NULL,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderGetObject( pFolder,
                             "test2",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psonObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, 
                "Test2", 5*sizeof(char) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   GET_PTR( pNode, pDescriptor->nodeOffset, psonTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, psonTxStatus );
   if ( txItemStatus->parentCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( psocGetLastError( &context.errorHandler ) != PSO_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
      
   ok = psonFolderInsertObject( pFolder,
                                "test4",
                                "Test4",
                                5,
                                &def,
                                NULL,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderGetObject( pFolder,
                             "test4",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psonObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, 
                "Test4", 5*sizeof(char) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   GET_PTR( pNode, pDescriptor->nodeOffset, psonTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, psonTxStatus );
   if ( txItemStatus->parentCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonFolderFini( pFolder, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

