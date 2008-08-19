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

#include "Nucleus/Queue.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psnQueueReleaseNoLock( psnQueue          * pQueue,
                             psnQueueItem      * pQueueItem,
                             psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnQueueCommitAdd( psnQueue * pQueue, 
                         ptrdiff_t   itemOffset )
{
   psnQueueItem * pQueueItem;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psnQueueItem );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. Clearing it is ok.
    */
   psnTxStatusClearTx( &pQueueItem->txStatus );
   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnQueueCommitRemove( psnQueue          * pQueue, 
                            ptrdiff_t            itemOffset,
                            psnSessionContext * pContext )
{
   psnQueueItem * pQueueItem;
   size_t len;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psnQueueItem );

   /* 
    * If someone is using it, the usageCounter will be greater than one.
    * If it isn't, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( pQueueItem->txStatus.usageCounter == 0 ) {
      len =  pQueueItem->dataLength + offsetof( psnQueueItem, data );
      
      psnLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      psnFree( &pQueue->memObject, 
                (unsigned char * )pQueueItem,
                len,
                pContext );

      pQueue->nodeObject.txCounter--;
   }
   else {
      psnTxStatusCommitRemove( &pQueueItem->txStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnQueueFini( psnQueue          * pQueue,
                    psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );

   psnLinkedListFini( &pQueue->listOfElements );
   psnTreeNodeFini(   &pQueue->nodeObject );
   psnMemObjectFini(  &pQueue->memObject, PSN_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnQueueGet( psnQueue          * pQueue,
                   unsigned int         flag,
                   psnQueueItem     ** ppIterator,
                   size_t               bufferLength,
                   psnSessionContext * pContext )
{
   psnQueueItem* pQueueItem = NULL;
   psnQueueItem* pOldItem = NULL;
   vdsErrors errcode;
   psnLinkNode * pNode = NULL;
   psnTxStatus * txItemStatus, * txQueueStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   VDS_PRE_CONDITION( pQueue     != NULL );
   VDS_PRE_CONDITION( ppIterator != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( flag == VDS_FIRST || flag == VDS_NEXT );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );
   
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );
   
   if ( psnLock( &pQueue->memObject, pContext ) ) {
      if ( flag == VDS_NEXT ) {
         
         errcode = VDS_REACHED_THE_END;
         pOldItem = (psnQueueItem*) *ppIterator;
         okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                           &pOldItem->node, 
                                           &pNode );
      }
      else {
         /* This call can only fail if the queue is empty. */
         okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
      }
      
      while ( okList ) {
         pQueueItem = (psnQueueItem*)
            ((char*)pNode - offsetof( psnQueueItem, node ));
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
         if ( txItemStatus->txOffset != PSN_NULL_OFFSET ) {
            switch( txItemStatus->status ) {

            case PSN_TXS_DESTROYED_COMMITTED:
               isOK = false;
               break;
               
            case PSN_TXS_DESTROYED:
               if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
                  isOK = false;
                  queueIsEmpty = false;
               }
               break;
               
            case PSN_TXS_ADDED:
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
               psnUnlock( &pQueue->memObject, pContext );
               pscSetError( &pContext->errorHandler,
                             g_vdsErrorHandle,
                             VDS_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txQueueStatus->usageCounter++;
            *ppIterator = pQueueItem;
            if ( flag == VDS_NEXT ) {
               psnQueueReleaseNoLock( pQueue, pOldItem, pContext );
            }
            
            psnUnlock( &pQueue->memObject, pContext );

            return true;
         }
         okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                           pNode, 
                                           &pNode );
      }
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
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
      psnQueueReleaseNoLock( pQueue, pOldItem, pContext );
   }
   
   psnUnlock( &pQueue->memObject, pContext );
   pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnQueueInit( psnQueue           * pQueue,
                    ptrdiff_t             parentOffset,
                    size_t                numberOfBlocks,
                    psnTxStatus        * pTxStatus,
                    size_t                origNameLength,
                    char                * origName,
                    ptrdiff_t             hashItemOffset,
                    vdsObjectDefinition * pDefinition,
                    psnSessionContext  * pContext )
{
   vdsErrors errcode;
   psnFieldDef * ptr;
   unsigned int i;
   
   VDS_PRE_CONDITION( pQueue       != NULL );
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( pTxStatus    != NULL );
   VDS_PRE_CONDITION( origName     != NULL );
   VDS_PRE_CONDITION( pDefinition  != NULL );
   VDS_PRE_CONDITION( hashItemOffset != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( parentOffset   != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   VDS_PRE_CONDITION( pDefinition->numFields > 0 );
   
   errcode = psnMemObjectInit( &pQueue->memObject, 
                                PSN_IDENT_QUEUE,
                                &pQueue->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return false;
   }

   psnTreeNodeInit( &pQueue->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   psnLinkedListInit( &pQueue->listOfElements );

   pQueue->numFields = (uint16_t) pDefinition->numFields;

   ptr = (psnFieldDef*) psnMalloc( &pQueue->memObject, 
                                     pQueue->numFields* sizeof(psnFieldDef),
                                     pContext );
   if ( ptr == NULL ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      return false;
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

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnQueueInsert( psnQueue          * pQueue,
                      const void         * pItem, 
                      size_t               length ,
                      enum psnQueueEnum   firstOrLast,
                      psnSessionContext * pContext )
{
   psnQueueItem* pQueueItem;
   vdsErrors errcode = VDS_OK;
   psnTxStatus* txQueueStatus;
   size_t allocLength;
   bool ok;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( length  > 0 );
   VDS_PRE_CONDITION( firstOrLast == PSN_QUEUE_FIRST || 
      firstOrLast == PSN_QUEUE_LAST );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pQueue->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txQueueStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txQueueStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( psnQueueItem, data );
      pQueueItem = (psnQueueItem *) psnMalloc( &pQueue->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pQueueItem == NULL ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
   
      psnLinkNodeInit( &pQueueItem->node );
      pQueueItem->dataLength = length;
      memcpy( pQueueItem->data, pItem, length );
   
      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_ADD_DATA,
                         SET_OFFSET(pQueue),
                         PSN_IDENT_QUEUE,
                         SET_OFFSET(pQueueItem),
                         0,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psnFree( &pQueue->memObject, 
                   (unsigned char * )pQueueItem,
                   allocLength,
                   pContext );
         goto the_exit;
      }

      if ( firstOrLast == PSN_QUEUE_FIRST ) {
         psnLinkedListPutFirst( &pQueue->listOfElements,
                                 &pQueueItem->node );
      }
      else {
         psnLinkedListPutLast( &pQueue->listOfElements,
                                &pQueueItem->node );
      }
      
      psnTxStatusInit( &pQueueItem->txStatus, SET_OFFSET(pContext->pTransaction) );
      pQueue->nodeObject.txCounter++;
      pQueueItem->txStatus.status = PSN_TXS_ADDED;

      psnUnlock( &pQueue->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psnUnlock( &pQueue->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
    
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnQueueInsertNow( psnQueue          * pQueue,
                         const void         * pItem, 
                         size_t               length ,
                         enum psnQueueEnum   firstOrLast,
                         psnSessionContext * pContext )
{
   psnQueueItem* pQueueItem;
   vdsErrors errcode = VDS_OK;
   psnTxStatus* txQueueStatus;
   size_t allocLength;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( length  > 0 );
   VDS_PRE_CONDITION( firstOrLast == PSN_QUEUE_FIRST || 
      firstOrLast == PSN_QUEUE_LAST );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pQueue->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txQueueStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txQueueStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( psnQueueItem, data );
      pQueueItem = (psnQueueItem *) psnMalloc( &pQueue->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pQueueItem == NULL ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
   
      psnLinkNodeInit( &pQueueItem->node );
      pQueueItem->dataLength = length;
      memcpy( pQueueItem->data, pItem, length );
   
      if ( firstOrLast == PSN_QUEUE_FIRST ) {
         psnLinkedListPutFirst( &pQueue->listOfElements,
                                 &pQueueItem->node );
      }
      else {
         psnLinkedListPutLast( &pQueue->listOfElements,
                                &pQueueItem->node );
      }
      
      psnTxStatusInit( &pQueueItem->txStatus, PSN_NULL_OFFSET );

      psnUnlock( &pQueue->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psnUnlock( &pQueue->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
    
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnQueueRelease( psnQueue          * pQueue,
                       psnQueueItem      * pQueueItem,
                       psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pQueueItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );

   if ( psnLock( &pQueue->memObject, pContext ) ) {
      psnQueueReleaseNoLock( pQueue,
                              pQueueItem,
                              pContext );
                             
      psnUnlock( &pQueue->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This version of the function is to be used when:
 *  - you know that the object is in use and should not/cannot be removed
 *  - the calling function holds the lock
 */
static
void psnQueueReleaseNoLock( psnQueue          * pQueue,
                             psnQueueItem      * pQueueItem,
                             psnSessionContext * pContext )
{
   psnTxStatus * txItemStatus, * txQueueStatus;
   size_t len;
   
   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pQueueItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );

   txItemStatus = &pQueueItem->txStatus;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );
   
   txItemStatus->usageCounter--;
   txQueueStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      psnTxStatusIsRemoveCommitted(txItemStatus) ) {
      /* Time to really delete the record! */
      psnLinkedListRemoveItem( &pQueue->listOfElements, 
                                &pQueueItem->node );

      len =  pQueueItem->dataLength + offsetof( psnQueueItem, data );
      psnFree( &pQueue->memObject, 
                (unsigned char *) pQueueItem, 
                len, 
                pContext );

      pQueue->nodeObject.txCounter--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnQueueRemove( psnQueue          * pQueue,
                      psnQueueItem     ** ppQueueItem,
                      enum psnQueueEnum   firstOrLast,
                      size_t               bufferLength,
                      psnSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   psnQueueItem * pItem = NULL;
   psnTxStatus  * txParentStatus, * txItemStatus;
   psnLinkNode  * pNode = NULL;
   bool queueIsEmpty = true;
   bool okList, ok;
   
   VDS_PRE_CONDITION( pQueue      != NULL );
   VDS_PRE_CONDITION( ppQueueItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( firstOrLast == PSN_QUEUE_FIRST || 
      firstOrLast == PSN_QUEUE_LAST );
   VDS_PRE_CONDITION( pQueue->memObject.objType == PSN_IDENT_QUEUE );
   
   GET_PTR( txParentStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pQueue->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txParentStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txParentStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /* This call can only fail if the queue is empty. */
      if ( firstOrLast == PSN_QUEUE_FIRST ) {
         okList = psnLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
      }
      else {
         okList = psnLinkedListPeakLast( &pQueue->listOfElements, &pNode );
      }
      
      while ( okList ) {
         pItem = (psnQueueItem *) ((char*)pNode - offsetof( psnQueueItem, node ));
         txItemStatus = &pItem->txStatus;
      
         /* 
          * If the transaction id of the item is non-zero, a big no-no - 
          * we do not support two transactions on the same data
          * (and if remove is committed - the data is "non-existent").
          */
         if ( txItemStatus->txOffset == PSN_NULL_OFFSET ) {
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
            ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                               PSN_TX_REMOVE_DATA,
                               SET_OFFSET( pQueue ),
                               PSN_IDENT_QUEUE,
                               SET_OFFSET( pItem ),
                               0, /* irrelevant */
                               pContext );
            VDS_POST_CONDITION( ok == true || ok == false );
            if ( ! ok ) goto the_exit;
      
            psnTxStatusSetTx( txItemStatus, SET_OFFSET(pContext->pTransaction) );
            pQueue->nodeObject.txCounter++;
            txItemStatus->usageCounter++;
            txParentStatus->usageCounter++;
            psnTxStatusMarkAsDestroyed( txItemStatus );

            *ppQueueItem = pItem;

            psnUnlock( &pQueue->memObject, pContext );

            return true;
         }
         /* We test the type of flag to return the proper error code (if needed) */
         if ( ! psnTxStatusIsRemoveCommitted(txItemStatus) ) {
            queueIsEmpty = false;
         }
         
         if ( firstOrLast == PSN_QUEUE_FIRST ) {
            okList = psnLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
         }
         else {
            okList = psnLinkedListPeakPrevious( &pQueue->listOfElements, 
                                                 pNode, 
                                                 &pNode );
         }
      }
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   /* Let this falls through the error handler */
   errcode = VDS_ITEM_IS_IN_USE;
   if ( queueIsEmpty ) errcode = VDS_IS_EMPTY;   
   
the_exit:

   psnUnlock( &pQueue->memObject, pContext );
   /* pscSetError might have been already called by some other function */
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnQueueRollbackAdd( psnQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           psnSessionContext * pContext  )
{
   psnQueueItem * pQueueItem;
   size_t len;
   psnTxStatus * txStatus;
   
   VDS_PRE_CONDITION( pQueue   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psnQueueItem );
   txStatus = &pQueueItem->txStatus;

   len =  pQueueItem->dataLength + offsetof( psnQueueItem, data );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. But it could be accessed by the current session,
    * for example during an iteration. To rollback we need to remove it 
    * from the linked list and free the memory.
    */
   if ( txStatus->usageCounter == 0 ) {
      psnLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      psnFree( &pQueue->memObject, 
                (unsigned char *) pQueueItem, 
                len, 
                pContext );

      pQueue->nodeObject.txCounter--;
   }
   else {
      psnTxStatusCommitRemove( txStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnQueueRollbackRemove( psnQueue * pQueue, 
                              ptrdiff_t   itemOffset )
{
   psnQueueItem * pQueueItem;
   
   VDS_PRE_CONDITION( pQueue     != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psnQueueItem );

   /*
    * This call resets the transaction (to "none") and remove the bit 
    * that flag this data as being in the process of being removed.
    */
   psnTxStatusUnmarkAsDestroyed(  &pQueueItem->txStatus );

   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnQueueStatus( psnQueue    * pQueue,
                      vdsObjStatus * pStatus )
{
   psnQueueItem * pQueueItem = NULL;
   psnLinkNode * pNode = NULL;
   psnTxStatus  * txStatus;
   bool okList;
   
   VDS_PRE_CONDITION( pQueue  != NULL );
   VDS_PRE_CONDITION( pStatus != NULL );
   
   GET_PTR( txStatus, pQueue->nodeObject.txStatusOffset, psnTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pQueue->listOfElements.currentSize;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;
   
   /* This call can only fail if the queue is empty. */
   okList = psnLinkedListPeakFirst( &pQueue->listOfElements, 
                                     &pNode );

   while ( okList ) {
      pQueueItem = (psnQueueItem*)
         ((char*)pNode - offsetof( psnQueueItem, node ));
      if ( pQueueItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pQueueItem->dataLength;
      }
      
      okList =  psnLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

