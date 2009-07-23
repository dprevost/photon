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
#include "Nucleus/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonTx* pTx;
   psonFolder * pFolder;
   psonSessionContext context;
   bool ok;
   psonFolderItem item;
   psonTxStatus status;
   psonObjectDescriptor * pDescriptor;
   psonQueue * pQueue;
   char * data1 = "My data1";
   char * data2 = "My data2";
   char * data3 = "My data3";
   psonQueueItem * pQueueItem;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psonDataDefinition fields;

   pFolder = initFolderTest( &context );
   pTx = context.pTransaction;
   
   psonTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 
                             1234, &context );
   assert_true( ok );
   
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                &fields,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonTxCommit( pTx, &context );
   assert_true( ok );
   ok = psonFolderGetObject( pFolder,
                             "test2",
                             5,
                             PSO_QUEUE,
                             &item,
                             &context );
   assert_true( ok );
   GET_PTR( pDescriptor, item.pHashItem->dataOffset, psonObjectDescriptor );
   GET_PTR( pQueue, pDescriptor->offset, psonQueue );

   /* Test 1 */
   ok = psonQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   
   assert_true( pQueue->nodeObject.txCounter == 3 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   psonTxRollback( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 0 );
   
   /* Test 2 */
   ok = psonQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   
   assert_true( pQueue->nodeObject.txCounter == 3 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   ok = psonTxCommit( pTx, &context );
   assert_true( ok );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   
   assert_true( pQueue->nodeObject.txCounter == 3 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   psonTxRollback( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   
   assert_true( pQueue->nodeObject.txCounter == 3 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   psonTxCommit( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 0 );
   
   /* Test 3 */
   ok = psonQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   
   ok = psonQueueGetFirst( pQueue,
                           &pQueueItem,
                           (uint32_t) -1,
                           &context );
   assert_true( ok );
   
   assert_true( pQueue->nodeObject.txCounter == 3 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   psonTxRollback( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 1 );
   assert_true( pQueue->listOfElements.currentSize == 1 );
   
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 0 );
   
   /* Test 4 */
   ok = psonQueueInsert( pQueue,
                         data1,
                         strlen(data1),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data2,
                         strlen(data2),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   ok = psonQueueInsert( pQueue,
                         data3,
                         strlen(data3),
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert_true( ok );
   
   ok = psonQueueGetFirst( pQueue,
                           &pQueueItem,
                           (uint32_t) -1,
                           &context );
   assert_true( ok );
   
   psonTxCommit( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 3 );

   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 3 );

   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   
   psonTxRollback( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 3 );
   
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &context );
   assert_true( ok );
   
   psonTxCommit( pTx, &context );
   assert_true( pQueue->nodeObject.txCounter == 1 );
   assert_true( pQueue->listOfElements.currentSize == 1 );
   
   ok = psonQueueRelease( pQueue,
                          pQueueItem,
                          &context );
   assert_true( ok );
   assert_true( pQueue->nodeObject.txCounter == 0 );
   assert_true( pQueue->listOfElements.currentSize == 0 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


