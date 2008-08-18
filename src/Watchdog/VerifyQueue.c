/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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
#include "Watchdog/VerifyCommon.h"
#include "Engine/Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError
vdswCheckQueueContent( vdswVerifyStruct * pVerify,
                       struct vdseQueue * pQueue )
{
   vdseTxStatus * txItemStatus;
   vdseLinkNode * pNode = NULL, * pDeletedNode = NULL;
   vdseQueueItem* pQueueItem = NULL;
   enum vdswRecoverError rc = VDSWR_OK;
   bool ok;
   
   ok = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   while ( ok ) {
      pQueueItem = (vdseQueueItem*) 
         ((char*)pNode - offsetof( vdseQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( txItemStatus->txOffset != VDSE_NULL_OFFSET ) {
         /*
          * So we have an interrupted transaction. What kind? 
          *   FLAG                      ACTION          
          *   TXS_ADDED                 remove item
          *   TXS_DESTROYED             reset txStatus  
          *   TXS_DESTROYED_COMMITTED   remove item
          *
          * Action is the equivalent of what a rollback would do.
          */
         if ( txItemStatus->status & VDSE_TXS_ADDED ) {
            vdswEcho( pVerify, "Queue item added but not committed" );
            pDeletedNode = pNode;
         }         
         else if ( txItemStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
            vdswEcho( pVerify, "Queue item deleted and committed" );
            pDeletedNode = pNode;
         }
         else if ( txItemStatus->status & VDSE_TXS_DESTROYED ) {
            vdswEcho( pVerify, "Queue item deleted but not committed" );
         }
         
         if ( pDeletedNode == NULL && pVerify->doRepair ) {
            txItemStatus->txOffset = VDSE_NULL_OFFSET;
            txItemStatus->status = VDSE_TXS_OK;
            vdswEcho( pVerify, "Queue item status fields reset to zero" );
         }
         rc = VDSWR_CHANGES;
      }

      if ( pDeletedNode == NULL && txItemStatus->usageCounter != 0 ) {
         rc = VDSWR_CHANGES;
         vdswEcho( pVerify, "Queue item usage counter is not zero" );
         if (pVerify->doRepair) {
            vdswEcho( pVerify, "Queue item usage counter set to zero" );
            txItemStatus->usageCounter = 0;
         }
      }
      
      ok =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                    pNode, 
                                    &pNode );
      /*
       * We need the old node to be able to get to the next node. That's
       * why we save the node to be deleted and delete it until after we
       * retrieve the next node.
       */
      if ( pDeletedNode != NULL && pVerify->doRepair ) {
         vdseLinkedListRemoveItem( &pQueue->listOfElements, pDeletedNode );
         vdswEcho( pVerify, "Queue item removed from the VDS" );
      }
      pDeletedNode = NULL;
   }

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError 
vdswVerifyQueue( vdswVerifyStruct   * pVerify,
                 struct vdseQueue   * pQueue,
                 vdseSessionContext * pContext )
{
   vdseTxStatus * txQueueStatus;
   enum vdswRecoverError rc = VDSWR_OK, rc2;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   if ( pscIsItLocked( &pQueue->memObject.lock ) ) {
      vdswEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         vdswEcho( pVerify, "Trying to reset the lock..." );
         pscReleaseProcessLock ( &pQueue->memObject.lock );
      }
      rc = vdswVerifyMemObject( pVerify, &pQueue->memObject, pContext );
      if ( rc > VDSWR_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc;
      }
      rc = VDSWR_CHANGES; 
      bTestObject = true;
   }
   
   /*
    * Currently, the bitmap is only use if the object was locked. This might
    * change (as it is the case for other types of objects) so populate the 
    * bitmap in all cases to be safe.
    */
   vdswPopulateBitmap( pVerify, &pQueue->memObject, pContext );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txQueueStatus->txOffset != VDSE_NULL_OFFSET ) {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txQueueStatus->status & VDSE_TXS_ADDED ) {
         vdswEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return VDSWR_DELETED_OBJECT;
      }
      if ( txQueueStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
         vdswEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return VDSWR_DELETED_OBJECT;
      }

      vdswEcho( pVerify, "Object deleted but not committed" );
      rc = VDSWR_CHANGES;
      if ( pVerify->doRepair) {
         vdswEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txQueueStatus->txOffset = VDSE_NULL_OFFSET;
         txQueueStatus->status = VDSE_TXS_OK;
      }
   }
   
   if ( txQueueStatus->usageCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txQueueStatus->usageCounter = 0;
         vdswEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txQueueStatus->parentCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txQueueStatus->parentCounter = 0;
         vdswEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pQueue->nodeObject.txCounter = 0;
         vdswEcho( pVerify, "Transaction counter set to zero" );
      }
   }
   
   if ( bTestObject ) {
      rc2 = vdswVerifyList( pVerify, &pQueue->listOfElements );
      if ( rc2 > VDSWR_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or VDSWR_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }
   
   rc2 = vdswCheckQueueContent( pVerify, pQueue );
   /* At this point rc is either 0 or VDSWR_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

