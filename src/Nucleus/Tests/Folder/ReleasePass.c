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
   psnFolder * pFolder;
   psnSessionContext context;
   bool ok;
   psnFolderItem item;
   psnTxStatus status;
   psnObjectDescriptor * pDescriptor;
   psnTxStatus * txItemStatus;
   psnTreeNode * pNode;
   vdsObjectDefinition def = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   pFolder = initFolderTest( expectedToPass, &context );

   psnTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psnFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnFolderGetFirst( pFolder,
                            &item,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnFolderGetNext( pFolder,
                           &item,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, psnObjectDescriptor );
   GET_PTR( pNode, pDescriptor->nodeOffset, psnTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, psnTxStatus );
   if ( txItemStatus->parentCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnFolderRelease( pFolder, &item, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( txItemStatus->parentCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
