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

#include "Nucleus/Set.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psonSetReleaseNoLock( psonSet          * pSet,
                             psonSetItem      * pSetItem,
                             psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonSetFini( psonSet            * pSet,
                  psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pSet   != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pSet->memObject.objType == PSON_IDENT_QUEUE );

   psonLinkedListFini( &pSet->listOfElements );
   psonTreeNodeFini(   &pSet->nodeObject );
   psonMemObjectFini(  &pSet->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSetGetFirst( psonSet            * pSet,
                      psonSetItem       ** ppIterator,
                      uint32_t             bufferLength,
                      psonSessionContext * pContext )
{
   psonSetItem* pSetItem = NULL;
   psoErrors errcode;
   psonLinkNode * pNode = NULL;
   psonTxStatus * txItemStatus, * txSetStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   PSO_PRE_CONDITION( pSet     != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pSet->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txSetStatus, pSet->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pSet->memObject, pContext ) ) {
      /* This call can only fail if the queue is empty. */
      okList = psonLinkedListPeakFirst( &pSet->listOfElements, &pNode );
      
      while ( okList ) {
         pSetItem = (psonSetItem*)
            ((char*)pNode - offsetof( psonSetItem, node ));
         txItemStatus = &pSetItem->txStatus;
         
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
            if ( bufferLength < pSetItem->dataLength ) {
               psonUnlock( &pSet->memObject, pContext );
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txSetStatus->usageCounter++;
            *ppIterator = pSetItem;
            
            psonUnlock( &pSet->memObject, pContext );

            return true;
         }
         okList =  psonLinkedListPeakNext( &pSet->listOfElements, 
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
   
   psonUnlock( &pSet->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSetGetNext( psonSet            * pSet,
                     psonSetItem       ** ppIterator,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext )
{
   psonSetItem* pSetItem = NULL;
   psonSetItem* pOldItem = NULL;
   psoErrors errcode;
   psonLinkNode * pNode = NULL;
   psonTxStatus * txItemStatus, * txSetStatus;
   bool isOK, okList;
   bool queueIsEmpty = true;
   
   PSO_PRE_CONDITION( pSet     != NULL );
   PSO_PRE_CONDITION( ppIterator != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pSet->memObject.objType == PSON_IDENT_QUEUE );
   
   GET_PTR( txSetStatus, pSet->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pSet->memObject, pContext ) ) {
         
      errcode = PSO_REACHED_THE_END;
      pOldItem = (psonSetItem*) *ppIterator;
      okList =  psonLinkedListPeakNext( &pSet->listOfElements, 
                                        &pOldItem->node, 
                                        &pNode );
      
      while ( okList ) {
         pSetItem = (psonSetItem*)
            ((char*)pNode - offsetof( psonSetItem, node ));
         txItemStatus = &pSetItem->txStatus;
         
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
            if ( bufferLength < pSetItem->dataLength ) {
               psonUnlock( &pSet->memObject, pContext );
               psocSetError( &pContext->errorHandler,
                             g_psoErrorHandle,
                             PSO_INVALID_LENGTH );
                return false;
            }

            txItemStatus->usageCounter++;
            txSetStatus->usageCounter++;
            *ppIterator = pSetItem;
            psonSetReleaseNoLock( pSet, pOldItem, pContext );
            
            psonUnlock( &pSet->memObject, pContext );

            return true;
         }
         okList =  psonLinkedListPeakNext( &pSet->listOfElements, 
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
   psonSetReleaseNoLock( pSet, pOldItem, pContext );
   
   psonUnlock( &pSet->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSetInit( psonSet           * pSet,
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
   
   PSO_PRE_CONDITION( pSet          != NULL );
   PSO_PRE_CONDITION( pContext        != NULL );
   PSO_PRE_CONDITION( pTxStatus       != NULL );
   PSO_PRE_CONDITION( origName        != NULL );
   PSO_PRE_CONDITION( pDefinition     != NULL );
   PSO_PRE_CONDITION( pDataDefinition != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   
   errcode = psonMemObjectInit( &pSet->memObject, 
                                PSON_IDENT_QUEUE,
                                &pSet->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pSet->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   psonLinkedListInit( &pSet->listOfElements );

   pSet->dataDefOffset = SET_OFFSET(pDataDefinition);
   pSet->flags = pDefinition->flags;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSetInsert( psonSet          * pSet,
                    const void         * pItem, 
                    uint32_t             length ,
                    psonDataDefinition * pDefinition,
                    psonSessionContext * pContext )
{
   psonSetItem* pSetItem;
   psoErrors errcode = PSO_OK;
   psonTxStatus* txSetStatus;
   size_t allocLength;
   bool ok;
   
   PSO_PRE_CONDITION( pSet != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( length  > 0 );
   PSO_PRE_CONDITION( pSet->memObject.objType == PSON_IDENT_QUEUE );

   GET_PTR( txSetStatus, pSet->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pSet->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txSetStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txSetStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      allocLength = length + offsetof( psonSetItem, data );
      pSetItem = (psonSetItem *) psonMalloc( &pSet->memObject,
                                                 allocLength,
                                                 pContext );
      if ( pSetItem == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
   
      psonLinkNodeInit( &pSetItem->node );
      pSetItem->dataLength = length;
      memcpy( pSetItem->data, pItem, length );
      if ( pDefinition == NULL ) {
         pSetItem->dataDefOffset = PSON_NULL_OFFSET;
      }
      else {
         pSetItem->dataDefOffset = SET_OFFSET(pDefinition);
      }
   
      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_DATA,
                         SET_OFFSET(pSet),
                         PSON_IDENT_QUEUE,
                         SET_OFFSET(pSetItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonFree( &pSet->memObject, 
                   (unsigned char * )pSetItem,
                   allocLength,
                   pContext );
         goto the_exit;
      }

      psonLinkedListPutLast( &pSet->listOfElements,
                             &pSetItem->node );
      
      psonTxStatusInit( &pSetItem->txStatus, SET_OFFSET(pContext->pTransaction) );
      pSet->nodeObject.txCounter++;
      pSetItem->txStatus.status = PSON_TXS_ADDED;

      psonUnlock( &pSet->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psonUnlock( &pSet->memObject, pContext );
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

bool psonSetRelease( psonSet          * pSet,
                       psonSetItem      * pSetItem,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pSet != NULL );
   PSO_PRE_CONDITION( pSetItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pSet->memObject.objType == PSON_IDENT_QUEUE );

   if ( psonLock( &pSet->memObject, pContext ) ) {
      psonSetReleaseNoLock( pSet,
                              pSetItem,
                              pContext );
                             
      psonUnlock( &pSet->memObject, pContext );
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
void psonSetReleaseNoLock( psonSet          * pSet,
                             psonSetItem      * pSetItem,
                             psonSessionContext * pContext )
{
   psonTxStatus * txItemStatus, * txSetStatus;
   size_t len;
   
   PSO_PRE_CONDITION( pSet != NULL );
   PSO_PRE_CONDITION( pSetItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( pSet->memObject.objType == PSON_IDENT_QUEUE );

   txItemStatus = &pSetItem->txStatus;
   GET_PTR( txSetStatus, pSet->nodeObject.txStatusOffset, psonTxStatus );
   
   txItemStatus->usageCounter--;
   txSetStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      psonTxStatusIsRemoveCommitted(txItemStatus) ) {
      /* Time to really delete the record! */
      psonLinkedListRemoveItem( &pSet->listOfElements, 
                                &pSetItem->node );

      len =  pSetItem->dataLength + offsetof( psonSetItem, data );
      psonFree( &pSet->memObject, 
                (unsigned char *) pSetItem, 
                len, 
                pContext );

      pSet->nodeObject.txCounter--;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonSetStatus( psonSet    * pSet,
                      psoObjStatus * pStatus )
{
   psonSetItem * pSetItem = NULL;
   psonLinkNode * pNode = NULL;
   psonTxStatus  * txStatus;
   bool okList;
   
   PSO_PRE_CONDITION( pSet  != NULL );
   PSO_PRE_CONDITION( pStatus != NULL );
   
   GET_PTR( txStatus, pSet->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pSet->listOfElements.currentSize;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;
   
   /* This call can only fail if the queue is empty. */
   okList = psonLinkedListPeakFirst( &pSet->listOfElements, 
                                     &pNode );

   while ( okList ) {
      pSetItem = (psonSetItem*)
         ((char*)pNode - offsetof( psonSetItem, node ));
      if ( pSetItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pSetItem->dataLength;
      }
      
      okList =  psonLinkedListPeakNext( &pSet->listOfElements, 
                                        pNode, 
                                        &pNode );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

