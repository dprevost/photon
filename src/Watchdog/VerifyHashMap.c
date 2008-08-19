/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError
vdswCheckHashMapContent( vdswVerifyStruct   * pVerify,
                         psnHashMap        * pHashMap, 
                         psnSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   psnHashItem * pItem, * pDeletedItem = NULL;
   psnTxStatus * txItemStatus;
   enum vdswRecoverError rc = VDSWR_OK;
   bool found;
   
   /* The easy case */
   if ( pHashMap->hashObj.numberOfItems == 0 ) return rc;
   
   found = psnHashGetFirst( &pHashMap->hashObj, &offset );
   while ( found ) {
      GET_PTR( pItem, offset, psnHashItem );
      txItemStatus = &pItem->txStatus;

      if ( txItemStatus->txOffset != PSN_NULL_OFFSET ) {
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
         if ( txItemStatus->status & PSN_TXS_ADDED ) {
            vdswEcho( pVerify, "Hash item added but not committed" );
            pDeletedItem = pItem;
         }         
         else if ( txItemStatus->status & PSN_TXS_REPLACED ) {
            vdswEcho( pVerify, "Hash item replaced but not committed" );
            pDeletedItem = pItem;
         }
         else if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
            vdswEcho( pVerify, "Hash item deleted and committed" );
            pDeletedItem = pItem;
         }
         else if ( txItemStatus->status & PSN_TXS_DESTROYED ) {
            vdswEcho( pVerify, "Hash item deleted but not committed" );
         }
         
         if ( pDeletedItem == NULL && pVerify->doRepair ) {
            txItemStatus->txOffset = PSN_NULL_OFFSET;
            txItemStatus->status = PSN_TXS_OK;
            vdswEcho( pVerify, "Hash item status fields reset to zero" );
         }
         rc = VDSWR_CHANGES;
      }
      
      if ( pDeletedItem == NULL && txItemStatus->usageCounter != 0 ) {
         rc = VDSWR_CHANGES;
         vdswEcho( pVerify, "Hash item usage counter is not zero" );
         if (pVerify->doRepair) {
            txItemStatus->usageCounter = 0;
            vdswEcho( pVerify, "Hash item usage counter set to zero" );
         }
      }
      
      previousOffset = offset;
      found = psnHashGetNext( &pHashMap->hashObj,
                               previousOffset,
                               &offset );

      /*
       * We need the old item to be able to get to the next item. That's
       * why we save the item to be deleted and delete it until after we
       * retrieve the next item.
       */
      if ( pDeletedItem != NULL && pVerify->doRepair ) {
         psnHashDelWithItem( &pHashMap->hashObj,
                              pDeletedItem,
                              pContext );
         vdswEcho( pVerify, "Hash item removed from the VDS" );
      }
      pDeletedItem = NULL;
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError 
vdswVerifyHashMap( vdswVerifyStruct   * pVerify,
                   psnHashMap        * pHashMap,
                   psnSessionContext * pContext )
{
   psnTxStatus * txHashMapStatus;
   enum vdswRecoverError rc = VDSWR_OK, rc2;
   bool bTestObject = false;
      
   pVerify->spaces += 2;

   /* Is the object lock ? */
   if ( pscIsItLocked( &pHashMap->memObject.lock ) ) {
      vdswEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         vdswEcho( pVerify, "Trying to reset the lock..." );
         pscReleaseProcessLock ( &pHashMap->memObject.lock );
      }
      rc = vdswVerifyMemObject( pVerify, &pHashMap->memObject, pContext );
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
   vdswPopulateBitmap( pVerify, &pHashMap->memObject, pContext );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( txHashMapStatus->txOffset != PSN_NULL_OFFSET ) {
      /*
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txHashMapStatus->status & PSN_TXS_ADDED) {
         vdswEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return VDSWR_DELETED_OBJECT;
      }
      if ( txHashMapStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
         vdswEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return VDSWR_DELETED_OBJECT;
      }
      
      vdswEcho( pVerify, "Object deleted but not committed" );
      rc = VDSWR_CHANGES;
      if ( pVerify->doRepair) {
         vdswEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txHashMapStatus->txOffset = PSN_NULL_OFFSET;
         txHashMapStatus->status = PSN_TXS_OK;
      }
   }

   if ( txHashMapStatus->usageCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txHashMapStatus->usageCounter = 0;
         vdswEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txHashMapStatus->parentCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txHashMapStatus->parentCounter = 0;
         vdswEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pHashMap->nodeObject.txCounter != 0 ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pHashMap->nodeObject.txCounter = 0;
         vdswEcho( pVerify, "Transaction counter set to zero" );
      }
   }

   if ( bTestObject ) {
      rc2 = vdswVerifyHash( pVerify, 
                           &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject) );
      if ( rc2 > VDSWR_START_ERRORS ) {
         pVerify->spaces -= 2;
         return rc2;
      }
      /* At this point rc is either 0 or VDSWR_CHANGES - same for rc2 */
      if ( rc2 > rc ) rc = rc2;
   }

   rc2 = vdswCheckHashMapContent( pVerify, pHashMap, pContext );
   /* At this point rc is either 0 or VDSWR_CHANGES */
   if ( rc2 > rc ) rc = rc2;
   pVerify->spaces -= 2;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

