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

#include "queueTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseQueue * pQueue;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   char * data = "My Data";
   vdseQueueItem * pItem = NULL;
   vdseTxStatus * txItemStatus;
   vdsObjectDefinition def = { 
      VDS_QUEUE, 
      1, 
      { 0, 0, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   
   pQueue = initQueueTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseQueueInit( pQueue, 
                            0, 1, &status, 4, 
                            "Queue1", SET_OFFSET(pQueue), &def, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueInsert( pQueue,
                              data,
                              8,
                              VDSE_QUEUE_FIRST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueInsert( pQueue,
                              data,
                              6,
                              VDSE_QUEUE_LAST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueGet( pQueue,
                           VDS_FIRST,
                           &pItem,
                           20,
                           &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->dataLength != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueGet( pQueue,
                           VDS_NEXT,
                           &pItem,
                           20,
                           &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->dataLength != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   txItemStatus = &pItem->txStatus;
   if ( txItemStatus->usageCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRelease( pQueue,
                               pItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( txItemStatus->usageCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.usageCounter != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pQueue->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
