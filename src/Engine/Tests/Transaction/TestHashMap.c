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
   psnTx* pTx;
   psnFolder * pFolder;
   psnSessionContext context;
   bool ok;
   psnFolderItem item;
   psnTxStatus status;
   psnObjectDescriptor * pDescriptor;
   psnHashMap * pHashMap;
   char * key1  = "My key1";
   char * key2  = "My key2";
   char * key3  = "My key3";
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   psnHashItem * pHashItem;
   vdsObjectDefinition def = { 
      VDS_HASH_MAP, 
      1, 
      { 0, 0, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };

   pFolder = initFolderTest( expectedToPass, &context );
   pTx = context.pTransaction;
   
   psnTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   ok = psnFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 
                             1234, &context );
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
   psnTxCommit( pTx, &context );

   ok = psnFolderGetObject( pFolder,
                             "test2",
                             5,
                             VDS_HASH_MAP,
                             &item,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, psnObjectDescriptor );
   GET_PTR( pHashMap, pDescriptor->offset, psnHashMap );

   /* Test 1 */
   ok = psnHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pHashMap->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 */
   ok = psnHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pHashMap->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pHashMap->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pHashMap->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 */
   ok = psnHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (size_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pHashMap->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 */
   ok = psnHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (size_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (size_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (size_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnHashMapRelease( pHashMap,
                            pHashItem,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

