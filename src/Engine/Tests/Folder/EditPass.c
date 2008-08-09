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
   vdseFolder* pFolder;
   vdseSessionContext context;
   bool ok;
   vdseTxStatus status;
   vdseFolderItem folderItem;
   vdseObjectDescriptor * pDescriptor;
   vdseTxStatus * txItemStatus;
   vdseTreeNode * pNode;
   vdsObjectDefinition mapDef = { 
      VDS_FAST_MAP, 
      1, 
      { VDS_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };

   pFolder = initFolderTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = vdseFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &mapDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseFolderEditObject( pFolder,
                              "test2",
                              5,
                              VDS_FAST_MAP,
                              &folderItem,
                              &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, vdseObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, 
                "Test2", 5*sizeof(char) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   GET_PTR( pNode, pDescriptor->nodeOffset, vdseTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, vdseTxStatus );
   if ( txItemStatus->parentCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = vdseFolderEditObject( pFolder,
                              "test3",
                              5,
                              VDS_FAST_MAP,
                              &folderItem,
                              &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( vdscGetLastError( &context.errorHandler ) != VDS_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseFolderInsertObject( pFolder,
                                "test4",
                                "Test4",
                                5,
                                &mapDef,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseFolderEditObject( pFolder,
                              "test4",
                              5,
                              VDS_FAST_MAP,
                              &folderItem,
                              &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, vdseObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, 
                "Test4", 5*sizeof(char) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   GET_PTR( pNode, pDescriptor->nodeOffset, vdseTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, vdseTxStatus );
   if ( txItemStatus->parentCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pFolder->nodeObject.txCounter != 4 ) {
      fprintf( stderr, "txCounter = %d\n", pFolder->nodeObject.txCounter );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseFolderFini( pFolder, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

