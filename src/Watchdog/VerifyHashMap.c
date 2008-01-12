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

int vdswCheckHashMapContent( vdswVerifyStruct   * pVerify,
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
      return 0;
   
//   pVerify->spaces += 2;
   
   listErr = vdseHashGetFirst( &pHashMap->hashObj,
                               &bucket, 
                               &offset );
   while ( listErr == LIST_OK )
   {
      GET_PTR( pItem, offset, vdseHashItem );
      txItemStatus = &pItem->txStatus;

      if ( txItemStatus->txOffset != NULL_OFFSET )
      {
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
         if ( txItemStatus->enumStatus == VDSE_TXS_ADDED )
         {
            vdswEcho( pVerify, "Hash item added but not committed - item removed" );
            deletedBucket = bucket;
            pDeletedItem = pItem;
         }         
         else if ( txItemStatus->enumStatus == VDSE_TXS_REPLACED )
         {
            vdswEcho( pVerify, "Hash item replaced but not committed - new item removed" );
            deletedBucket = bucket;
            pDeletedItem = pItem;
         }         
         else if ( txItemStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED )
         {
            vdswEcho( pVerify, "Hash item deleted and committed - item removed" );
            deletedBucket = bucket;
            pDeletedItem = pItem;
         }
         else if ( txItemStatus->enumStatus == VDSE_TXS_DESTROYED )
         {
            vdswEcho( pVerify, "Hash item deleted but not committed - item is kept" );
         }
         
         txItemStatus->txOffset = NULL_OFFSET;
         txItemStatus->enumStatus = VDSE_TXS_OK;
      }

      if ( pDeletedItem == NULL && txItemStatus->usageCounter != 0 )
      {
         vdswEcho( pVerify, "Hash item usage counter set to zero" );
         txItemStatus->usageCounter = 0;
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
      if ( pDeletedItem != NULL )
      {
         vdseHashDeleteAt( &pHashMap->hashObj,
                           deletedBucket,
                           pDeletedItem,
                           pContext );
         pDeletedItem = NULL;
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyHashMap( vdswVerifyStruct   * pVerify,
                   vdseHashMap        * pHashMap,
                   vdseSessionContext * pContext )
{
   vdseTxStatus * txHashMapStatus;
   int rc;
      
   pVerify->spaces += 2;
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txHashMapStatus->txOffset != NULL_OFFSET )
   {
      /*
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txHashMapStatus->enumStatus == VDSE_TXS_ADDED)
      {
         vdswEcho( pVerify, "Object added but not committed - object removed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }         
      if ( txHashMapStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED )
      {
         vdswEcho( pVerify, "Object deleted and committed - object removed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }
      vdswEcho( pVerify, "Object deleted but not committed - object is kept" );

      txHashMapStatus->txOffset = NULL_OFFSET;
      txHashMapStatus->enumStatus = VDSE_TXS_OK;
   }
   
   if ( txHashMapStatus->usageCounter != 0 )
   {
      txHashMapStatus->usageCounter = 0;
      vdswEcho( pVerify, "Usage counter set to zero" );
   }
   if ( txHashMapStatus->parentCounter != 0 )
   {
      txHashMapStatus->parentCounter = 0;
      vdswEcho( pVerify, "Parent counter set to zero" );
   }
   if ( pHashMap->nodeObject.txCounter != 0 )
   {
      pHashMap->nodeObject.txCounter = 0;
      vdswEcho( pVerify, "Transaction counter set to zero" );
   }

   rc = vdswCheckHashMapContent( pVerify, pHashMap, pContext );
   pVerify->spaces -= 2;

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

