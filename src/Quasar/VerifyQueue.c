/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Quasar/VerifyCommon.h"
#include "Nucleus/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError
psoqCheckQueueContent( psoqVerifyStruct * pVerify,
                       struct psonQueue * pQueue )
{
   psonTxStatus * txItemStatus;
   psonLinkNode * pNode = NULL, * pDeletedNode = NULL;
   psonQueueItem* pQueueItem = NULL;
   enum psoqRecoverError rc = PSOQ_REC_OK;
   bool ok;
   
   ok = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   while ( ok ) {
      pQueueItem = (psonQueueItem*) 
         ((char*)pNode - offsetof( psonQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( txItemStatus->txOffset != PSON_NULL_OFFSET ) {
         /*
          * So we have an interrupted transaction. What kind? 
          *   FLAG                      ACTION          
          *   TXS_ADDED                 remove item
          *   TXS_DESTROYED             reset txStatus  
          *   TXS_DESTROYED_COMMITTED   remove item
          *
          * Action is the equivalent of what a rollback would do.
          */
         if ( txItemStatus->status & PSON_TXS_ADDED ) {
            psoqEcho( pVerify, "Queue item added but not committed" );
            pDeletedNode = pNode;
         }         
         else if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
            psoqEcho( pVerify, "Queue item deleted and committed" );
            pDeletedNode = pNode;
         }
         else if ( txItemStatus->status & PSON_TXS_DESTROYED ) {
            psoqEcho( pVerify, "Queue item deleted but not committed" );
         }
         
         if ( pDeletedNode == NULL && pVerify->doRepair ) {
            txItemStatus->txOffset = PSON_NULL_OFFSET;
            txItemStatus->status = PSON_TXS_OK;
            psoqEcho( pVerify, "Queue item status fields reset to zero" );
         }
         rc = PSOQ_REC_CHANGES;
      }

      if ( pDeletedNode == NULL && txItemStatus->usageCounter != 0 ) {
         rc = PSOQ_REC_CHANGES;
         psoqEcho( pVerify, "Queue item usage counter is not zero" );
         if (pVerify->doRepair) {
            psoqEcho( pVerify, "Queue item usage counter set to zero" );
            txItemStatus->usageCounter = 0;
         }
      }
      
      ok =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                    pNode, 
                                    &pNode );
      /*
       * We need the old node to be able to get to the next node. That's
       * why we save the node to be deleted and delete it until after we
       * retrieve the next node.
       */
      if ( pDeletedNode != NULL && pVerify->doRepair ) {
         psonLinkedListRemoveItem( &pQueue->listOfElements, pDeletedNode );
         psoqEcho( pVerify, "Queue item removed from shared memory" );
      }
      pDeletedNode = NULL;
   }

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError 
psoqVerifyQueue( psoqVerifyStruct   * pVerify,
                 struct psonQueue   * pQueue,
                 psonSessionContext * pContext )
{
   psonTxStatus * txQueueStatus;
   enum psoqRecoverError rc = PSOQ_REC_OK, rc2;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   if ( psocIsItLocked( &pQueue->memObject.lock ) ) {
      psoqEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         psoqEcho( pVerify, "Trying to reset the lock..." );
         psocReleaseProcessLock ( &pQueue->memObject.lock );
      }
      rc = psoqVerifyMemObject( pVerify, &pQueue->memObject, pContext );
      if ( rc > PSOQ_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc;
      }
      rc = PSOQ_REC_CHANGES; 
      bTestObject = true;
   }
   
   /*
    * Currently, the bitmap is only use if the object was locked. This might
    * change (as it is the case for other types of objects) so populate the 
    * bitmap in all cases to be safe.
    */
   psoqPopulateBitmap( pVerify, &pQueue->memObject, pContext );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );

   if ( txQueueStatus->txOffset != PSON_NULL_OFFSET ) {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txQueueStatus->status & PSON_TXS_ADDED ) {
         psoqEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return PSOQ_REC_DELETED_OBJECT;
      }
      if ( txQueueStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
         psoqEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return PSOQ_REC_DELETED_OBJECT;
      }

      psoqEcho( pVerify, "Object deleted but not committed" );
      rc = PSOQ_REC_CHANGES;
      if ( pVerify->doRepair) {
         psoqEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txQueueStatus->txOffset = PSON_NULL_OFFSET;
         txQueueStatus->status = PSON_TXS_OK;
      }
   }
   
   if ( txQueueStatus->usageCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txQueueStatus->usageCounter = 0;
         psoqEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txQueueStatus->parentCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txQueueStatus->parentCounter = 0;
         psoqEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pQueue->nodeObject.txCounter = 0;
         psoqEcho( pVerify, "Transaction counter set to zero" );
      }
   }
   
   if ( bTestObject ) {
      rc2 = psoqVerifyList( pVerify, &pQueue->listOfElements );
      if ( rc2 > PSOQ_REC_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or PSOQ_REC_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }
   
   rc2 = psoqCheckQueueContent( pVerify, pQueue );
   /* At this point rc is either 0 or PSOQ_REC_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

