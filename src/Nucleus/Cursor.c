/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/Cursor.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psonCursorReleaseNoLock( psonCursor          * pCursor,
                             psonCursorItem      * pCursorItem,
                             psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorFini( psonCursor            * pCursor,
                  psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pCursor   != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_QUEUE );

   psonLinkedListFini( &pCursor->listOfElements );
   psonTreeNodeFini(   &pCursor->nodeObject );
   psonMemObjectFini(  &pCursor->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetFirst( psonCursor            * pCursor,
                      psonCursorItem       ** ppIterator,
                      uint32_t             bufferLength,
                      psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem = NULL;
   psoErrors errcode;
   psonLinkNode * pNode = NULL;
   psonTxStatus * txItemStatus, * txCursorStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   PSO_PRE_CONDITION( pCursor     != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txCursorStatus, pCursor->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pCursor->memObject, pContext ) ) {
      /* This call can only fail if the queue is empty. */
      okList = psonLinkedListPeakFirst( &pCursor->listOfElements, &pNode );
      
      while ( okList ) {
         pCursorItem = (psonCursorItem*)
            ((char*)pNode - offsetof( psonCursorItem, node ));
         txItemStatus = &pCursorItem->txStatus;
         
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
            if ( bufferLength < pCursorItem->dataLength ) {
               psonUnlock( &pCursor->memObject, pContext );
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txCursorStatus->usageCounter++;
            *ppIterator = pCursorItem;
            
            psonUnlock( &pCursor->memObject, pContext );

            return true;
         }
         okList =  psonLinkedListPeakNext( &pCursor->listOfElements, 
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
   
   psonUnlock( &pCursor->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorGetNext( psonCursor            * pCursor,
                     psonCursorItem       ** ppIterator,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem = NULL;
   psonCursorItem* pOldItem = NULL;
   psoErrors errcode;
   psonLinkNode * pNode = NULL;
   psonTxStatus * txItemStatus, * txCursorStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   PSO_PRE_CONDITION( pCursor     != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txCursorStatus, pCursor->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pCursor->memObject, pContext ) ) {
         
      errcode = PSO_REACHED_THE_END;
      pOldItem = (psonCursorItem*) *ppIterator;
      okList =  psonLinkedListPeakNext( &pCursor->listOfElements, 
                                        &pOldItem->node, 
                                        &pNode );
      
      while ( okList ) {
         pCursorItem = (psonCursorItem*)
            ((char*)pNode - offsetof( psonCursorItem, node ));
         txItemStatus = &pCursorItem->txStatus;
         
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
            if ( bufferLength < pCursorItem->dataLength ) {
               psonUnlock( &pCursor->memObject, pContext );
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txCursorStatus->usageCounter++;
            *ppIterator = pCursorItem;
            psonCursorReleaseNoLock( pCursor, pOldItem, pContext );
            
            psonUnlock( &pCursor->memObject, pContext );

            return true;
         }
         okList =  psonLinkedListPeakNext( &pCursor->listOfElements, 
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
   psonCursorReleaseNoLock( pCursor, pOldItem, pContext );
   
   psonUnlock( &pCursor->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInit( psonCursor           * pCursor,
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
   
   PSO_PRE_CONDITION( pCursor          != NULL );
   PSO_PRE_CONDITION( pContext        != NULL );
   PSO_PRE_CONDITION( pTxStatus       != NULL );
   PSO_PRE_CONDITION( origName        != NULL );
   PSO_PRE_CONDITION( pDefinition     != NULL );
   PSO_PRE_CONDITION( pDataDefinition != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   
   errcode = psonMemObjectInit( &pCursor->memObject, 
                                PSON_IDENT_QUEUE,
                                &pCursor->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pCursor->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   psonLinkedListInit( &pCursor->listOfElements );

   pCursor->dataDefOffset = SET_OFFSET(pDataDefinition);
   pCursor->flags = pDefinition->flags;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonCursorInsert( psonCursor          * pCursor,
                    const void         * pItem, 
                    uint32_t             length ,
                    psonDataDefinition * pDefinition,
                    psonSessionContext * pContext )
{
   psonCursorItem* pCursorItem;
   psoErrors errcode = PSO_OK;
   psonTxStatus* txCursorStatus;
   size_t allocLength;
   bool ok;
   
   PSO_PRE_CONDITION( pCursor != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( length  > 0 );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_QUEUE );

   GET_PTR( txCursorStatus, pCursor->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pCursor->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txCursorStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txCursorStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( psonCursorItem, data );
      pCursorItem = (psonCursorItem *) psonMalloc( &pCursor->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pCursorItem == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
   
      psonLinkNodeInit( &pCursorItem->node );
      pCursorItem->dataLength = length;
      memcpy( pCursorItem->data, pItem, length );
      if ( pDefinition == NULL ) {
         pCursorItem->dataDefOffset = PSON_NULL_OFFSET;
      }
      else {
         pCursorItem->dataDefOffset = SET_OFFSET(pDefinition);
      }
   
      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_DATA,
                         SET_OFFSET(pCursor),
                         PSON_IDENT_QUEUE,
                         SET_OFFSET(pCursorItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonFree( &pCursor->memObject, 
                   (unsigned char * )pCursorItem,
                   allocLength,
                   pContext );
         goto the_exit;
      }

      psonLinkedListPutLast( &pCursor->listOfElements,
                             &pCursorItem->node );
      
      psonTxStatusInit( &pCursorItem->txStatus, SET_OFFSET(pContext->pTransaction) );
      pCursor->nodeObject.txCounter++;
      pCursorItem->txStatus.status = PSON_TXS_ADDED;

      psonUnlock( &pCursor->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psonUnlock( &pCursor->memObject, pContext );
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

bool psonCursorRelease( psonCursor          * pCursor,
                       psonCursorItem      * pCursorItem,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pCursor != NULL );
   PSO_PRE_CONDITION( pCursorItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_QUEUE );

   if ( psonLock( &pCursor->memObject, pContext ) ) {
      psonCursorReleaseNoLock( pCursor,
                              pCursorItem,
                              pContext );
                             
      psonUnlock( &pCursor->memObject, pContext );
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
void psonCursorReleaseNoLock( psonCursor          * pCursor,
                             psonCursorItem      * pCursorItem,
                             psonSessionContext * pContext )
{
   psonTxStatus * txItemStatus, * txCursorStatus;
   size_t len;
   
   PSO_PRE_CONDITION( pCursor != NULL );
   PSO_PRE_CONDITION( pCursorItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pCursor->memObject.objType == PSON_IDENT_QUEUE );

   txItemStatus = &pCursorItem->txStatus;
   GET_PTR( txCursorStatus, pCursor->nodeObject.txStatusOffset, psonTxStatus );
   
   txItemStatus->usageCounter--;
   txCursorStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      psonTxStatusIsRemoveCommitted(txItemStatus) ) {
      /* Time to really delete the record! */
      psonLinkedListRemoveItem( &pCursor->listOfElements, 
                                &pCursorItem->node );

      len =  pCursorItem->dataLength + offsetof( psonCursorItem, data );
      psonFree( &pCursor->memObject, 
                (unsigned char *) pCursorItem, 
                len, 
                pContext );

      pCursor->nodeObject.txCounter--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonCursorStatus( psonCursor    * pCursor,
                      psoObjStatus * pStatus )
{
   psonCursorItem * pCursorItem = NULL;
   psonLinkNode * pNode = NULL;
   psonTxStatus  * txStatus;
   bool okList;
   
   PSO_PRE_CONDITION( pCursor  != NULL );
   PSO_PRE_CONDITION( pStatus != NULL );
   
   GET_PTR( txStatus, pCursor->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pCursor->listOfElements.currentSize;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;
   
   /* This call can only fail if the queue is empty. */
   okList = psonLinkedListPeakFirst( &pCursor->listOfElements, 
                                     &pNode );

   while ( okList ) {
      pCursorItem = (psonCursorItem*)
         ((char*)pNode - offsetof( psonCursorItem, node ));
      if ( pCursorItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pCursorItem->dataLength;
      }
      
      okList =  psonLinkedListPeakNext( &pCursor->listOfElements, 
                                        pNode, 
                                        &pNode );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

