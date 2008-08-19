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
#include "Engine/Queue.h"

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
   psnQueue * pQueue;
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   psnQueueItem * pQueueItem;
   vdsObjectDefinition def = { 
      VDS_QUEUE, 
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
                             VDS_QUEUE,
                             &item,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, psnObjectDescriptor );
   GET_PTR( pQueue, pDescriptor->offset, psnQueue );

   /* Test 1 */
   ok = psnQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 */
   ok = psnQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 */
   ok = psnQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnQueueGet( pQueue,
                      VDS_FIRST,
                      &pQueueItem,
                      (size_t) -1,
                      &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 */
   ok = psnQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnQueueGet( pQueue,
                      VDS_FIRST,
                      &pQueueItem,
                      (size_t) -1,
                      &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnQueueRemove( pQueue,
                         &pQueueItem,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psnQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

