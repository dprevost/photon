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
#include "Engine/HashMap.h"

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
   vdseObjectDescriptor * pDescriptor;
   vdseHashMap * pHashMap;
   char * key1  = "My key1";
   char * key2  = "My key2";
   char * key3  = "My key3";
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   vdseHashItem * pHashItem;

   pFolder = initFolderTest( expectedToPass, &context );
   pTx = context.pTransaction;
   
   vdseTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   errcode = vdseFolderInit( pFolder, 0, 1, 0, &status, 5, strCheck("Test1"), 
                             1234, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseFolderInsertObject( pFolder,
                                     strCheckLow("test2"),
                                     strCheck("Test2"),
                                     5,
                                     VDS_HASH_MAP,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseFolderGetObject( pFolder,
                                  strCheck("test2"),
                                  5,
                                  VDS_HASH_MAP,
                                  &item,
                                  &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, vdseObjectDescriptor );
   GET_PTR( pHashMap, pDescriptor->offset, vdseHashMap );

   /* Test 1 */
   errcode = vdseHashMapInsert( pHashMap,
                                key1,
                                strlen(key1),
                                data1,
                                strlen(data1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key2,
                                strlen(key2),
                                data2,
                                strlen(data2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key3,
                                strlen(key3),
                                data3,
                                strlen(data3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( pHashMap->nodeObject.txCounter != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdseTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Test 2 */
   errcode = vdseHashMapInsert( pHashMap,
                                key1,
                                strlen(key1),
                                data1,
                                strlen(data1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key2,
                                strlen(key2),
                                data2,
                                strlen(data2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key3,
                                strlen(key3),
                                data3,
                                strlen(data3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( pHashMap->nodeObject.txCounter != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapDelete( pHashMap,
                                key1,
                                strlen(key1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key2,
                                strlen(key2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key3,
                                strlen(key3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( pHashMap->nodeObject.txCounter != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdseTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapDelete( pHashMap,
                                key1,
                                strlen(key1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key2,
                                strlen(key2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key3,
                                strlen(key3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( pHashMap->nodeObject.txCounter != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Test 3 */
   errcode = vdseHashMapInsert( pHashMap,
                                key1,
                                strlen(key1),
                                data1,
                                strlen(data1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key2,
                                strlen(key2),
                                data2,
                                strlen(data2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key3,
                                strlen(key3),
                                data3,
                                strlen(data3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseHashMapGet( pHashMap,
                             key2,
                             strlen(key2),
                             &pHashItem,
                             (size_t) -1,
                             &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( pHashMap->nodeObject.txCounter != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdseTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapRelease( pHashMap,
                                 pHashItem,
                                 &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Test 4 */
   errcode = vdseHashMapInsert( pHashMap,
                                key1,
                                strlen(key1),
                                data1,
                                strlen(data1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key2,
                                strlen(key2),
                                data2,
                                strlen(data2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                key3,
                                strlen(key3),
                                data3,
                                strlen(data3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseHashMapGet( pHashMap,
                             key2,
                             strlen(key2),
                             &pHashItem,
                             (size_t) -1,
                             &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapRelease( pHashMap,
                                 pHashItem,
                                 &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapGet( pHashMap,
                             key2,
                             strlen(key2),
                             &pHashItem,
                             (size_t) -1,
                             &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseHashMapDelete( pHashMap,
                                key1,
                                strlen(key1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key2,
                                strlen(key2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key3,
                                strlen(key3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapRelease( pHashMap,
                                 pHashItem,
                                 &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapGet( pHashMap,
                             key2,
                             strlen(key2),
                             &pHashItem,
                             (size_t) -1,
                             &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseHashMapDelete( pHashMap,
                                key1,
                                strlen(key1),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key2,
                                strlen(key2),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapDelete( pHashMap,
                                key3,
                                strlen(key3),
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdseHashMapRelease( pHashMap,
                                 pHashItem,
                                 &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pHashMap->nodeObject.txCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->hashObj.numberOfItems != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

