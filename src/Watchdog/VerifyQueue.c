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

int vdswCheckQueueContent( vdswVerifyStruct * pVerify,
                           struct vdseQueue * pQueue )
{
   vdseTxStatus * txItemStatus;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL, * pDeleteNode = NULL;
   vdseQueueItem* pQueueItem = NULL;
   
//   pVerify->spaces += 2;
   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
   while ( listErrCode == LIST_OK )
   {
      pQueueItem = (vdseQueueItem*) 
         ((char*)pNode - offsetof( vdseQueueItem, node ));
      txItemStatus = &pQueueItem->txStatus;

      if ( txItemStatus->txOffset != NULL_OFFSET )
      {
         /*
          * So we have an interrupted transaction. What kind? 
          *   FLAG                      ACTION          
          *   TXS_ADDED                 remove item
          *   TXS_DESTROYED             reset txStatus  
          *   TXS_DESTROYED_COMMITTED   remove item
          *
          * Action is the equivalent of what a rollback would do.
          */
         if ( txItemStatus->enumStatus == VDSE_TXS_ADDED )
         {
            vdswEcho( pVerify, "Queue item added but not committed - item removed" );
            pDeleteNode = pNode;

         }         
         else if ( txItemStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED )
         {
            vdswEcho( pVerify, "Queue item deleted and committed - item removed" );
            pDeleteNode = pNode;
         }
         else if ( txItemStatus->enumStatus == VDSE_TXS_DESTROYED )
         {
            vdswEcho( pVerify, "Queue item deleted but not committed - item is kept" );
         }
         
         txItemStatus->txOffset = NULL_OFFSET;
         txItemStatus->enumStatus = VDSE_TXS_OK;
      }

      if ( pDeleteNode == NULL && txItemStatus->usageCounter != 0 )
      {
         vdswEcho( pVerify, "Queue item usage counter set to zero" );
         txItemStatus->usageCounter = 0;
      }
      
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
      /*
       * We need the old node to be able to get to the next node. That's
       * why we save the node to be deleted and delete it until after we
       * retrieve the next node.
       */
      if ( pDeleteNode != NULL )
      {
         vdseLinkedListRemoveItem( &pQueue->listOfElements, pDeleteNode );
         pDeleteNode = NULL;
      }
   }

   if ( listErrCode == LIST_END_OF_LIST || listErrCode == LIST_EMPTY )
      return 0;
   
   fprintf( stderr, "Abnormal error in list, list error code = %d\n", 
      listErrCode );
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyQueue( vdswVerifyStruct * pVerify,
                 struct vdseQueue * pQueue )
{
   vdseTxStatus * txQueueStatus;
   int rc;
   
   pVerify->spaces += 2;
   
   if ( vdscIsItLocked( &pQueue->memObject.lock ) )
   {
      vdscReleaseProcessLock ( &pQueue->memObject.lock );
   
      rc = vdswVerifyList( pVerify, &pQueue->memObject.listBlockGroup );
   //   rc = VerifyMemObject( &pQueue->memObject );
      rc = vdswVerifyList( pVerify, &pQueue->listOfElements );
   }

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txQueueStatus->txOffset != NULL_OFFSET )
   {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txQueueStatus->enumStatus == VDSE_TXS_ADDED )
      {
         vdswEcho( pVerify, "Object added but not committed - object removed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }         
      if ( txQueueStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED )
      {
         vdswEcho( pVerify, "Object deleted and committed - object removed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }
      vdswEcho( pVerify, "Object deleted but not committed - object is kept" );

      txQueueStatus->txOffset = NULL_OFFSET;
      txQueueStatus->enumStatus = VDSE_TXS_OK;
   }
   
   if ( txQueueStatus->usageCounter != 0 )
   {
      txQueueStatus->usageCounter = 0;
      vdswEcho( pVerify, "Usage counter set to zero" );
   }
   if ( txQueueStatus->parentCounter != 0 )
   {
      txQueueStatus->parentCounter = 0;
      vdswEcho( pVerify, "Parent counter set to zero" );
   }
   if ( pQueue->nodeObject.txCounter != 0 )
   {
      pQueue->nodeObject.txCounter = 0;
      vdswEcho( pVerify, "Transaction counter set to zero" );
   }

   rc = vdswCheckQueueContent( pVerify, pQueue );
   pVerify->spaces -= 2;

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

