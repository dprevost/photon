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
#include "Engine/MemoryObject.h"
#include "Engine/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyMemObject( struct vdswVerifyStruct   * pVerify,
                     struct vdseMemObject      * pMemObj,
                     struct vdseSessionContext * pContext )
{
   vdseLinkNode * dummy;
   enum ListErrors errGroup;
   vdseBlockGroup * pGroup;
   int rc;
   struct vdseMemAlloc * pAlloc = (vdseMemAlloc *) pContext->pAllocator;

   /*
    * Reset the bitmap and the the list of groups.
    */
   vdswResetBitmap( pVerify->pBitmap );
   vdseSetBufferAllocated( pVerify->pBitmap, 0, pAlloc->totalLength );

   rc = vdswVerifyList( pVerify, &pMemObj->listBlockGroup );
   if ( rc != 0 ) return rc;
   
   /*
    * The list is ok (or repaired), use it to populate the bitmap for
    * validating the content of the object itself.
    */
   vdswResetBitmap( pVerify->pBitmap );
   /*
    * We retrieve the first node
    */
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                       &dummy );
   VDS_PRE_CONDITION( errGroup == LIST_OK );

   while ( errGroup == LIST_OK )
   {
      pGroup = (vdseBlockGroup*)( 
         (unsigned char*)dummy - offsetof(vdseBlockGroup,node));
      

      errGroup = vdseLinkedListPeakNext( &pMemObj->listBlockGroup,
                                         dummy,
                                         &dummy );
   }
   
#if 0
   vdseTxStatus * txQueueStatus;
   int rc;
   bool bTestObject = false;
   
   pVerify->spaces += 2;
   
   if ( vdscIsItLocked( &pQueue->memObject.lock ) ) {
      vdswEcho( pVerify, "The object is locked - it might be corrupted" );
      if ( pVerify->doRepair ) {
         vdswEcho( pVerify, "Trying to reset the lock..." );
         vdscReleaseProcessLock ( &pQueue->memObject.lock );
      }
      rc = vdswVerifyList( pVerify, &pQueue->memObject.listBlockGroup );
   //   rc = VerifyMemObject( &pQueue->memObject );
      bTestObject = true;
   }

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txQueueStatus->txOffset != NULL_OFFSET ) {
      /*
       * So we have an interrupted transaction. What kind? 
       *   FLAG                      ACTION          
       *   TXS_ADDED                 remove object   
       *   TXS_DESTROYED             reset txStatus  
       *   TXS_DESTROYED_COMMITTED   remove object
       *
       * Action is the equivalent of what a rollback would do.
       */
      if ( txQueueStatus->enumStatus == VDSE_TXS_ADDED ) {
         vdswEcho( pVerify, "Object added but not committed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }         
      if ( txQueueStatus->enumStatus == VDSE_TXS_DESTROYED_COMMITTED ) {
         vdswEcho( pVerify, "Object deleted and committed" );
         pVerify->spaces -= 2;
         return VDSW_DELETE_OBJECT;
      }

      vdswEcho( pVerify, "Object deleted but not committed" );

      if ( pVerify->doRepair) {
         vdswEcho( pVerify, "Object deleted but not committed - resetting the delete flags" );
         txQueueStatus->txOffset = NULL_OFFSET;
         txQueueStatus->enumStatus = VDSE_TXS_OK;
      }
   }
   
   if ( txQueueStatus->usageCounter != 0 ) {
      vdswEcho( pVerify, "Usage counter is not zero" );
      if (pVerify->doRepair) {
         txQueueStatus->usageCounter = 0;
         vdswEcho( pVerify, "Usage counter set to zero" );
      }
   }
   if ( txQueueStatus->parentCounter != 0 ) {
      vdswEcho( pVerify, "Parent counter is not zero" );
      if (pVerify->doRepair) {
         txQueueStatus->parentCounter = 0;
         vdswEcho( pVerify, "Parent counter set to zero" );
      }
   }
   if ( pQueue->nodeObject.txCounter != 0 ) {
      vdswEcho( pVerify, "Transaction counter is not zero" );
      if (pVerify->doRepair) {
         pQueue->nodeObject.txCounter = 0;
         vdswEcho( pVerify, "Transaction counter set to zero" );
      }
   }
   
   if ( bTestObject )
      rc = vdswVerifyList( pVerify, &pQueue->listOfElements );

   rc = vdswCheckQueueContent( pVerify, pQueue );
   pVerify->spaces -= 2;
#endif

   return VDSW_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
