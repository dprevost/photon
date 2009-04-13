/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
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
void psonQueueReleaseNoLock( psonQueue          * pQueue,
                             psonQueueItem      * pQueueItem,
                             psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueCommitAdd( psonQueue * pQueue, 
                         ptrdiff_t   itemOffset )
{
   psonQueueItem * pQueueItem;
   
   PSO_PRE_CONDITION( pQueue   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psonQueueItem );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. Clearing it is ok.
    */
   psonTxStatusClearTx( &pQueueItem->txStatus );
   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueCommitRemove( psonQueue          * pQueue, 
                            ptrdiff_t            itemOffset,
                            psonSessionContext * pContext )
{
   psonQueueItem * pQueueItem;
   size_t len;
   
   PSO_PRE_CONDITION( pQueue   != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psonQueueItem );

   /* 
    * If someone is using it, the usageCounter will be greater than one.
    * If it isn't, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( pQueueItem->txStatus.usageCounter == 0 ) {
      len =  pQueueItem->dataLength + offsetof( psonQueueItem, data );
      
      psonLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      psonFree( &pQueue->memObject, 
                (unsigned char * )pQueueItem,
                len,
                pContext );

      pQueue->nodeObject.txCounter--;
   }
   else {
      psonTxStatusCommitRemove( &pQueueItem->txStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueFini( psonQueue          * pQueue,
                    psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pQueue   != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );

   psonLinkedListFini( &pQueue->listOfElements );
   psonTreeNodeFini(   &pQueue->nodeObject );
   psonMemObjectFini(  &pQueue->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueGetFirst( psonQueue          * pQueue,
                        psonQueueItem     ** ppIterator,
                        uint32_t             bufferLength,
                        psonSessionContext * pContext )
{
   psonQueueItem* pQueueItem = NULL;
//   psonQueueItem* pOldItem = NULL;
   psoErrors errcode;
   psonLinkNode * pNode = NULL;
   psonTxStatus * txItemStatus, * txQueueStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   PSO_PRE_CONDITION( pQueue     != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pQueue->memObject, pContext ) ) {
      /* This call can only fail if the queue is empty. */
      okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
      
      while ( okList ) {
         pQueueItem = (psonQueueItem*)
            ((char*)pNode - offsetof( psonQueueItem, node ));
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
         if ( txItemStatus->txOffset != PSON_NULL_OFFSET ) {
            switch( txItemStatus->status ) {

            case PSON_TXS_DESTROYED_COMMITTED:
               isOK = false;
               break;
               
            case PSON_TXS_DESTROYED:
               if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
                  isOK = false;
                  queueIsEmpty = false;
               }
               break;
               
            case PSON_TXS_ADDED:
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
               psonUnlock( &pQueue->memObject, pContext );
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txQueueStatus->usageCounter++;
            *ppIterator = pQueueItem;
            
            psonUnlock( &pQueue->memObject, pContext );

            return true;
         }
         okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                           pNode, 
                                           &pNode );
      }
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   errcode = PSO_ITEM_IS_IN_USE;
   if ( queueIsEmpty ) {
      errcode = PSO_IS_EMPTY;   
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppIterator = NULL;
   
   psonUnlock( &pQueue->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueGetNext( psonQueue          * pQueue,
                       psonQueueItem     ** ppIterator,
                       uint32_t             bufferLength,
                       psonSessionContext * pContext )
{
   psonQueueItem* pQueueItem = NULL;
   psonQueueItem* pOldItem = NULL;
   psoErrors errcode;
   psonLinkNode * pNode = NULL;
   psonTxStatus * txItemStatus, * txQueueStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   PSO_PRE_CONDITION( pQueue     != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pQueue->memObject, pContext ) ) {
      errcode = PSO_REACHED_THE_END;
      pOldItem = (psonQueueItem*) *ppIterator;
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        &pOldItem->node, 
                                        &pNode );
      
      while ( okList ) {
         pQueueItem = (psonQueueItem*)
            ((char*)pNode - offsetof( psonQueueItem, node ));
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
         if ( txItemStatus->txOffset != PSON_NULL_OFFSET ) {
            switch( txItemStatus->status ) {

            case PSON_TXS_DESTROYED_COMMITTED:
               isOK = false;
               break;
               
            case PSON_TXS_DESTROYED:
               if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
                  isOK = false;
                  queueIsEmpty = false;
               }
               break;
               
            case PSON_TXS_ADDED:
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
               psonUnlock( &pQueue->memObject, pContext );
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txQueueStatus->usageCounter++;
            *ppIterator = pQueueItem;
   
            psonQueueReleaseNoLock( pQueue, pOldItem, pContext );
            
            psonUnlock( &pQueue->memObject, pContext );

            return true;
         }
         okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                           pNode, 
                                           &pNode );
      }
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   errcode = PSO_ITEM_IS_IN_USE;
   if ( queueIsEmpty ) {
      errcode = PSO_REACHED_THE_END;
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   *ppIterator = NULL;
   psonQueueReleaseNoLock( pQueue, pOldItem, pContext );
   
   psonUnlock( &pQueue->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueInit( psonQueue           * pQueue,
                    ptrdiff_t             parentOffset,
                    size_t                numberOfBlocks,
                    psonTxStatus        * pTxStatus,
                    uint32_t              origNameLength,
                    char                * origName,
                    ptrdiff_t             hashItemOffset,
                    psoObjectDefinition * pDefinition,
                    psonDataDefinition  * pDataDefinition,
                    psonSessionContext  * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pQueue          != NULL );
   PSO_PRE_CONDITION( pContext        != NULL );
   PSO_PRE_CONDITION( pTxStatus       != NULL );
   PSO_PRE_CONDITION( origName        != NULL );
   PSO_PRE_CONDITION( pDefinition     != NULL );
   PSO_PRE_CONDITION( pDataDefinition != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   
   errcode = psonMemObjectInit( &pQueue->memObject, 
                                PSON_IDENT_QUEUE,
                                &pQueue->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pQueue->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   psonLinkedListInit( &pQueue->listOfElements );

   pQueue->dataDefOffset = SET_OFFSET(pDataDefinition);
   pQueue->flags = pDefinition->flags;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueInsert( psonQueue          * pQueue,
                      const void         * pItem, 
                      uint32_t             length ,
                      psonDataDefinition * pDefinition,
                      enum psonQueueEnum   firstOrLast,
                      psonSessionContext * pContext )
{
   psonQueueItem* pQueueItem;
   psoErrors errcode = PSO_OK;
   psonTxStatus* txQueueStatus;
   size_t allocLength;
   bool ok;
   
   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( length  > 0 );
   PSO_PRE_CONDITION( firstOrLast == PSON_QUEUE_FIRST || 
      firstOrLast == PSON_QUEUE_LAST );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pQueue->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txQueueStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txQueueStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( psonQueueItem, data );
      pQueueItem = (psonQueueItem *) psonMalloc( &pQueue->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pQueueItem == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
   
      psonLinkNodeInit( &pQueueItem->node );
      pQueueItem->dataLength = length;
      memcpy( pQueueItem->data, pItem, length );
      if ( pDefinition == NULL ) {
         pQueueItem->dataDefOffset = PSON_NULL_OFFSET;
      }
      else {
         pQueueItem->dataDefOffset = SET_OFFSET(pDefinition);
      }
   
      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_DATA,
                         SET_OFFSET(pQueue),
                         PSON_IDENT_QUEUE,
                         SET_OFFSET(pQueueItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonFree( &pQueue->memObject, 
                   (unsigned char * )pQueueItem,
                   allocLength,
                   pContext );
         goto the_exit;
      }

      if ( firstOrLast == PSON_QUEUE_FIRST ) {
         psonLinkedListPutFirst( &pQueue->listOfElements,
                                 &pQueueItem->node );
      }
      else {
         psonLinkedListPutLast( &pQueue->listOfElements,
                                &pQueueItem->node );
      }
      
      psonTxStatusInit( &pQueueItem->txStatus, SET_OFFSET(pContext->pTransaction) );
      pQueue->nodeObject.txCounter++;
      pQueueItem->txStatus.status = PSON_TXS_ADDED;

      psonUnlock( &pQueue->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psonUnlock( &pQueue->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
    
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueInsertNow( psonQueue          * pQueue,
                         const void         * pItem, 
                         uint32_t             length ,
                         psonDataDefinition * pDefinition,
                         enum psonQueueEnum   firstOrLast,
                         psonSessionContext * pContext )
{
   psonQueueItem* pQueueItem;
   psoErrors errcode = PSO_OK;
   psonTxStatus* txQueueStatus;
   size_t allocLength;
   
   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( length  > 0 );
   PSO_PRE_CONDITION( firstOrLast == PSON_QUEUE_FIRST || 
      firstOrLast == PSON_QUEUE_LAST );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );

   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pQueue->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txQueueStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txQueueStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( psonQueueItem, data );
      pQueueItem = (psonQueueItem *) psonMalloc( &pQueue->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pQueueItem == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
   
      psonLinkNodeInit( &pQueueItem->node );
      pQueueItem->dataLength = length;
      memcpy( pQueueItem->data, pItem, length );
      if ( pDefinition == NULL ) {
         pQueueItem->dataDefOffset = PSON_NULL_OFFSET;
      }
      else {
         pQueueItem->dataDefOffset = SET_OFFSET(pDefinition);
      }
      
      if ( firstOrLast == PSON_QUEUE_FIRST ) {
         psonLinkedListPutFirst( &pQueue->listOfElements,
                                 &pQueueItem->node );
      }
      else {
         psonLinkedListPutLast( &pQueue->listOfElements,
                                &pQueueItem->node );
      }
      
      psonTxStatusInit( &pQueueItem->txStatus, PSON_NULL_OFFSET );

      psonUnlock( &pQueue->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psonUnlock( &pQueue->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
    
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueRelease( psonQueue          * pQueue,
                       psonQueueItem      * pQueueItem,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pQueueItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );

   if ( psonLock( &pQueue->memObject, pContext ) ) {
      psonQueueReleaseNoLock( pQueue,
                              pQueueItem,
                              pContext );
                             
      psonUnlock( &pQueue->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
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
void psonQueueReleaseNoLock( psonQueue          * pQueue,
                             psonQueueItem      * pQueueItem,
                             psonSessionContext * pContext )
{
   psonTxStatus * txItemStatus, * txQueueStatus;
   size_t len;
   
   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pQueueItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );

   txItemStatus = &pQueueItem->txStatus;
   GET_PTR( txQueueStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );
   
   txItemStatus->usageCounter--;
   txQueueStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      psonTxStatusIsRemoveCommitted(txItemStatus) ) {
      /* Time to really delete the record! */
      psonLinkedListRemoveItem( &pQueue->listOfElements, 
                                &pQueueItem->node );

      len =  pQueueItem->dataLength + offsetof( psonQueueItem, data );
      psonFree( &pQueue->memObject, 
                (unsigned char *) pQueueItem, 
                len, 
                pContext );

      pQueue->nodeObject.txCounter--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonQueueRemove( psonQueue          * pQueue,
                      psonQueueItem     ** ppQueueItem,
                      enum psonQueueEnum   firstOrLast,
                      uint32_t             bufferLength,
                      psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   psonQueueItem * pItem = NULL;
   psonTxStatus  * txParentStatus, * txItemStatus;
   psonLinkNode  * pNode = NULL;
   bool queueIsEmpty = true;
   bool okList, ok;
   
   PSO_PRE_CONDITION( pQueue      != NULL );
   PSO_PRE_CONDITION( ppQueueItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( firstOrLast == PSON_QUEUE_FIRST || 
      firstOrLast == PSON_QUEUE_LAST );
   PSO_PRE_CONDITION( pQueue->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txParentStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pQueue->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txParentStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txParentStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /* This call can only fail if the queue is empty. */
      if ( firstOrLast == PSON_QUEUE_FIRST ) {
         okList = psonLinkedListPeakFirst( &pQueue->listOfElements, &pNode );
      }
      else {
         okList = psonLinkedListPeakLast( &pQueue->listOfElements, &pNode );
      }
      
      while ( okList ) {
         pItem = (psonQueueItem *) ((char*)pNode - offsetof( psonQueueItem, node ));
         txItemStatus = &pItem->txStatus;
      
         /* 
          * If the transaction id of the item is non-zero, a big no-no - 
          * we do not support two transactions on the same data
          * (and if remove is committed - the data is "non-existent").
          */
         if ( txItemStatus->txOffset == PSON_NULL_OFFSET ) {
            /*
             * This test cannot be done in the API (before calling the current
             * function) since we do not know the item size. It could be done
             * after but this way makes the code faster.
             */
            if ( bufferLength < pItem->dataLength ) {
               errcode = PSO_INVALID_LENGTH;
               goto the_exit;
            }

            /* Add to current transaction  */
            ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                               PSON_TX_REMOVE_DATA,
                               SET_OFFSET( pQueue ),
                               PSON_IDENT_QUEUE,
                               SET_OFFSET( pItem ),
                               0, /* irrelevant */
                               pContext );
            PSO_POST_CONDITION( ok == true || ok == false );
            if ( ! ok ) goto the_exit;
      
            psonTxStatusSetTx( txItemStatus, SET_OFFSET(pContext->pTransaction) );
            pQueue->nodeObject.txCounter++;
            txItemStatus->usageCounter++;
            txParentStatus->usageCounter++;
            psonTxStatusMarkAsDestroyed( txItemStatus );

            *ppQueueItem = pItem;

            psonUnlock( &pQueue->memObject, pContext );

            return true;
         }
         /* We test the type of flag to return the proper error code (if needed) */
         if ( ! psonTxStatusIsRemoveCommitted(txItemStatus) ) {
            queueIsEmpty = false;
         }
         
         if ( firstOrLast == PSON_QUEUE_FIRST ) {
            okList = psonLinkedListPeakNext( &pQueue->listOfElements, 
                                             pNode, 
                                             &pNode );
         }
         else {
            okList = psonLinkedListPeakPrevious( &pQueue->listOfElements, 
                                                 pNode, 
                                                 &pNode );
         }
      }
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   /* Let this falls through the error handler */
   errcode = PSO_ITEM_IS_IN_USE;
   if ( queueIsEmpty ) errcode = PSO_IS_EMPTY;   
   
the_exit:

   psonUnlock( &pQueue->memObject, pContext );
   /* psocSetError might have been already called by some other function */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueRollbackAdd( psonQueue          * pQueue, 
                           ptrdiff_t            itemOffset,
                           psonSessionContext * pContext  )
{
   psonQueueItem * pQueueItem;
   size_t len;
   psonTxStatus * txStatus;
   
   PSO_PRE_CONDITION( pQueue   != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psonQueueItem );
   txStatus = &pQueueItem->txStatus;

   len =  pQueueItem->dataLength + offsetof( psonQueueItem, data );

   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. But it could be accessed by the current session,
    * for example during an iteration. To rollback we need to remove it 
    * from the linked list and free the memory.
    */
   if ( txStatus->usageCounter == 0 ) {
      psonLinkedListRemoveItem( &pQueue->listOfElements,
                                &pQueueItem->node );
      psonFree( &pQueue->memObject, 
                (unsigned char *) pQueueItem, 
                len, 
                pContext );

      pQueue->nodeObject.txCounter--;
   }
   else {
      psonTxStatusCommitRemove( txStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueRollbackRemove( psonQueue * pQueue, 
                              ptrdiff_t   itemOffset )
{
   psonQueueItem * pQueueItem;
   
   PSO_PRE_CONDITION( pQueue     != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pQueueItem, itemOffset, psonQueueItem );

   /*
    * This call resets the transaction (to "none") and remove the bit 
    * that flag this data as being in the process of being removed.
    */
   psonTxStatusUnmarkAsDestroyed(  &pQueueItem->txStatus );

   pQueue->nodeObject.txCounter--;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonQueueStatus( psonQueue    * pQueue,
                      psoObjStatus * pStatus )
{
   psonQueueItem * pQueueItem = NULL;
   psonLinkNode * pNode = NULL;
   psonTxStatus  * txStatus;
   bool okList;
   
   PSO_PRE_CONDITION( pQueue  != NULL );
   PSO_PRE_CONDITION( pStatus != NULL );
   
   GET_PTR( txStatus, pQueue->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pQueue->listOfElements.currentSize;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;
   
   /* This call can only fail if the queue is empty. */
   okList = psonLinkedListPeakFirst( &pQueue->listOfElements, 
                                     &pNode );

   while ( okList ) {
      pQueueItem = (psonQueueItem*)
         ((char*)pNode - offsetof( psonQueueItem, node ));
      if ( pQueueItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pQueueItem->dataLength;
      }
      
      okList =  psonLinkedListPeakNext( &pQueue->listOfElements, 
                                        pNode, 
                                        &pNode );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

