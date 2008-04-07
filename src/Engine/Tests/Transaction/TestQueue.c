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
   vdseTx* pTx;
   vdseFolder * pFolder;
   vdseSessionContext context;
   int errcode;
   vdseFolderItem item;
   vdseTxStatus status;
   vdseObjectDescriptor * pDescriptor;
   vdseQueue * pQueue;
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   vdseQueueItem * pQueueItem;

   pFolder = initFolderTest( expectedToPass, &context );
   pTx = context.pTransaction;
   
   vdseTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   errcode = vdseFolderInit( pFolder, 0, 1, 0, &status, 5, strCheck("Test1"), 
                             1234, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseFolderInsertObject( pFolder,
                                     strCheckLow("test2"),
                                     strCheck("Test2"),
                                     5,
                                     VDS_QUEUE,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseFolderGetObject( pFolder,
                                  strCheck("test2"),
                                  5,
                                  VDS_QUEUE,
                                  &item,
                                  &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, vdseObjectDescriptor );
   GET_PTR( pQueue, pDescriptor->offset, vdseQueue );

   /* Test 1 */
   errcode = vdseQueueInsert( pQueue,
                              data1,
                              strlen(data1),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data2,
                              strlen(data2),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data3,
                              strlen(data3),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 2 */
   errcode = vdseQueueInsert( pQueue,
                              data1,
                              strlen(data1),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data2,
                              strlen(data2),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data3,
                              strlen(data3),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 3 */
   errcode = vdseQueueInsert( pQueue,
                              data1,
                              strlen(data1),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data2,
                              strlen(data2),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data3,
                              strlen(data3),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueGet( pQueue,
                           VDS_FIRST,
                           &pQueueItem,
                           (size_t) -1,
                           &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( pQueue->nodeObject.txCounter != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Test 4 */
   errcode = vdseQueueInsert( pQueue,
                              data1,
                              strlen(data1),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data2,
                              strlen(data2),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueInsert( pQueue,
                              data3,
                              strlen(data3),
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueGet( pQueue,
                           VDS_FIRST,
                           &pQueueItem,
                           (size_t) -1,
                           &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   vdseTxRollback( pTx, &context );
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseQueueRemove( pQueue,
                              &pQueueItem,
                              VDSE_QUEUE_FIRST,
                              (size_t) -1,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTxCommit( pTx, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->listOfElements.currentSize != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRelease( pQueue,
                               pQueueItem,
                               &context );
   if ( errcode != 0 ) {
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

