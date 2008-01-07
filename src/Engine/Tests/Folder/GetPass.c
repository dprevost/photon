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
   int errcode;
   vdseTxStatus status;
   vdseFolderItem folderItem;
   vdseObjectDescriptor * pDescriptor;
   vdseTxStatus * txItemStatus;
   vdseTreeNode * pNode;

   pFolder = initFolderTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseFolderInit( pFolder, 0, 1, 0, &status, 5, strCheck("Test1"), 1234, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseFolderInsertObject( pFolder,
                                     strCheckLow("test2"),
                                     strCheck("Test2"),
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseFolderGetObject( pFolder,
                                  strCheckLow("test2"),
                                  5,
                                  &folderItem,
                                  &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, vdseObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, strCheck("Test2"), 5*sizeof(vdsChar_T) ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   GET_PTR( pNode, pDescriptor->nodeOffset, vdseTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, vdseTxStatus );
   if ( txItemStatus->parentCounter != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.usageCounter != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseFolderGetObject( pFolder,
                                  strCheckLow("test3"),
                                  5,
                                  &folderItem,
                                  &context );
   if ( errcode != -1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( vdscGetLastError( &context.errorHandler ) != VDS_NO_SUCH_OBJECT )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseFolderInsertObject( pFolder,
                                     strCheckLow("test4"),
                                     strCheck("Test4"),
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseFolderGetObject( pFolder,
                                  strCheckLow("test4"),
                                  5,
                                  &folderItem,
                                  &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, vdseObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, strCheck("Test4"), 5*sizeof(vdsChar_T) ) != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   GET_PTR( pNode, pDescriptor->nodeOffset, vdseTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, vdseTxStatus );
   if ( txItemStatus->parentCounter != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.usageCounter != 2 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pFolder->nodeObject.txCounter != 2 )
      ERROR_EXIT( expectedToPass, NULL, ; );
      
   vdseFolderFini( pFolder, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

