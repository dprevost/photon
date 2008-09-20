/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Nucleus/HashMap.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration of static functions */

static
void psonHashMapReleaseNoLock( psonHashMap        * pHashMap,
                               psonHashItem       * pHashItem,
                               psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashMapCommitAdd( psonHashMap        * pHashMap, 
                           ptrdiff_t            itemOffset,
                           psonSessionContext * pContext  )
{
   psonHashItem * pHashItem;
   psonTxStatus * txHashMapStatus;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psonHashItem );

   pHashItem->txStatus.txOffset = PSON_NULL_OFFSET;
   pHashItem->txStatus.status = PSON_TXS_OK;
   pHashMap->nodeObject.txCounter--;

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psonHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
   if ( (txHashMapStatus->usageCounter == 0) &&
                   (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) { 
         psonHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashMapCommitRemove( psonHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              psonSessionContext * pContext )
{
   psonHashItem * pHashItem;
   psonTxStatus * txItemStatus;
   psonTxStatus * txHashMapStatus;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psonHashItem );
   txItemStatus = &pHashItem->txStatus;
   /* 
    * If someone is using it, the usageCounter will be greater than zero.
    * If it zero, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( txItemStatus->usageCounter == 0 ) {
      psonHashDelWithItem( &pHashMap->hashObj, 
                           pHashItem,
                           pContext );
      pHashMap->nodeObject.txCounter--;

      /*
       * Do we need to resize? We need both conditions here:
       *
       *   - txHashMapStatus->usageCounter someone has a pointer to the data
       *
       *   - nodeObject.txCounter: offset to some of our data is part of a
       *                           transaction.
       *
       * Note: we do not check the return value of psonHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
         GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
         if ( (txHashMapStatus->usageCounter == 0) &&
            (pHashMap->nodeObject.txCounter == 0 ) ) {
            psonHashResize( &pHashMap->hashObj, pContext );
         }
      }
   }
   else {
      txItemStatus->status = PSON_TXS_DESTROYED_COMMITTED;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashMapDelete( psonHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength, 
                        psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   psonHashItem* pHashItem = NULL;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /*
       * The first step is to retrieve the item.
       */
      found = psonHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey,
                           keyLength,
                           &pHashItem,
                           &bucket,
                           pContext );
      if ( ! found ) {
         errcode = PSO_NO_SUCH_ITEM;
         goto the_exit;
      }
      while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
         GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashItem );
      }
      
      txItemStatus = &pHashItem->txStatus;

      /* 
       * If the transaction id of the item is non-zero, a big no-no - 
       * we do not support two transactions on the same data
       * (and if remove is committed - the data is "non-existent").
       */
      if ( txItemStatus->txOffset != PSON_NULL_OFFSET ) {
         if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_NO_SUCH_ITEM;
         }
         else {
            errcode = PSO_ITEM_IS_IN_USE;
         }
         goto the_exit;
      }

      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_REMOVE_DATA,
                         SET_OFFSET(pHashMap),
                         PSON_IDENT_HASH_MAP,
                         SET_OFFSET( pHashItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto the_exit;
      
      txItemStatus->txOffset = SET_OFFSET(pContext->pTransaction);
      txItemStatus->status = PSON_TXS_DESTROYED;
      pHashMap->nodeObject.txCounter++;

      psonUnlock( &pHashMap->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;
   
the_exit:

   psonUnlock( &pHashMap->memObject, pContext );
   /* psocSetError might have been already called by some other function */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashMapFini( psonHashMap        * pHashMap,
                      psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   psonHashFini( &pHashMap->hashObj );
   psonTreeNodeFini( &pHashMap->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   psonMemObjectFini(  &pHashMap->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashMapGet( psonHashMap        * pHashMap,
                     const void         * pKey,
                     size_t               keyLength, 
                     psonHashItem      ** ppHashItem,
                     size_t               bufferLength,
                     psonSessionContext * pContext )
{
   psonHashItem* pHashItem = NULL, * previousItem = NULL;
   psoErrors errcode;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pKey       != NULL );
   PSO_PRE_CONDITION( ppHashItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }

      found = psonHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey, 
                           keyLength,
                           &pHashItem,
                           &bucket,
                           pContext );
      if ( ! found ) {
         errcode = PSO_NO_SUCH_ITEM;
         goto the_exit;
      }
      while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
         previousItem = pHashItem;
         GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashItem );
      }

      /*
       * This test cannot be done in the API (before calling the current
       * function) since we do not know the item size. It could be done
       * after but this way makes the code faster.
       */
      if ( bufferLength < pHashItem->dataLength ) {
         errcode = PSO_INVALID_LENGTH;
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      /* 
       * If the transaction id of the item (to retrieve) is equal to the 
       * current transaction id AND the item is marked as deleted... error.
       *
       * If the transaction id of the item (to retrieve) is NOT equal to the 
       * current transaction id AND the item is added... error.
       *
       * If the object is replaced, the "right" item depends on the transaction
       * id.
       *
       * If the item is flagged as deleted and committed, it does not exists
       * from the API point of view.
       */
      if ( txItemStatus->txOffset != PSON_NULL_OFFSET ) {
         if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_NO_SUCH_ITEM;
            goto the_exit;
         }
         if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) &&
            txItemStatus->status & PSON_TXS_DESTROYED ) {
            errcode = PSO_ITEM_IS_DELETED;
            goto the_exit;
         }
         if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) &&
            txItemStatus->status & PSON_TXS_ADDED ) {
            errcode = PSO_ITEM_IS_IN_USE;
            goto the_exit;
         }
         if ( txItemStatus->status & PSON_TXS_REPLACED ) {
            if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) ) {
               pHashItem = previousItem;
               txItemStatus = &pHashItem->txStatus;
            }
         }
      }
      
      /*
       * We must increment the usage counter since we are passing back
       * a pointer to the data, not a copy. 
       */
      txItemStatus->usageCounter++;
      txHashMapStatus->usageCounter++;
      *ppHashItem = pHashItem;

      psonUnlock( &pHashMap->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psonUnlock( &pHashMap->memObject, pContext );
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

bool psonHashMapGetFirst( psonHashMap        * pHashMap,
                          psonHashMapItem    * pItem,
                          size_t               keyLength,
                          size_t               bufferLength,
                          psonSessionContext * pContext )
{
   psonHashItem* pHashItem = NULL;
   psonTxStatus * txItemStatus;
   psonTxStatus * txHashMapStatus;
   ptrdiff_t  firstItemOffset;
   bool isOK, found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      found = psonHashGetFirst( &pHashMap->hashObj, 
                                &firstItemOffset );
      while ( found ) {
         GET_PTR( pHashItem, firstItemOffset, psonHashItem );
         txItemStatus = &pHashItem->txStatus;

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
            if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSON_TXS_DESTROYED ) {
               isOK = false;
            }
            if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSON_TXS_ADDED ) {
               isOK = false;
            }
            if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
               isOK = false;
            }
         }
 
         if ( isOK ) {
            /*
             * These tests cannot be done in the API (before calling the 
             * current function) since we do not know the item size. They 
             * could be done after but this way makes the code faster.
             */
            if ( bufferLength < pHashItem->dataLength ) {
               psonUnlock( &pHashMap->memObject, pContext );
               psocSetError( &pContext->errorHandler, 
                             g_psoErrorHandle, PSO_INVALID_LENGTH );
               return false;
            }
            if ( keyLength < pHashItem->keyLength ) {
               psonUnlock( &pHashMap->memObject, pContext );
               psocSetError( &pContext->errorHandler, 
                             g_psoErrorHandle, PSO_INVALID_LENGTH );
               return false;
            }

            txItemStatus->usageCounter++;
            txHashMapStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = firstItemOffset;

            psonUnlock( &pHashMap->memObject, pContext );
            
            return true;
         }
  
         found = psonHashGetNext( &pHashMap->hashObj, 
                                  firstItemOffset,
                                  &firstItemOffset );
      }
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   psonUnlock( &pHashMap->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );

   return false;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashMapGetNext( psonHashMap        * pHashMap,
                         psonHashMapItem    * pItem,
                         size_t               keyLength,
                         size_t               bufferLength,
                         psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonHashItem * previousHashItem = NULL;
   psonTxStatus * txItemStatus;
   psonTxStatus * txHashMapStatus;
   ptrdiff_t  itemOffset;
   bool isOK, found;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );
   PSO_PRE_CONDITION( pItem->pHashItem  != NULL );
   PSO_PRE_CONDITION( pItem->itemOffset != PSON_NULL_OFFSET );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      found = psonHashGetNext( &pHashMap->hashObj, 
                                 itemOffset,
                                 &itemOffset );
      while ( found ) {
         GET_PTR( pHashItem, itemOffset, psonHashItem );
         txItemStatus = &pHashItem->txStatus;

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
            if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSON_TXS_DESTROYED ) {
               isOK = false;
            }
            if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSON_TXS_ADDED ) {
               isOK = false;
            }
            if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
               isOK = false;
            }
         }
 
         if ( isOK ) {
            /*
             * These tests cannot be done in the API (before calling the 
             * current function) since we do not know the item size. They 
             * could be done after but this way makes the code faster.
             */
            if ( bufferLength < pHashItem->dataLength ) {
               psonUnlock( &pHashMap->memObject, pContext );
               psocSetError( &pContext->errorHandler, 
                             g_psoErrorHandle, PSO_INVALID_LENGTH );
               return false;
            }
            if ( keyLength < pHashItem->keyLength ) {
               psonUnlock( &pHashMap->memObject, pContext );
               psocSetError( &pContext->errorHandler, 
                             g_psoErrorHandle, PSO_INVALID_LENGTH );
               return false;
            }

            txItemStatus->usageCounter++;
            txHashMapStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = itemOffset;
            psonHashMapReleaseNoLock( pHashMap, previousHashItem, pContext );

            psonUnlock( &pHashMap->memObject, pContext );
            
            return true;
         }
  
         found = psonHashGetNext( &pHashMap->hashObj, 
                                  itemOffset,
                                  &itemOffset );
      }
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   pItem->pHashItem = NULL;
   pItem->itemOffset = PSON_NULL_OFFSET;
   psonHashMapReleaseNoLock( pHashMap, previousHashItem, pContext );
   
   psonUnlock( &pHashMap->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashMapInit( psonHashMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfItems,
                      psonTxStatus        * pTxStatus,
                      size_t                origNameLength,
                      char                * origName,
                      ptrdiff_t             hashItemOffset,
                      psoObjectDefinition * pDefinition,
                      psonSessionContext  * pContext )
{
   psoErrors errcode;
   psonFieldDef * ptr;
   unsigned int i;
   
   PSO_PRE_CONDITION( pHashMap     != NULL );
   PSO_PRE_CONDITION( pContext     != NULL );
   PSO_PRE_CONDITION( pTxStatus    != NULL );
   PSO_PRE_CONDITION( origName     != NULL );
   PSO_PRE_CONDITION( pDefinition  != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks  > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   PSO_PRE_CONDITION( pDefinition->numFields > 0 );
   
   errcode = psonMemObjectInit( &pHashMap->memObject, 
                                PSON_IDENT_HASH_MAP,
                                &pHashMap->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pHashMap->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = psonHashInit( &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject),
                           expectedNumOfItems, 
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }
   
   pHashMap->numFields = (uint16_t) pDefinition->numFields;

   ptr = (psonFieldDef*) psonMalloc( &pHashMap->memObject, 
                                     pHashMap->numFields* sizeof(psonFieldDef),
                                     pContext );
   if ( ptr == NULL ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_NOT_ENOUGH_PSO_MEMORY );
      return false;
   }
   pHashMap->dataDefOffset = SET_OFFSET(ptr);

   for ( i = 0; i < pHashMap->numFields; ++i) {
      memcpy( ptr[i].name, pDefinition->fields[i].name, PSO_MAX_FIELD_LENGTH );
      ptr[i].type = pDefinition->fields[i].type;
      switch( ptr[i].type ) {
      case PSO_INTEGER:
      case PSO_BINARY:
      case PSO_STRING:
         ptr[i].length1 = pDefinition->fields[i].length;
         break;
      case PSO_DECIMAL:
         ptr[i].length1 = pDefinition->fields[i].precision;
         ptr[i].length2 = pDefinition->fields[i].scale;         
         break;
      case PSO_BOOLEAN:
         break;
      case PSO_VAR_BINARY:
      case PSO_VAR_STRING:
         ptr[i].length1 = pDefinition->fields[i].minLength;
         ptr[i].length2 = pDefinition->fields[i].maxLength;
         
      }
   }
   memcpy( &pHashMap->keyDef, &pDefinition->key, sizeof(psoKeyDefinition) );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashMapInsert( psonHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength, 
                        const void         * pData,
                        size_t               itemLength,
                        psonSessionContext * pContext )
{
   psonHashItem* pHashItem = NULL, * previousHashItem = NULL;
   psoErrors errcode = PSO_OK;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pData    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( itemLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      found = psonHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey, 
                           keyLength,
                           &previousHashItem,
                           &bucket,
                           pContext );
      if ( found ) {
         /* Find the last one in the chain of items with same key */
         while ( previousHashItem->nextSameKey != PSON_NULL_OFFSET ) {
            GET_PTR( previousHashItem, previousHashItem->nextSameKey, psonHashItem );
         }

         /* 
          * Anything othor than a deleted item committed means that the
          * key exists and that we cannot insert the item
          */
         txItemStatus = &previousHashItem->txStatus;
         if ( txItemStatus->status != PSON_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_ITEM_ALREADY_PRESENT;
            goto the_exit;
         }
      }
      
      errcode = psonHashInsertAt( &pHashMap->hashObj,
                                  bucket,
                                  (unsigned char *)pKey, 
                                  keyLength, 
                                  pData, 
                                  itemLength,
                                  &pHashItem,
                                  pContext );
      if ( errcode != PSO_OK ) goto the_exit;

      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_DATA,
                         SET_OFFSET(pHashMap),
                         PSON_IDENT_HASH_MAP,
                         SET_OFFSET(pHashItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonHashDelWithItem( &pHashMap->hashObj,
                              pHashItem,
                              pContext );
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      psonTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      pHashMap->nodeObject.txCounter++;
      txItemStatus->status = PSON_TXS_ADDED;
      if ( previousHashItem != NULL ) {
         previousHashItem->nextSameKey = SET_OFFSET(pHashItem);
      }
      
      psonUnlock( &pHashMap->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;

the_exit:

   psonUnlock( &pHashMap->memObject, pContext );
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

bool psonHashMapRelease( psonHashMap        * pHashMap,
                         psonHashItem       * pHashItem,
                         psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      psonHashMapReleaseNoLock( pHashMap,
                                pHashItem,
                                pContext );

      psonUnlock( &pHashMap->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * This version of Release is only called internally, by the current
 * object. Always use the standard one when calling from the API.
 */
static
void psonHashMapReleaseNoLock( psonHashMap        * pHashMap,
                               psonHashItem       * pHashItem,
                               psonSessionContext * pContext )
{
   psonTxStatus * txItemStatus, * txHashMapStatus;
   
   PSO_PRE_CONDITION( pHashMap  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   txItemStatus = &pHashItem->txStatus;
   
   txItemStatus->usageCounter--;
   txHashMapStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      /* Time to really delete the record! */
      psonHashDelWithItem( &pHashMap->hashObj, 
                           pHashItem,
                           pContext );
      pHashMap->nodeObject.txCounter--;
   }
   
   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psonHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
         psonHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonHashMapReplace( psonHashMap        * pHashMap,
                         const void         * pKey,
                         size_t               keyLength, 
                         const void         * pData,
                         size_t               itemLength,
                         psonSessionContext * pContext )
{
   psonHashItem * pHashItem, * pNewHashItem;
   psoErrors errcode = PSO_OK;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pData    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( itemLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psonTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psonTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = PSO_OBJECT_IS_DELETED;
         goto the_exit;
      }

      found = psonHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey, 
                           keyLength,
                           &pHashItem,
                           &bucket,
                           pContext );
      if ( ! found ) {
         errcode = PSO_NO_SUCH_ITEM;
         goto the_exit;
      }
      while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
         GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashItem );
      }

      txItemStatus = &pHashItem->txStatus;
      if ( txItemStatus->status != PSON_TXS_OK ) {
         errcode = PSO_ITEM_IS_IN_USE;
         goto the_exit;
      }
      
      errcode = psonHashInsertAt( &pHashMap->hashObj,
                                  bucket,
                                  (unsigned char *)pKey, 
                                  keyLength, 
                                  pData, 
                                  itemLength,
                                  &pNewHashItem,
                                  pContext );
      if ( errcode != PSO_OK ) goto the_exit;

      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_REMOVE_DATA,
                         SET_OFFSET(pHashMap),
                         PSON_IDENT_HASH_MAP,
                         SET_OFFSET(pHashItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonHashDelWithItem( &pHashMap->hashObj, 
                              pNewHashItem,
                              pContext );
         goto the_exit;
      }
      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_DATA,
                         SET_OFFSET(pHashMap),
                         PSON_IDENT_HASH_MAP,
                         SET_OFFSET(pNewHashItem),
                         0,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonHashDelWithItem( &pHashMap->hashObj, 
                              pNewHashItem,
                              pContext );
         psonTxRemoveLastOps( (psonTx*)pContext->pTransaction, pContext );
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      psonTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      txItemStatus->status = PSON_TXS_DESTROYED;

      txItemStatus = &pNewHashItem->txStatus;
      psonTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      txItemStatus->status = PSON_TXS_REPLACED;

      pHashItem->nextSameKey = SET_OFFSET(pNewHashItem);
      pHashMap->nodeObject.txCounter += 2;

      psonUnlock( &pHashMap->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;

the_exit:

   psonUnlock( &pHashMap->memObject, pContext );
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

void psonHashMapRollbackAdd( psonHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             psonSessionContext * pContext )
{
   psonHashItem * pHashItem;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psonHashItem );
   txItemStatus = &pHashItem->txStatus;
   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. But it could be accessed by the current session,
    * for example during an iteration. To rollback we need to remove it 
    * from the hash (this function will also free the memory back to 
    * the memory object).
    */
   if ( txItemStatus->usageCounter == 0 ) {
      psonHashDelWithItem( &pHashMap->hashObj, 
                           pHashItem,
                           pContext );
      pHashMap->nodeObject.txCounter--;
   
      /*
       * Do we need to resize? We need both conditions here:
       *
       *   - txHashMapStatus->usageCounter someone has a pointer to the data
       *
       *   - nodeObject.txCounter: offset to some of our data is part of a
       *                           transaction.
       *
       * Note: we do not check the return value of psonHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
      if ( (txHashMapStatus->usageCounter == 0) &&
                     (pHashMap->nodeObject.txCounter == 0 ) ) {
         if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
            psonHashResize( &pHashMap->hashObj, pContext );
         }
      }
   }
   else {
      psonTxStatusCommitRemove( txItemStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashMapRollbackRemove( psonHashMap        * pHashMap, 
                                ptrdiff_t            itemOffset,
                                psonSessionContext * pContext  )
{
   psonHashItem * pHashItem;
   psonTxStatus * txItemStatus, * txHashMapStatus;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( itemOffset != PSON_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psonHashItem );
   txItemStatus = &pHashItem->txStatus;

   /*
    * This call resets the transaction (to "none") and remove the 
    * bit that flag this data as being in the process of being removed.
    */
   psonTxStatusUnmarkAsDestroyed( txItemStatus );
   pHashMap->nodeObject.txCounter--;
   
   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psonHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
         psonHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonHashMapStatus( psonHashMap  * pHashMap,
                        psoObjStatus * pStatus )
{
   psonHashItem* pHashItem = NULL;
   ptrdiff_t  firstItemOffset;
   psonTxStatus  * txStatus;
   bool found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pStatus  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_HASH_MAP );
   
   GET_PTR( txStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   found = psonHashGetFirst( &pHashMap->hashObj, 
                               &firstItemOffset );
   while ( found ) {
      GET_PTR( pHashItem, firstItemOffset, psonHashItem );
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      found = psonHashGetNext( &pHashMap->hashObj, 
                               firstItemOffset,
                               &firstItemOffset );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

