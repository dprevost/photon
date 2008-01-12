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
#include "Engine/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswCheckHashMapContent( vdswVerifyStruct   * pVerify,
                             vdseHashMap        * pHashMap, 
                             vdseSessionContext * pContext )
{
   enum ListErrors listErr;
   size_t bucket, previousBucket, deletedBucket = -1;
   ptrdiff_t offset, previousOffset;
   vdseHashItem * pItem, * pDeletedItem = NULL;
   vdseTxStatus * txItemStatus;
   
   /* The easy case */
   if ( pHashMap->hashObj.numberOfItems == 0 )
      return;
   
//   pVerify->spaces += 2;
   
   listErr = vdseHashGetFirst( &pHashMap->hashObj,
                               &bucket, 
                               &offset );
   while ( listErr == LIST_OK ) {
      GET_PTR( pItem, offset, vdseHashItem );
      txItemStatus = &pItem->txStatus;

      if ( txItemStatus->txOffset != NULL_OFFSET ) {
         /*
          * So we have an interrupted transaction. What kind? 
          *   FLAG                      ACTION          
          *   TXS_ADDED                 remove item
          *   TXS_REPLACED              remove item (the older item is also reset)
          *   TXS_DESTROYED             reset txStatus  
          *   TXS_DESTROYED_COMMITTED   remove item
          *
          * Action is the equivalent of what a rollback would do.
          */
         if ( txItemStatus->enumStatus == VDSE_TXS_ADDED ) {
            vdswEcho( pVerify, "Hash item added but not committed" );
            deletedBucket = bucket;
            pDeletedItem = pItem;
         }         
         else if ( txItemStatus->enumStatus == VDSE_TXS_REPLACED ) {
            vdswEcho( pVerify, "Hash item replaced but not committed" );
            deletedBucket = bucket;
            pDeletedItem = pItem;
         }
         else if ( txItemStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED ) {
            vdswEcho( pVerify, "Hash item deleted and committed" );
            deletedBucket = bucket;
            pDeletedItem = pItem;
         }
         else if ( txItemStatus->enumStatus == VDSE_TXS_DESTROYED ) {
            vdswEcho( pVerify, "Hash item deleted but not committed" );
         }
         
         if ( pDeletedItem == NULL && pVerify->doRepair ) {
            txItemStatus->txOffset = NULL_OFFSET;
            txItemStatus->enumStatus = VDSE_TXS_OK;
            vdswEcho( pVerify, "Hash item status fields reset to zero" );
         }
      }
      
      if ( pDeletedItem == NULL && txItemStatus->usageCounter != 0 ) {
         vdswEcho( pVerify, "Hash item usage counter is not zero" );
         if (pVerify->doRepair) {
            txItemStatus->usageCounter = 0;
            vdswEcho( pVerify, "Hash item usage counter set to zero" );
         }
      }
      
      previousBucket = bucket;
      previousOffset = offset;
      listErr = vdseHashGetNext( &pHashMap->hashObj,
                                 previousBucket,
                                 previousOffset,
                                 &bucket, 
                                 &offset );

      /*
       * We need the old item to be able to get to the next item. That's
       * why we save the item to be deleted and delete it until after we
       * retrieve the next item.
       */
      if ( pDeletedItem != NULL && pVerify->doRepair ) {
         vdseHashDeleteAt( &pHashMap->hashObj,
                           deletedBucket,
                           pDeletedItem,
                           pContext );
         vdswEcho( pVerify, "Hash item removed from the VDS" );
      }
      pDeletedItem = NULL;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyHashMap( vdswVerifyStruct   * pVerify,
                   vdseHashMap        * pHashMap,
                   vdseSessionContext * pContext )
{
   vdseTxStatus * txHashMapStatus;
   int rc;
   bool bTestObject = false;
      
   pVerify->spaces += 2;

   /* Is the object lock ? */
   if ( vdscIsItLocked( &pHashMap->memObject.lock ) ) {
      vdswEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         vdswEcho( pVerify, "Trying to reset the lock..." );
         vdscReleaseProcessLock ( &pHashMap->memObject.lock );
      }
      bTestObject = true;
   }

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txHashMapStatus->txOffset != NULL_OFFSET ) {
      /*
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txHashMapStatus->enumStatus == VDSE_TXS_ADDED) {
         vdswEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }
      if ( txHashMapStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED ) {
         vdswEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }
      
      vdswEcho( pVerify, "Object deleted but not committed" );
      if ( pVerify->doRepair) {
         vdswEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txHashMapStatus->txOffset = NULL_OFFSET;
         txHashMapStatus->enumStatus = VDSE_TXS_OK;
      }
   }

   if ( txHashMapStatus->usageCounter != 0 ) {
      vdswEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txHashMapStatus->usageCounter = 0;
         vdswEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txHashMapStatus->parentCounter != 0 ) {
      vdswEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txHashMapStatus->parentCounter = 0;
         vdswEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      vdswEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pHashMap->nodeObject.txCounter = 0;
         vdswEcho( pVerify, "Transaction counter set to zero" );
      }
   }

   if ( bTestObject )
      rc = vdswVerifyHash( pVerify, 
                           &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject) );

   vdswCheckHashMapContent( pVerify, pHashMap, pContext );
   pVerify->spaces -= 2;

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

