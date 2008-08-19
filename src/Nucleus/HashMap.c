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

#include "Nucleus/HashMap.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration of static functions */

static
void psnHashMapReleaseNoLock( psnHashMap        * pHashMap,
                               psnHashItem       * pHashItem,
                               psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashMapCommitAdd( psnHashMap        * pHashMap, 
                           ptrdiff_t            itemOffset,
                           psnSessionContext * pContext  )
{
   psnHashItem * pHashItem;
   psnTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psnHashItem );

   pHashItem->txStatus.txOffset = PSN_NULL_OFFSET;
   pHashItem->txStatus.status = PSN_TXS_OK;
   pHashMap->nodeObject.txCounter--;

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psnHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
   if ( (txHashMapStatus->usageCounter == 0) &&
                   (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) { 
         psnHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashMapCommitRemove( psnHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              psnSessionContext * pContext )
{
   psnHashItem * pHashItem;
   psnTxStatus * txItemStatus;
   psnTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psnHashItem );
   txItemStatus = &pHashItem->txStatus;
   /* 
    * If someone is using it, the usageCounter will be greater than zero.
    * If it zero, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( txItemStatus->usageCounter == 0 ) {
      psnHashDelWithItem( &pHashMap->hashObj, 
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
       * Note: we do not check the return value of psnHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
         GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
         if ( (txHashMapStatus->usageCounter == 0) &&
            (pHashMap->nodeObject.txCounter == 0 ) ) {
            psnHashResize( &pHashMap->hashObj, pContext );
         }
      }
   }
   else {
      txItemStatus->status = PSN_TXS_DESTROYED_COMMITTED;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashMapDelete( psnHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength, 
                        psnSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   psnHashItem* pHashItem = NULL;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found, ok;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /*
       * The first step is to retrieve the item.
       */
      found = psnHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey,
                           keyLength,
                           &pHashItem,
                           &bucket,
                           pContext );
      if ( ! found ) {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }
      while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
         GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
      }
      
      txItemStatus = &pHashItem->txStatus;

      /* 
       * If the transaction id of the item is non-zero, a big no-no - 
       * we do not support two transactions on the same data
       * (and if remove is committed - the data is "non-existent").
       */
      if ( txItemStatus->txOffset != PSN_NULL_OFFSET ) {
         if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
            errcode = VDS_NO_SUCH_ITEM;
         }
         else {
            errcode = VDS_ITEM_IS_IN_USE;
         }
         goto the_exit;
      }

      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_REMOVE_DATA,
                         SET_OFFSET(pHashMap),
                         PSN_IDENT_HASH_MAP,
                         SET_OFFSET( pHashItem),
                         0,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto the_exit;
      
      txItemStatus->txOffset = SET_OFFSET(pContext->pTransaction);
      txItemStatus->status = PSN_TXS_DESTROYED;
      pHashMap->nodeObject.txCounter++;

      psnUnlock( &pHashMap->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;
   
the_exit:

   psnUnlock( &pHashMap->memObject, pContext );
   /* pscSetError might have been already called by some other function */
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashMapFini( psnHashMap        * pHashMap,
                      psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   psnHashFini( &pHashMap->hashObj );
   psnTreeNodeFini( &pHashMap->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   psnMemObjectFini(  &pHashMap->memObject, PSN_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashMapGet( psnHashMap        * pHashMap,
                     const void         * pKey,
                     size_t               keyLength, 
                     psnHashItem      ** ppHashItem,
                     size_t               bufferLength,
                     psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL, * previousItem = NULL;
   vdsErrors errcode;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pKey       != NULL );
   VDS_PRE_CONDITION( ppHashItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }

      found = psnHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey, 
                           keyLength,
                           &pHashItem,
                           &bucket,
                           pContext );
      if ( ! found ) {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }
      while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
         previousItem = pHashItem;
         GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
      }

      /*
       * This test cannot be done in the API (before calling the current
       * function) since we do not know the item size. It could be done
       * after but this way makes the code faster.
       */
      if ( bufferLength < pHashItem->dataLength ) {
         errcode = VDS_INVALID_LENGTH;
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
      if ( txItemStatus->txOffset != PSN_NULL_OFFSET ) {
         if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
            errcode = VDS_NO_SUCH_ITEM;
            goto the_exit;
         }
         if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) &&
            txItemStatus->status & PSN_TXS_DESTROYED ) {
            errcode = VDS_ITEM_IS_DELETED;
            goto the_exit;
         }
         if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) &&
            txItemStatus->status & PSN_TXS_ADDED ) {
            errcode = VDS_ITEM_IS_IN_USE;
            goto the_exit;
         }
         if ( txItemStatus->status & PSN_TXS_REPLACED ) {
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

      psnUnlock( &pHashMap->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   return true;

the_exit:

   psnUnlock( &pHashMap->memObject, pContext );
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

bool psnHashMapGetFirst( psnHashMap        * pHashMap,
                          psnHashMapItem    * pItem,
                          size_t               keyLength,
                          size_t               bufferLength,
                          psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL;
   psnTxStatus * txItemStatus;
   psnTxStatus * txHashMapStatus;
   ptrdiff_t  firstItemOffset;
   bool isOK, found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      found = psnHashGetFirst( &pHashMap->hashObj, 
                                &firstItemOffset );
      while ( found ) {
         GET_PTR( pHashItem, firstItemOffset, psnHashItem );
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
         if ( txItemStatus->txOffset != PSN_NULL_OFFSET ) {
            if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSN_TXS_DESTROYED ) {
               isOK = false;
            }
            if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSN_TXS_ADDED ) {
               isOK = false;
            }
            if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
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
               psnUnlock( &pHashMap->memObject, pContext );
               pscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return false;
            }
            if ( keyLength < pHashItem->keyLength ) {
               psnUnlock( &pHashMap->memObject, pContext );
               pscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return false;
            }

            txItemStatus->usageCounter++;
            txHashMapStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = firstItemOffset;

            psnUnlock( &pHashMap->memObject, pContext );
            
            return true;
         }
  
         found = psnHashGetNext( &pHashMap->hashObj, 
                                  firstItemOffset,
                                  &firstItemOffset );
      }
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   psnUnlock( &pHashMap->memObject, pContext );
   pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );

   return false;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashMapGetNext( psnHashMap        * pHashMap,
                         psnHashMapItem    * pItem,
                         size_t               keyLength,
                         size_t               bufferLength,
                         psnSessionContext * pContext )
{
   psnHashItem * pHashItem = NULL;
   psnHashItem * previousHashItem = NULL;
   psnTxStatus * txItemStatus;
   psnTxStatus * txHashMapStatus;
   ptrdiff_t  itemOffset;
   bool isOK, found;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != PSN_NULL_OFFSET );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      found = psnHashGetNext( &pHashMap->hashObj, 
                                 itemOffset,
                                 &itemOffset );
      while ( found ) {
         GET_PTR( pHashItem, itemOffset, psnHashItem );
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
         if ( txItemStatus->txOffset != PSN_NULL_OFFSET ) {
            if ( txItemStatus->txOffset == SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSN_TXS_DESTROYED ) {
               isOK = false;
            }
            if ( txItemStatus->txOffset != SET_OFFSET(pContext->pTransaction) &&
               txItemStatus->status & PSN_TXS_ADDED ) {
               isOK = false;
            }
            if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
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
               psnUnlock( &pHashMap->memObject, pContext );
               pscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return false;
            }
            if ( keyLength < pHashItem->keyLength ) {
               psnUnlock( &pHashMap->memObject, pContext );
               pscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return false;
            }

            txItemStatus->usageCounter++;
            txHashMapStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = itemOffset;
            psnHashMapReleaseNoLock( pHashMap, previousHashItem, pContext );

            psnUnlock( &pHashMap->memObject, pContext );
            
            return true;
         }
  
         found = psnHashGetNext( &pHashMap->hashObj, 
                                  itemOffset,
                                  &itemOffset );
      }
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   pItem->pHashItem = NULL;
   pItem->itemOffset = PSN_NULL_OFFSET;
   psnHashMapReleaseNoLock( pHashMap, previousHashItem, pContext );
   
   psnUnlock( &pHashMap->memObject, pContext );
   pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashMapInit( psnHashMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfItems,
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
   
   VDS_PRE_CONDITION( pHashMap     != NULL );
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( pTxStatus    != NULL );
   VDS_PRE_CONDITION( origName     != NULL );
   VDS_PRE_CONDITION( pDefinition  != NULL );
   VDS_PRE_CONDITION( hashItemOffset != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( parentOffset   != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   VDS_PRE_CONDITION( pDefinition->numFields > 0 );
   
   errcode = psnMemObjectInit( &pHashMap->memObject, 
                                PSN_IDENT_HASH_MAP,
                                &pHashMap->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return false;
   }

   psnTreeNodeInit( &pHashMap->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = psnHashInit( &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject),
                           expectedNumOfItems, 
                           pContext );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    errcode );
      return false;
   }
   
   pHashMap->numFields = (uint16_t) pDefinition->numFields;

   ptr = (psnFieldDef*) psnMalloc( &pHashMap->memObject, 
                                     pHashMap->numFields* sizeof(psnFieldDef),
                                     pContext );
   if ( ptr == NULL ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      return false;
   }
   pHashMap->dataDefOffset = SET_OFFSET(ptr);

   for ( i = 0; i < pHashMap->numFields; ++i) {
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
   memcpy( &pHashMap->keyDef, &pDefinition->key, sizeof(vdsKeyDefinition) );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashMapInsert( psnHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength, 
                        const void         * pData,
                        size_t               itemLength,
                        psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL, * previousHashItem = NULL;
   vdsErrors errcode = VDS_OK;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found, ok;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pData    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      found = psnHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey, 
                           keyLength,
                           &previousHashItem,
                           &bucket,
                           pContext );
      if ( found ) {
         /* Find the last one in the chain of items with same key */
         while ( previousHashItem->nextSameKey != PSN_NULL_OFFSET ) {
            GET_PTR( previousHashItem, previousHashItem->nextSameKey, psnHashItem );
         }

         /* 
          * Anything othor than a deleted item committed means that the
          * key exists and that we cannot insert the item
          */
         txItemStatus = &previousHashItem->txStatus;
         if ( txItemStatus->status != PSN_TXS_DESTROYED_COMMITTED ) {
            errcode = VDS_ITEM_ALREADY_PRESENT;
            goto the_exit;
         }
      }
      
      errcode = psnHashInsertAt( &pHashMap->hashObj,
                                  bucket,
                                  (unsigned char *)pKey, 
                                  keyLength, 
                                  pData, 
                                  itemLength,
                                  &pHashItem,
                                  pContext );
      if ( errcode != VDS_OK ) goto the_exit;

      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_ADD_DATA,
                         SET_OFFSET(pHashMap),
                         PSN_IDENT_HASH_MAP,
                         SET_OFFSET(pHashItem),
                         0,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psnHashDelWithItem( &pHashMap->hashObj,
                              pHashItem,
                              pContext );
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      psnTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      pHashMap->nodeObject.txCounter++;
      txItemStatus->status = PSN_TXS_ADDED;
      if ( previousHashItem != NULL ) {
         previousHashItem->nextSameKey = SET_OFFSET(pHashItem);
      }
      
      psnUnlock( &pHashMap->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;

the_exit:

   psnUnlock( &pHashMap->memObject, pContext );
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

bool psnHashMapRelease( psnHashMap        * pHashMap,
                         psnHashItem       * pHashItem,
                         psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      psnHashMapReleaseNoLock( pHashMap,
                                pHashItem,
                                pContext );

      psnUnlock( &pHashMap->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    VDS_OBJECT_CANNOT_GET_LOCK );
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
void psnHashMapReleaseNoLock( psnHashMap        * pHashMap,
                               psnHashItem       * pHashItem,
                               psnSessionContext * pContext )
{
   psnTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   txItemStatus = &pHashItem->txStatus;
   
   txItemStatus->usageCounter--;
   txHashMapStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
      /* Time to really delete the record! */
      psnHashDelWithItem( &pHashMap->hashObj, 
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
    * Note: we do not check the return value of psnHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
         psnHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnHashMapReplace( psnHashMap        * pHashMap,
                         const void         * pKey,
                         size_t               keyLength, 
                         const void         * pData,
                         size_t               itemLength,
                         psnSessionContext * pContext )
{
   psnHashItem * pHashItem, * pNewHashItem;
   vdsErrors errcode = VDS_OK;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   size_t bucket;
   bool found, ok;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pData    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pHashMap->memObject, pContext ) ) {
      if ( ! psnTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || psnTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }

      found = psnHashGet( &pHashMap->hashObj, 
                           (unsigned char *)pKey, 
                           keyLength,
                           &pHashItem,
                           &bucket,
                           pContext );
      if ( ! found ) {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }
      while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
         GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
      }

      txItemStatus = &pHashItem->txStatus;
      if ( txItemStatus->status != PSN_TXS_OK ) {
         errcode = VDS_ITEM_IS_IN_USE;
         goto the_exit;
      }
      
      errcode = psnHashInsertAt( &pHashMap->hashObj,
                                  bucket,
                                  (unsigned char *)pKey, 
                                  keyLength, 
                                  pData, 
                                  itemLength,
                                  &pNewHashItem,
                                  pContext );
      if ( errcode != VDS_OK ) goto the_exit;

      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_REMOVE_DATA,
                         SET_OFFSET(pHashMap),
                         PSN_IDENT_HASH_MAP,
                         SET_OFFSET(pHashItem),
                         0,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psnHashDelWithItem( &pHashMap->hashObj, 
                              pNewHashItem,
                              pContext );
         goto the_exit;
      }
      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_ADD_DATA,
                         SET_OFFSET(pHashMap),
                         PSN_IDENT_HASH_MAP,
                         SET_OFFSET(pNewHashItem),
                         0,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psnHashDelWithItem( &pHashMap->hashObj, 
                              pNewHashItem,
                              pContext );
         psnTxRemoveLastOps( (psnTx*)pContext->pTransaction, pContext );
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      psnTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      txItemStatus->status = PSN_TXS_DESTROYED;

      txItemStatus = &pNewHashItem->txStatus;
      psnTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      txItemStatus->status = PSN_TXS_REPLACED;

      pHashItem->nextSameKey = SET_OFFSET(pNewHashItem);
      pHashMap->nodeObject.txCounter += 2;

      psnUnlock( &pHashMap->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;

the_exit:

   psnUnlock( &pHashMap->memObject, pContext );
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

void psnHashMapRollbackAdd( psnHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             psnSessionContext * pContext )
{
   psnHashItem * pHashItem;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psnHashItem );
   txItemStatus = &pHashItem->txStatus;
   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. But it could be accessed by the current session,
    * for example during an iteration. To rollback we need to remove it 
    * from the hash (this function will also free the memory back to 
    * the memory object).
    */
   if ( txItemStatus->usageCounter == 0 ) {
      psnHashDelWithItem( &pHashMap->hashObj, 
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
       * Note: we do not check the return value of psnHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
      if ( (txHashMapStatus->usageCounter == 0) &&
                     (pHashMap->nodeObject.txCounter == 0 ) ) {
         if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
            psnHashResize( &pHashMap->hashObj, pContext );
         }
      }
   }
   else {
      psnTxStatusCommitRemove( txItemStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashMapRollbackRemove( psnHashMap        * pHashMap, 
                                ptrdiff_t            itemOffset,
                                psnSessionContext * pContext  )
{
   psnHashItem * pHashItem;
   psnTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != PSN_NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, psnHashItem );
   txItemStatus = &pHashItem->txStatus;

   /*
    * This call resets the transaction (to "none") and remove the 
    * bit that flag this data as being in the process of being removed.
    */
   psnTxStatusUnmarkAsDestroyed( txItemStatus );
   pHashMap->nodeObject.txCounter--;
   
   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psnHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
         psnHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnHashMapStatus( psnHashMap  * pHashMap,
                        vdsObjStatus * pStatus )
{
   psnHashItem* pHashItem = NULL;
   ptrdiff_t  firstItemOffset;
   psnTxStatus  * txStatus;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pStatus  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_HASH_MAP );
   
   GET_PTR( txStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   found = psnHashGetFirst( &pHashMap->hashObj, 
                               &firstItemOffset );
   while ( found ) {
      GET_PTR( pHashItem, firstItemOffset, psnHashItem );
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      found = psnHashGetNext( &pHashMap->hashObj, 
                               firstItemOffset,
                               &firstItemOffset );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

