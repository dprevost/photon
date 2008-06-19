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

#include "Engine/Queue.h"
#include "Engine/LinkedList.h"
#include "Engine/Transaction.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void vdseQueueReleaseNoLock( vdseQueue          * pQueue,
                             vdseQueueItem      * pQueueItem,
                             vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueCommitAdd( vdseQueue * pQueue, 
                         ptrdiff_t   itemOffset )
{
   vdseQueueItem * pQueueItem;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, vdseQueueItem );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. Clearing it is ok.
    */
   vdseTxStatusSetTx( &pQueueItem->txStatus, NULL_OFFSET );
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

   GET_PTR( pQueueItem, itemOffset, vdseQueueItem );

   /* 
    * If someone is using it, the usageCounter will be greater than one.
    * If it isn't, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( pQueueItem->txStatus.usageCounter == 0 ) {
      len =  pQueueItem->dataLength + offsetof( vdseQueueItem, data );
      
      vdseLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      vdseFree( &pQueue->memObject, 
                (unsigned char * )pQueueItem,
                len,
                pContext );

      pQueue->nodeObject.txCounter--;
   }
   else {
      vdseTxStatusCommitRemove( &pQueueItem->txStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueFini( vdseQueue          * pQueue,
                    vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   vdseLinkedListFini( &pQueue->listOfElements );
   vdseTreeNodeFini(   &pQueue->nodeObject );
   vdseMemObjectFini(  &pQueue->memObject, VDSE_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueGet( vdseQueue          * pQueue,
                  unsigned int         flag,
                  vdseQueueItem     ** ppIterator,
                  size_t               bufferLength,
                  vdseSessionContext * pContext )
{
   vdseQueueItem* pQueueItem = NULL;
   vdseQueueItem* pOldItem = NULL;
   vdsErrors errcode;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL;
   vdseTxStatus * txItemStatus, * txQueueStatus;
   bool isOK;
   bool queueIsEmpty = true;
   
   VDS_PRE_CONDITION( pQueue     != NULL );
   VDS_PRE_CONDITION( ppIterator != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( flag == VDS_FIRST || flag == VDS_NEXT );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );
   
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( vdseLock( &pQueue->memObject, pContext ) == 0 ) {
      if ( flag == VDS_NEXT ) {
         
         errcode = VDS_REACHED_THE_END;
         pOldItem = (vdseQueueItem*) *ppIterator;
         listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                                &pOldItem->node, 
                                                &pNode );
      }
      else {
         /* This call can only fail if the queue is empty. */
         listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, 
                                                &pNode );
      }
      
      while ( listErrCode == LIST_OK ) {
         pQueueItem = (vdseQueueItem*)
            ((char*)pNode - offsetof( vdseQueueItem, node ));
         txItemStatus = &pQueueItem->txStatus;
         
         /* 
          * If the transaction id of the item (to retrieve) is equal to the 
          * current transaction id AND the object is marked as deleted, we 
          * go to the next item.
          *
          * If the transaction id of the item (to retrieve) is NOT equal to the 
          * current transaction id AND the object is added... next!
          *
          * If the item is flagged as deleted and committed, it does not exists
          * from the API point of view.
          */
         isOK = true;
         if ( txItemStatus->txOffset != NULL_OFFSET ) {
            switch( txItemStatus->enumStatus ) {

            case VDSE_TXS_DESTROYED_COMMITTED:
               isOK = false;
               break;
               
            case VDSE_TXS_DESTROYED:
               if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
                  isOK = false;
                  queueIsEmpty = false;
               }
               break;
               
            case VDSE_TXS_ADDED:
               if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) ) {
                  isOK = false;
                  queueIsEmpty = false;
               }
               break;
               
            default:
               break;
            }
         }
         if ( isOK ) {
            /*
             * This test cannot be done in the API (before calling the current
             * function) since we do not know the item size. It could be done
             * after but this way makes the code faster.
             */
            if ( bufferLength < pQueueItem->dataLength ) {
               vdseUnlock( &pQueue->memObject, pContext );
               vdscSetError( &pContext->errorHandler,
                             g_vdsErrorHandle,
                             VDS_INVALID_LENGTH );
                return -1;
            }

            txItemStatus->usageCounter++;
            txQueueStatus->usageCounter++;
            *ppIterator = pQueueItem;
            if ( flag == VDS_NEXT ) {
               vdseQueueReleaseNoLock( pQueue, pOldItem, pContext );
            }
            
            vdseUnlock( &pQueue->memObject, pContext );

            return 0;
         }
         listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                                pNode, 
                                                &pNode );
      }
   }
   else {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }
   
   errcode = VDS_ITEM_IS_IN_USE;
   if ( queueIsEmpty ) {
      errcode = VDS_IS_EMPTY;   
      if ( flag == VDS_NEXT ) errcode = VDS_REACHED_THE_END;
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppIterator = NULL;
   if ( flag == VDS_NEXT ) {
      vdseQueueReleaseNoLock( pQueue, pOldItem, pContext );
   }
   
   vdseUnlock( &pQueue->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueInit( vdseQueue           * pQueue,
                   ptrdiff_t             parentOffset,
                   size_t                numberOfBlocks,
                   vdseTxStatus        * pTxStatus,
                   size_t                origNameLength,
                   char                * origName,
                   ptrdiff_t             keyOffset,
                   vdsObjectDefinition * pDefinition,
                   vdseSessionContext  * pContext )
{
   vdsErrors errcode;
   vdseFieldDef * ptr;
   unsigned int i;
   
   VDS_PRE_CONDITION( pQueue       != NULL );
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( pTxStatus    != NULL );
   VDS_PRE_CONDITION( origName     != NULL );
   VDS_PRE_CONDITION( pDefinition  != NULL );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   VDS_PRE_CONDITION( pDefinition->numFields > 0 );
   
   errcode = vdseMemObjectInit( &pQueue->memObject, 
                                VDSE_IDENT_QUEUE,
                                &pQueue->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }

   vdseTreeNodeInit( &pQueue->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     keyOffset );

   vdseLinkedListInit( &pQueue->listOfElements );

   pQueue->numFields = (uint16_t) pDefinition->numFields;

   ptr = (vdseFieldDef*) vdseMalloc( &pQueue->memObject, 
                                     pQueue->numFields* sizeof(vdseFieldDef),
                                     pContext );
   if ( ptr == NULL ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      return -1;
   }
   pQueue->dataDefOffset = SET_OFFSET(ptr);

   for ( i = 0; i < pQueue->numFields; ++i) {
      memcpy( ptr[i].name, pDefinition->fields[i].name, VDS_MAX_FIELD_LENGTH );
      ptr[i].type = pDefinition->fields[i].type;
      switch( ptr[i].type ) {
      case VDS_INTEGER:
      case VDS_BINARY:
      case VDS_STRING:
         ptr[i].length1 = pDefinition->fields[i].length;
         break;
      case VDS_DECIMAL:
         ptr[i].length1 = pDefinition->fields[i].precision;
         ptr[i].length2 = pDefinition->fields[i].scale;         
         break;
      case VDS_BOOLEAN:
         break;
      case VDS_VAR_BINARY:
      case VDS_VAR_STRING:
         ptr[i].length1 = pDefinition->fields[i].minLength;
         ptr[i].length2 = pDefinition->fields[i].maxLength;
         
      }
   }

   return 0;
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
   vdseTxStatus* txQueueStatus;
   int rc;
   size_t allocLength;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( length  > 0 );
   VDS_PRE_CONDITION( firstOrLast == VDSE_QUEUE_FIRST || 
      firstOrLast == VDSE_QUEUE_LAST );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pQueue->memObject, pContext ) == 0 ) {
      if ( ! vdseTxStatusIsValid( txQueueStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txQueueStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( vdseQueueItem, data );
      pQueueItem = (vdseQueueItem *) vdseMalloc( &pQueue->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pQueueItem == NULL ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
   
      vdseLinkNodeInit( &pQueueItem->node );
      pQueueItem->dataLength = length;
      memcpy( pQueueItem->data, pItem, length );
   
      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD_DATA,
                         SET_OFFSET(pQueue),
                         VDSE_IDENT_QUEUE,
                         SET_OFFSET(pQueueItem),
                         0,
                         pContext );
      if ( rc != 0 ) {
         vdseFree( &pQueue->memObject, 
                   (unsigned char * )pQueueItem,
                   allocLength,
                   pContext );
         goto the_exit;
      }

      if ( firstOrLast == VDSE_QUEUE_FIRST ) {
         vdseLinkedListPutFirst( &pQueue->listOfElements,
                                 &pQueueItem->node );
      }
      else {
         vdseLinkedListPutLast( &pQueue->listOfElements,
                                &pQueueItem->node );
      }
      
      vdseTxStatusInit( &pQueueItem->txStatus, SET_OFFSET(pContext->pTransaction) );
      pQueue->nodeObject.txCounter++;
      pQueueItem->txStatus.enumStatus = VDSE_TXS_ADDED;

      vdseUnlock( &pQueue->memObject, pContext );
   }
   else {
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
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
    
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueRelease( vdseQueue          * pQueue,
                      vdseQueueItem      * pQueueItem,
                      vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pQueueItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   if ( vdseLock( &pQueue->memObject, pContext ) == 0 ) {
      vdseQueueReleaseNoLock( pQueue,
                              pQueueItem,
                              pContext );
                             
      vdseUnlock( &pQueue->memObject, pContext );
   }
   else {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This version of the function is to be used when:
 *  - you know that the object is in use and should not/cannot be removed
 *  - the calling function holds the lock
 */
static
void vdseQueueReleaseNoLock( vdseQueue          * pQueue,
                             vdseQueueItem      * pQueueItem,
                             vdseSessionContext * pContext )
{
   vdseTxStatus * txItemStatus, * txQueueStatus;
   size_t len;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pQueueItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );

   txItemStatus = &pQueueItem->txStatus;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );
   
   txItemStatus->usageCounter--;
   txQueueStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      vdseTxStatusIsRemoveCommitted(txItemStatus) ) {
      /* Time to really delete the record! */
      vdseLinkedListRemoveItem( &pQueue->listOfElements, 
                                &pQueueItem->node );

      len =  pQueueItem->dataLength + offsetof( vdseQueueItem, data );
      vdseFree( &pQueue->memObject, 
                (unsigned char *) pQueueItem, 
                len, 
                pContext );

      pQueue->nodeObject.txCounter--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseQueueRemove( vdseQueue          * pQueue,
                     vdseQueueItem     ** ppQueueItem,
                     enum vdseQueueEnum   firstOrLast,
                     size_t               bufferLength,
                     vdseSessionContext * pContext )
{
   int rc;
   vdsErrors errcode = VDS_OK;
   enum ListErrors listErr = LIST_OK;
   vdseQueueItem * pItem = NULL;
   vdseTxStatus  * txParentStatus, * txItemStatus;
   vdseLinkNode  * pNode = NULL;
   bool queueIsEmpty = true;

   VDS_PRE_CONDITION( pQueue      != NULL );
   VDS_PRE_CONDITION( ppQueueItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( firstOrLast == VDSE_QUEUE_FIRST || 
      firstOrLast == VDSE_QUEUE_LAST );
   VDS_PRE_CONDITION( pQueue->memObject.objType == VDSE_IDENT_QUEUE );
   
   GET_PTR( txParentStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pQueue->memObject, pContext ) == 0 ) {
      if ( ! vdseTxStatusIsValid( txParentStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txParentStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /* This call can only fail if the queue is empty. */
      if ( firstOrLast == VDSE_QUEUE_FIRST ) {
         listErr = vdseLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
      }
      else {
         listErr = vdseLinkedListPeakLast( &pQueue->listOfElements, &pNode );
      }
      
      while ( listErr == LIST_OK ) {
         pItem = (vdseQueueItem *) ((char*)pNode - offsetof( vdseQueueItem, node ));
         txItemStatus = &pItem->txStatus;
      
         /* 
          * If the transaction id of the item is non-zero, a big no-no - 
          * we do not support two transactions on the same data
          * (and if remove is committed - the data is "non-existent").
          */
         if ( txItemStatus->txOffset == NULL_OFFSET ) {
            /*
             * This test cannot be done in the API (before calling the current
             * function) since we do not know the item size. It could be done
             * after but this way makes the code faster.
             */
            if ( bufferLength < pItem->dataLength ) {
               errcode = VDS_INVALID_LENGTH;
               goto the_exit;
            }

            /* Add to current transaction  */
            rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                               VDSE_TX_REMOVE_DATA,
                               SET_OFFSET( pQueue ),
                               VDSE_IDENT_QUEUE,
                               SET_OFFSET( pItem ),
                               0, /* irrelevant */
                               pContext );
            if ( rc != 0 ) goto the_exit;
      
            vdseTxStatusSetTx( txItemStatus, SET_OFFSET(pContext->pTransaction) );
            pQueue->nodeObject.txCounter++;
            txItemStatus->usageCounter++;
            txParentStatus->usageCounter++;
            vdseTxStatusMarkAsDestroyed( txItemStatus );

            *ppQueueItem = pItem;

            vdseUnlock( &pQueue->memObject, pContext );

            return 0;
         }
         /* We test the type of flag to return the proper error code (if needed) */
         if ( ! vdseTxStatusIsRemoveCommitted(txItemStatus) ) {
            queueIsEmpty = false;
         }
         
         if ( firstOrLast == VDSE_QUEUE_FIRST ) {
            listErr = vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                              pNode, 
                                              &pNode );
         }
         else {
            listErr = vdseLinkedListPeakPrevious( &pQueue->listOfElements, 
                                                  pNode, 
                                                  &pNode );
         }
      }
   }
   else {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   /* Let this falls through the error handler */
   errcode = VDS_ITEM_IS_IN_USE;
   if ( queueIsEmpty ) errcode = VDS_IS_EMPTY;   
   
the_exit:

   vdseUnlock( &pQueue->memObject, pContext );
   /* vdscSetError might have been already called by some other function */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueRollbackAdd( vdseQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           vdseSessionContext * pContext  )
{
   vdseQueueItem * pQueueItem;
   size_t len;
   vdseTxStatus * txStatus;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, vdseQueueItem );
   txStatus = &pQueueItem->txStatus;

   len =  pQueueItem->dataLength + offsetof( vdseQueueItem, data );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. But it could be accessed by the current session,
    * for example during an iteration. To rollback we need to remove it 
    * from the linked list and free the memory.
    */
   if ( txStatus->usageCounter == 0 ) {
      vdseLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      vdseFree( &pQueue->memObject, 
                (unsigned char *) pQueueItem, 
                len, 
                pContext );

      pQueue->nodeObject.txCounter--;
   }
   else {
      vdseTxStatusCommitRemove( txStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueRollbackRemove( vdseQueue * pQueue, 
                              ptrdiff_t   itemOffset )
{
   vdseQueueItem * pQueueItem;
   
   VDS_PRE_CONDITION( pQueue     != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, vdseQueueItem );

   /*
    * This call resets the transaction (to "none"), decrement the
    * counter and remove the bit that flag this data as being in the
    * process of being removed.
    */
   vdseTxStatusUnmarkAsDestroyed(  &pQueueItem->txStatus );

   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseQueueStatus( vdseQueue    * pQueue,
                      vdsObjStatus * pStatus )
{
   vdseQueueItem * pQueueItem = NULL;
   enum ListErrors listErrCode;
   vdseLinkNode * pNode = NULL;
   vdseTxStatus  * txStatus;

   VDS_PRE_CONDITION( pQueue  != NULL );
   VDS_PRE_CONDITION( pStatus != NULL );
   
   GET_PTR( txStatus, pQueue->nodeObject.txStatusOffset, vdseTxStatus );

   pStatus->status = txStatus->enumStatus;
   pStatus->numDataItem = pQueue->listOfElements.currentSize;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;
   
   /* This call can only fail if the queue is empty. */
   listErrCode = vdseLinkedListPeakFirst( &pQueue->listOfElements, 
                                          &pNode );

   while ( listErrCode == LIST_OK ) {
      pQueueItem = (vdseQueueItem*)
         ((char*)pNode - offsetof( vdseQueueItem, node ));
      if ( pQueueItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pQueueItem->dataLength;
      }
      
      listErrCode =  vdseLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

