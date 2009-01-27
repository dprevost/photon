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

#include "txTest.h"
#include "Nucleus/HashMap.h"

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
   psonObjectDescriptor * pDescriptor;
   psonHashMap * pHashMap;
   char * key1  = "My key1";
   char * key2  = "My key2";
   char * key3  = "My key3";
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   psonHashTxItem * pHashItem;
   psoBasicObjectDef def = { 
      PSO_HASH_MAP, 1, { 0, 0, 0, 0} 
   };

   psoFieldDefinition fields[1] =  { 
      { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 } 
   };

   pFolder = initFolderTest( expectedToPass, &context );
   pTx = context.pTransaction;
   
   psonTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 
                             1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                fields,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   psonTxCommit( pTx, &context );

   ok = psonFolderGetObject( pFolder,
                             "test2",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, psonObjectDescriptor );
   GET_PTR( pHashMap, pDescriptor->offset, psonHashMap );

   /* Test 1 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
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
   
   psonTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
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
   
   psonTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
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
   
   psonTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
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
   
   psonTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 */
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
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
   
   psonTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonHashMapRelease( pHashMap,
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
   ok = psonHashMapInsert( pHashMap,
                           key1,
                           strlen(key1),
                           data1,
                           strlen(data1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
                           key2,
                           strlen(key2),
                           data2,
                           strlen(data2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapInsert( pHashMap,
                           key3,
                           strlen(key3),
                           data3,
                           strlen(data3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonHashMapRelease( pHashMap,
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
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonTxRollback( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonHashMapRelease( pHashMap,
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
   
   ok = psonHashMapGet( pHashMap,
                        key2,
                        strlen(key2),
                        &pHashItem,
                        (uint32_t) -1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonHashMapDelete( pHashMap,
                           key1,
                           strlen(key1),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key2,
                           strlen(key2),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonHashMapDelete( pHashMap,
                           key3,
                           strlen(key3),
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonTxCommit( pTx, &context );
   if ( pHashMap->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pHashMap->hashObj.numberOfItems != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonHashMapRelease( pHashMap,
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

