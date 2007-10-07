/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/Queue.h"
#include "Engine/LinkedList.h"
#include "Engine/Transaction.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueInit( vdseQueue          * pQueue,
                   ptrdiff_t            parentOffset,
                   size_t               numberOfBlocks,
                   vdseTxStatus       * pTxStatus,
                   size_t               origNameLength,
                   vdsChar_T          * origName,
                   ptrdiff_t            keyOffset,
                   vdseSessionContext * pContext )
{
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pQueue    != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pTxStatus != NULL );
   VDS_PRE_CONDITION( origName  != NULL );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   
   errcode = vdseMemObjectInit( &pQueue->memObject, 
                                VDSE_IDENT_QUEUE,
                                &pQueue->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }
   pContext->pCurrentMemObject = &pQueue->memObject;

   vdseTreeNodeInit( &pQueue->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     keyOffset );

   vdseLinkedListInit( &pQueue->listOfElements );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueFini( vdseQueue          * pQueue,
                    vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   pContext->pCurrentMemObject = &pQueue->memObject;

   vdseLinkedListFini( &pQueue->listOfElements );
   vdseTreeNodeFini(   &pQueue->nodeObject );
   vdseMemObjectFini(  &pQueue->memObject, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueInsert( vdseQueue          * pQueue,
                     const void         * pItem, 
                     size_t               length ,
                     enum vdseQueueEnum   firstOrLast,
                     vdseSessionContext * pContext )
{
   vdseQueueItem* pQueueItem;
   vdsErrors errcode = VDS_OK;
   vdseTxStatus* txStatus;
   int rc;
   size_t allocLength;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( length  > 0 );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   txStatus = GET_PTR( pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pQueue->memObject, pContext ) == 0 )
   {
      if ( ! vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txStatus ) )
      {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( vdseQueueItem, data );
      pQueueItem = (vdseQueueItem *) vdseMalloc( &pQueue->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pQueueItem == NULL )
      {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
   
      pQueueItem->dataLength = length;
      memcpy( pQueueItem->data, pItem, length );
   
      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD,
                         SET_OFFSET(pQueue),
                         VDS_HASH_MAP,
                         SET_OFFSET(pQueueItem),
                         0,
                         pContext );
      if ( rc != 0 )
      {
         vdseFree( &pQueue->memObject, 
                   (unsigned char * )pQueueItem,
                   allocLength,
                   pContext );
         goto the_exit;
      }

      if ( firstOrLast == VDSE_QUEUE_FIRST )
         vdseLinkedListPutFirst( &pQueue->listOfElements,
                                 &pQueueItem->node );
      else
         vdseLinkedListPutLast( &pQueue->listOfElements,
                                &pQueueItem->node );

      vdseTxStatusSetTx( &pQueueItem->txStatus, SET_OFFSET(pContext->pTransaction) );
      pQueue->nodeObject.txCounter++;
      pQueueItem->txStatus.usageCounter++;

      vdseUnlock( &pQueue->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }
   
   return 0;

the_exit:

   vdseUnlock( &pQueue->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueRemove( vdseQueue          * pQueue,
                     vdseQueueItem     ** ppQueueItem,
                     enum vdseQueueEnum   firstOrLast,
                     vdseSessionContext * pContext )
{
   int rc;
   vdsErrors errcode = VDS_OK;
   enum ListErrors listErr = LIST_OK;
   vdseQueueItem * pItem = NULL;
   vdseTxStatus  * txStatus;
   vdseLinkNode  * pNode = NULL;

   VDS_PRE_CONDITION( pQueue      != NULL );
   VDS_PRE_CONDITION( ppQueueItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );
   
   txStatus = GET_PTR( pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pQueue->memObject, pContext ) == 0 )
   {
      if ( ! vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txStatus ) )
      {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /* This call can only fail if the queue is empty. */
      if ( firstOrLast == VDSE_QUEUE_FIRST )
         listErr = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
      else
         listErr = vdseLinkedListPeakLast( &pQueue->listOfElements, &pNode );

      while ( listErr == LIST_OK )
      {
         pItem = (vdseQueueItem *) ((char*)pNode - offsetof( vdseQueueItem, node ));
         txStatus = &pItem->txStatus;
      
         if ( vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction) ) 
                && ! vdseTxStatusIsMarkedAsDestroyed( txStatus ) 
                && pItem->txStatus.usageCounter == 0 )
         {
            /* Add to current transaction  */
            rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                               VDSE_TX_REMOVE,
                               SET_OFFSET( pQueue ),
                               VDS_QUEUE,
                               SET_OFFSET( pItem ),
                               0, /* irrelevant */
                               pContext );
            if ( rc != 0 ) 
               goto the_exit;
      
            vdseTxStatusSetTx( txStatus, SET_OFFSET(pContext->pTransaction) );
            pQueue->nodeObject.txCounter++;
            pItem->txStatus.usageCounter++;
            vdseTxStatusMarkAsDestroyed( txStatus );

            *ppQueueItem = pItem;

            vdseUnlock( &pQueue->memObject, pContext );

            return 0;
         }
         if ( firstOrLast == VDSE_QUEUE_FIRST )
            listErr = vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                              pNode, 
                                              &pNode );
         else
            listErr = vdseLinkedListPeakPrevious( &pQueue->listOfElements, 
                                                  pNode, 
                                                  &pNode );
      }
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   /* Let this falls through the error handler */
   errcode = VDS_IS_EMPTY;   
   
the_exit:

   vdseUnlock( &pQueue->memObject, pContext );
   /* vdscSetError might have been already called by some other function */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueGet( vdseQueue          * pQueue,
                  unsigned int         flag,
                  void               * pItem,
                  size_t               length,
                  void              ** ppIterator,
                  vdseSessionContext * pContext )
{
#if 0
vdsErrors Queue::GetItem( unsigned int    flag,
                          void          * pItem,
                          size_t          length,
                          void         ** ppIterator,
                          vdseSessionContext* pContext )
{
   QueueElement* pElement = NULL;
   QueueElement* pOldElement = NULL;
   vdsErrors errCode = VDS_IS_EMPTY;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL;
   
   if ( flag == VDS_NEXT )
   {
      errCode = VDS_REACHED_THE_END;
      pOldElement = (QueueElement*) *ppIterator;
      listErrCode =  vdseLinkedListPeakNext( &m_listOfElements, 
                                             &pOldElement->node, 
                                             &pNode,
                                             pContext->pAllocator );
   }
   else
      /* This call can only fail if the queue is empty. */
      listErrCode = vdseLinkedListPeakFirst( &m_listOfElements, 
                                             &pNode,
                                             pContext->pAllocator );

   // BUG remove Transaction cast, eventually
   Transaction* pTrans = (Transaction*) pContext->pTransaction;

   while ( listErrCode == LIST_OK )
   {
      pElement = (QueueElement*)
         ((char*)pNode - offsetof( QueueElement, node ));
      if ( pElement->
           IsTransactionValid( pTrans->TransactionId() ) 
           && ! pElement->IsMarkedAsDestroyed() )
      {
         if ( pElement->length > length )
            return VDS_INVALID_LENGTH_FIELD;

         memcpy( pItem, pElement->data, pElement->length );

         pElement->usageCounter++;
         *ppIterator = (void *) pElement;
         if ( flag == VDS_NEXT )
            ReleaseIterator( pOldElement, pContext );

         return VDS_OK;
      }
/*      fprintf(stderr, "pE: %p %d %d \n", pElement, pElement->usageCounter, */
/*              pElement->bMarkedForDeletion ); */
      
      listErrCode =  vdseLinkedListPeakNext( &m_listOfElements, 
                                             pNode, 
                                             &pNode,
                                             pContext->pAllocator );
   }

   /* If we come here, we are in error */
   if ( flag == VDS_NEXT )
      ReleaseIterator( *ppIterator, pContext );

   return errCode;
#endif
  return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueReleaseItem( vdseQueue          * pQueue,
                          vdseQueueItem      * pQueueItem,
                          vdseSessionContext * pContext )
{
   vdseTxStatus * txItemStatus, * txQueueStatus;
   size_t len;
   vdseFolder * pFolder;
   bool mustUnlock = true;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pQueueItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   pContext->pCurrentMemObject = &pQueue->memObject;

   txItemStatus = &pQueueItem->txStatus;
   txQueueStatus = GET_PTR( pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( vdseLock( &pQueue->memObject, pContext ) == 0 )
   {
      txItemStatus->usageCounter--;
      txQueueStatus->usageCounter--;

      if ( (txItemStatus->usageCounter == 0) && vdseTxStatusIsRemoveCommitted(txItemStatus) )
      {
         /* Time to really delete the record! */
         vdseLinkedListRemoveItem( &pQueue->listOfElements, 
                                   &pQueueItem->node );

         len =  pQueueItem->dataLength + offsetof( vdseQueueItem, data );
         vdseFree( &pQueue->memObject, 
                   (unsigned char *) pQueueItem, 
                   len, 
                   pContext );

         pQueue->nodeObject.txCounter--;

         if ( vdseTxStatusIsRemoveCommitted(txQueueStatus) )
         {
            /* 
             * The object is committed to be removed but was not yet removed  
             * since its data records were in use.  
             */
            if ( pQueue->nodeObject.txCounter == 0 && 
               txQueueStatus->usageCounter == 0 )
            {
               /*
                * No data record in use, no pending transactions on these data 
                * records. We can proceed with harakiri. 
                *
                * We use a blocking lock on the folder. It is a bit dangerous
                * but it should be ok since the queue itself is locked and no
                * other session is using it - in other words, this object 
                * cannot be involved in a transaction (the other place where
                * we can try to lock forever).
                */
               pFolder = GET_PTR( pQueue->nodeObject.myParentOffset, vdseFolder );
               vdseLockNoFailure( &pFolder->memObject, pContext );
               vdseUnlock( &pQueue->memObject, pContext );
               mustUnlock = false;
               
               vdseFolderRemoveObject( pFolder, 
                                       GET_PTR(pQueue->nodeObject.myKeyOffset, vdsChar_T),
                                       pQueue->nodeObject.myNameLength,
                                       pContext );
               //   pFolder->RemoveObject( this, 
               //                          VDS_QUEUE,
               //                          pContext );
               vdseUnlock( &pFolder->memObject, pContext );
               
            } /* end if - is someone using the queue */
         } /* end if IsRemoveCommitted on the queue itself */
      }
      /* If the queue was not destroyed, we unlock it */
      if ( mustUnlock )
         vdseUnlock( &pQueue->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueStatus( vdseQueue          * pQueue,
                     size_t             * pNumValidItems,
                     size_t             * pNumTotalItems,
                     vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pQueue         != NULL );
   VDS_PRE_CONDITION( pNumValidItems != NULL );
   VDS_PRE_CONDITION( pNumTotalItems != NULL );
   VDS_PRE_CONDITION( pContext       != NULL );
   
   if ( vdseLock( &pQueue->memObject, pContext ) == 0 )
   {
      *pNumTotalItems = pQueue->listOfElements.currentSize;
      *pNumValidItems = pQueue->numValidItems;

      vdseUnlock( &pQueue->memObject, pContext );
      
      return 0;
   }
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueCommitAdd( vdseQueue * pQueue, 
                         ptrdiff_t   itemOffset )
{
   vdseQueueItem * pQueueItem;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   pQueueItem = GET_PTR( itemOffset, vdseQueueItem );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. Clearing it is ok.
    */
   vdseTxStatusClearTx( &pQueueItem->txStatus );
   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueRollbackAdd( vdseQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           vdseSessionContext * pContext  )
{
   vdseQueueItem * pQueueItem;
   size_t len;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   pQueueItem = GET_PTR( itemOffset, vdseQueueItem );

   len =  pQueueItem->dataLength + offsetof( vdseQueueItem, data );
   /*
    * Needed ? 
    */
   pContext->pCurrentMemObject = &pQueue->memObject;

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. To rollback we need to remove it from the hash.
    * (this function will also free the memory back to the memory object).
    */
   vdseLinkedListRemoveItem( &pQueue->listOfElements,
                             &pQueueItem->node );
   vdseFree( &pQueue->memObject, 
             (unsigned char *) pQueueItem, 
             len, 
             pContext );

   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueCommitRemove( vdseQueue          * pQueue, 
                            ptrdiff_t            itemOffset,
                            vdseSessionContext * pContext )
{
   vdseQueueItem * pQueueItem;
   size_t len;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   pQueueItem = GET_PTR( itemOffset, vdseQueueItem );

   /* 
    * If someone is using it, the usageCounter will be greater than one.
    * If it isn't, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( pQueueItem->txStatus.usageCounter > 1 )
      vdseTxStatusCommitRemove( &pQueueItem->txStatus );
   else
   {
      len =  pQueueItem->dataLength + offsetof( vdseQueueItem, data );
      
      vdseLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      vdseFree( &pQueue->memObject, 
                (unsigned char * )pQueueItem,
                len,
                pContext );

      pQueue->nodeObject.txCounter--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueRollbackRemove( vdseQueue * pQueue, 
                              ptrdiff_t   itemOffset )
{
   vdseQueueItem * pQueueItem;
   
   VDS_PRE_CONDITION( pQueue     != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   pQueueItem = GET_PTR( itemOffset, vdseQueueItem );

   /*
    * This call resets the transaction (to "none"), decrement the
    * counter and remove the bit that flag this data as being in the
    * process of being removed.
    */
   vdseTxStatusUnmarkAsDestroyed(  &pQueueItem->txStatus );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors Queue::GetItem( unsigned int    flag,
                          void          * pItem,
                          size_t          length,
                          void         ** ppIterator,
                          vdseSessionContext* pContext )
{
   QueueElement* pElement = NULL;
   QueueElement* pOldElement = NULL;
   vdsErrors errCode = VDS_IS_EMPTY;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL;
   
   if ( flag == VDS_NEXT )
   {
      errCode = VDS_REACHED_THE_END;
      pOldElement = (QueueElement*) *ppIterator;
      listErrCode =  vdseLinkedListPeakNext( &m_listOfElements, 
                                             &pOldElement->node, 
                                             &pNode,
                                             pContext->pAllocator );
   }
   else
      /* This call can only fail if the queue is empty. */
      listErrCode = vdseLinkedListPeakFirst( &m_listOfElements, 
                                             &pNode,
                                             pContext->pAllocator );

   // BUG remove Transaction cast, eventually
   Transaction* pTrans = (Transaction*) pContext->pTransaction;

   while ( listErrCode == LIST_OK )
   {
      pElement = (QueueElement*)
         ((char*)pNode - offsetof( QueueElement, node ));
      if ( pElement->
           IsTransactionValid( pTrans->TransactionId() ) 
           && ! pElement->IsMarkedAsDestroyed() )
      {
         if ( pElement->length > length )
            return VDS_INVALID_LENGTH_FIELD;

         memcpy( pItem, pElement->data, pElement->length );

         pElement->usageCounter++;
         *ppIterator = (void *) pElement;
         if ( flag == VDS_NEXT )
            ReleaseIterator( pOldElement, pContext );

         return VDS_OK;
      }
/*      fprintf(stderr, "pE: %p %d %d \n", pElement, pElement->usageCounter, */
/*              pElement->bMarkedForDeletion ); */
      
      listErrCode =  vdseLinkedListPeakNext( &m_listOfElements, 
                                             pNode, 
                                             &pNode,
                                             pContext->pAllocator );
   }

   /* If we come here, we are in error */
   if ( flag == VDS_NEXT )
      ReleaseIterator( *ppIterator, pContext );

   return errCode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

