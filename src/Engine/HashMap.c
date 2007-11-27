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

#include "Engine/HashMap.h"
#include "Engine/Transaction.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration of static functions */

static
void vdseHashMapReleaseNoLock( vdseHashMap        * pHashMap,
                              vdseHashItem       * pHashItem,
                              vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashMapCommitAdd( vdseHashMap        * pHashMap, 
                           ptrdiff_t            itemOffset,
                           vdseSessionContext * pContext  )
{
   vdseHashItem * pHashItem;
   vdseTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, vdseHashItem );

   vdseTxStatusSetTx( &pHashItem->txStatus, NULL_OFFSET );
   pHashMap->nodeObject.txCounter--;

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of vdseHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) )
   {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE )
         vdseHashResize( &pHashMap->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashMapCommitRemove( vdseHashMap        * pHashMap, 
                              ptrdiff_t            itemOffset,
                              vdseSessionContext * pContext )
{
   vdseHashItem * pHashItem;
   enum ListErrors errcode =  LIST_OK;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, vdseHashItem );
   txItemStatus = &pHashItem->txStatus;
   /* 
    * If someone is using it, the usageCounter will be greater than zero.
    * If it zero, we can safely remove the entry from the hash, otherwise
    * we mark it as a committed remove
    */
   if ( txItemStatus->usageCounter == 0 )
   {
      errcode = vdseHashDelete( &pHashMap->hashObj, 
                                pHashItem->key,
                                pHashItem->keyLength,
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
       * Note: we do not check the return value of vdseHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
      if ( (txHashMapStatus->usageCounter == 0) &&
         (pHashMap->nodeObject.txCounter == 0 ) )
      {
         if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE )
            vdseHashResize( &pHashMap->hashObj, pContext );
      }
   }
   else
   {
      vdseTxStatusCommitRemove( txItemStatus );
   }
   
   VDS_POST_CONDITION( errcode == LIST_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapDelete( vdseHashMap        * pHashMap,
                       const void         * pKey,
                       size_t               keyLength, 
                       vdseSessionContext * pContext )
{
   int rc;
   vdsErrors errcode = VDS_OK;
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) )
      {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      /*
       * The first step is to retrieve the item.
       */
      listErr = vdseHashGet( &pHashMap->hashObj, 
                             (unsigned char *)pKey,
                             keyLength,
                             &pHashItem,
                             pContext,
                             NULL );
      if ( listErr != LIST_OK )
      {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }

      /* txStatus is now for the item to delete, not the hash map */
      txItemStatus = &pHashItem->txStatus;
   
      /* 
       * If the item (to delete) transaction id is not either equal to
       * zero or to the current transaction id, then it belongs to 
       * another transaction - uncommitted. For the current transaction it
       * is as if it does not exist.
       * Similarly, if the item is already marked as destroyed... can't 
       * remove ourselves twice...
       */
      if ( ! vdseTxStatusIsValid( txItemStatus, SET_OFFSET(pContext->pTransaction) ) 
            || vdseTxStatusIsMarkedAsDestroyed( txItemStatus ) )
      {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }
   
      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_REMOVE_DATA,
                         SET_OFFSET(pHashMap),
                         VDSE_IDENT_HASH_MAP,
                         SET_OFFSET( pHashItem),
                         0,
                         pContext );
      if ( rc != 0 )
         goto the_exit;
      
      vdseTxStatusSetTx( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      vdseTxStatusMarkAsDestroyed( txItemStatus );
      pHashMap->nodeObject.txCounter++;

      vdseUnlock( &pHashMap->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;
   
the_exit:

   vdseUnlock( &pHashMap->memObject, pContext );
   /* vdscSetError might have been already called by some other function */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashMapFini( vdseHashMap        * pHashMap,
                      vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   vdseHashFini( &pHashMap->hashObj );
   vdseTreeNodeFini( &pHashMap->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   vdseMemObjectFini(  &pHashMap->memObject, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapGet( vdseHashMap        * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    vdseHashItem      ** ppHashItem,
                    size_t               bufferLength,
                    vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL;
   vdsErrors errcode;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pKey       != NULL );
   VDS_PRE_CONDITION( ppHashItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) )
      {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }

      listErr = vdseHashGet( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             &pHashItem,
                             pContext,
                             NULL );
      if ( listErr != LIST_OK )
      {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }
      /*
       * This test cannot be done in the API (before calling the current
       * function) since we do not know the item size. It could be done
       * after but this way makes the code faster.
       */
      if ( bufferLength < pHashItem->dataLength )
      {
         errcode = VDS_INVALID_LENGTH;
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
  
      /* 
       * If the transaction id of the item (to retrieve) is not either equal
       * to zero or to the current transaction id, then it belongs to 
       * another transaction - uncommitted. For the current transaction it
       * is as if it does not exist.
       * Similarly, if the object is marked as destroyed... we can't access it. 
       * (to have the id as ok and be marked as destroyed is a rare case - 
       * it would require that the current transaction deleted the item and 
       * than tries to access it).
       */
      if ( ! vdseTxStatusIsValid( txItemStatus, SET_OFFSET(pContext->pTransaction) )
         || vdseTxStatusIsMarkedAsDestroyed( txItemStatus ) )
      {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }

      /*
       * We must increment the usage counter since we are passing back
       * a pointer to the data, not a copy. 
       */
      txItemStatus->usageCounter++;
      txHashMapStatus->usageCounter++;
      *ppHashItem = pHashItem;

      vdseUnlock( &pHashMap->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }
   
   return 0;

the_exit:

   vdseUnlock( &pHashMap->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapGetFirst( vdseHashMap        * pHashMap,
                         vdseHashMapItem    * pItem,
                         size_t               keyLength,
                         size_t               bufferLength,
                         vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txHashMapStatus;
   size_t     bucket;
   ptrdiff_t  firstItemOffset;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      listErr = vdseHashGetFirst( &pHashMap->hashObj, 
                                  &bucket,
                                  &firstItemOffset );
      while ( listErr == LIST_OK )
      {
         GET_PTR( pHashItem, firstItemOffset, vdseHashItem );
         txItemStatus = &pHashItem->txStatus;
         if ( vdseTxStatusIsValid( txItemStatus, SET_OFFSET(pContext->pTransaction) ) 
             && ! vdseTxStatusIsMarkedAsDestroyed( txItemStatus ) )
         {
            /*
             * These tests cannot be done in the API (before calling the 
             * current function) since we do not know the item size. They 
             * could be done after but this way makes the code faster.
             */
            if ( bufferLength < pHashItem->dataLength )
            {
               vdseUnlock( &pHashMap->memObject, pContext );
               vdscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return -1;
            }
            if ( keyLength < pHashItem->keyLength )
            {
               vdseUnlock( &pHashMap->memObject, pContext );
               vdscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return -1;
            }

            txItemStatus->usageCounter++;
            txHashMapStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->bucket = bucket;
            pItem->itemOffset = firstItemOffset;

            vdseUnlock( &pHashMap->memObject, pContext );
            
            return 0;
         }
  
         listErr = vdseHashGetNext( &pHashMap->hashObj, 
                                    bucket,
                                    firstItemOffset,
                                    &bucket,
                                    &firstItemOffset );
      }
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }
   
   vdseUnlock( &pHashMap->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );

   return -1;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapGetNext( vdseHashMap        * pHashMap,
                        vdseHashMapItem    * pItem,
                        size_t               keyLength,
                        size_t               bufferLength,
                        vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem * pHashItem = NULL;
   vdseHashItem * previousHashItem = NULL;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txHashMapStatus;
   size_t     bucket;
   ptrdiff_t  itemOffset;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != NULL_OFFSET );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   bucket           = pItem->bucket;
   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      listErr = vdseHashGetNext( &pHashMap->hashObj, 
                                 bucket,
                                 itemOffset,
                                 &bucket,
                                 &itemOffset );
      while ( listErr == LIST_OK )
      {
         GET_PTR( pHashItem, itemOffset, vdseHashItem );
         txItemStatus = &pHashItem->txStatus;
         if ( vdseTxStatusIsValid( txItemStatus, SET_OFFSET(pContext->pTransaction) ) 
             && ! vdseTxStatusIsMarkedAsDestroyed( txItemStatus ) )
         {
            /*
             * These tests cannot be done in the API (before calling the 
             * current function) since we do not know the item size. They 
             * could be done after but this way makes the code faster.
             */
            if ( bufferLength < pHashItem->dataLength )
            {
               vdseUnlock( &pHashMap->memObject, pContext );
               vdscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return -1;
            }
            if ( keyLength < pHashItem->keyLength )
            {
               vdseUnlock( &pHashMap->memObject, pContext );
               vdscSetError( &pContext->errorHandler, 
                             g_vdsErrorHandle, VDS_INVALID_LENGTH );
               return -1;
            }

            txItemStatus->usageCounter++;
            txHashMapStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->bucket = bucket;
            pItem->itemOffset = itemOffset;
            vdseHashMapReleaseNoLock( pHashMap, previousHashItem, pContext );

            vdseUnlock( &pHashMap->memObject, pContext );
            
            return 0;
         }
  
         listErr = vdseHashGetNext( &pHashMap->hashObj, 
                                    bucket,
                                    itemOffset,
                                    &bucket,
                                    &itemOffset );
      }
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   pItem->pHashItem = NULL;
   pItem->bucket = 0;
   pItem->itemOffset = NULL_OFFSET;
   vdseHashMapReleaseNoLock( pHashMap, previousHashItem, pContext );
   
   vdseUnlock( &pHashMap->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapInit( vdseHashMap        * pHashMap,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfItems,
                     vdseTxStatus       * pTxStatus,
                     size_t               origNameLength,
                     vdsChar_T          * origName,
                     ptrdiff_t            keyOffset,
                     vdseSessionContext * pContext )
{
   vdsErrors errcode;
   enum ListErrors listErr;
   
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pTxStatus != NULL );
   VDS_PRE_CONDITION( origName  != NULL );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   
   errcode = vdseMemObjectInit( &pHashMap->memObject, 
                                VDSE_IDENT_HASH_MAP,
                                &pHashMap->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }

   vdseTreeNodeInit( &pHashMap->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     keyOffset );

   listErr = vdseHashInit( &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject),
                           expectedNumOfItems, 
                           pContext );
   if ( listErr != LIST_OK )
   {
      if ( listErr == LIST_NO_MEMORY )
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
      else
         errcode = VDS_INTERNAL_ERROR;
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapInsert( vdseHashMap        * pHashMap,
                       const void         * pKey,
                       size_t               keyLength, 
                       const void         * pItem,
                       size_t               itemLength,
                       vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem;
   vdsErrors errcode = VDS_OK;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   int rc;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) )
      {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }
   
      listErr = vdseHashInsert( &pHashMap->hashObj, 
                                (unsigned char *)pKey, 
                                keyLength, 
                                pItem, 
                                itemLength,
                                &pHashItem,
                                pContext );
      if ( listErr != LIST_OK )
      {
         if ( listErr == LIST_KEY_FOUND )
            errcode = VDS_ITEM_ALREADY_PRESENT;
         else if ( listErr == LIST_NO_MEMORY )
            errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         else
            errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD_DATA,
                         SET_OFFSET(pHashMap),
                         VDSE_IDENT_HASH_MAP,
                         SET_OFFSET(pHashItem),
                         0,
                         pContext );
      if ( rc != 0 )
      {
         vdseHashDelete( &pHashMap->hashObj, 
                         (unsigned char*)pKey,
                         keyLength, 
                         pContext );
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      vdseTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      pHashMap->nodeObject.txCounter++;
   
      vdseUnlock( &pHashMap->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;

the_exit:

   vdseUnlock( &pHashMap->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapRelease( vdseHashMap        * pHashMap,
                        vdseHashItem       * pHashItem,
                        vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      vdseHashMapReleaseNoLock( pHashMap,
                                pHashItem,
                                pContext );

      vdseUnlock( &pHashMap->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * This version of Release is only called internally, by the current
 * object. Always use the standard one when calling from the API.
 */
static
void vdseHashMapReleaseNoLock( vdseHashMap        * pHashMap,
                               vdseHashItem       * pHashItem,
                               vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   txItemStatus = &pHashItem->txStatus;
   
   txItemStatus->usageCounter--;
   txHashMapStatus->usageCounter--;

   if ( (txItemStatus->usageCounter == 0) && 
      vdseTxStatusIsRemoveCommitted(txItemStatus) )
   {
      /* Time to really delete the record! */
      listErr = vdseHashDelete( &pHashMap->hashObj, 
                                pHashItem->key,
                                pHashItem->keyLength,
                                pContext );
      pHashMap->nodeObject.txCounter--;

      VDS_POST_CONDITION( listErr == LIST_OK );
   }
   
   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of vdseHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) )
   {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE )
         vdseHashResize( &pHashMap->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseHashMapReplace( vdseHashMap        * pHashMap,
                        const void         * pKey,
                        size_t               keyLength, 
                        const void         * pItem,
                        size_t               itemLength,
                        vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem * pHashItem, * pNewHashItem;
   vdsErrors errcode = VDS_OK;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   int rc;
   size_t bucket;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_HASH_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pHashMap->memObject, pContext ) == 0 )
   {
      if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) )
      {
         errcode = VDS_OBJECT_IS_DELETED;
         goto the_exit;
      }

      listErr = vdseHashGet( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             &pHashItem,
                             pContext,
                             &bucket );
      if ( listErr != LIST_OK )
      {
         errcode = VDS_NO_SUCH_ITEM;
         goto the_exit;
      }

      listErr = vdseHashInsertAt( &pHashMap->hashObj,
                                  bucket,
                                  (unsigned char *)pKey, 
                                  keyLength, 
                                  pItem, 
                                  itemLength,
                                  &pNewHashItem,
                                  pContext );
      if ( listErr != LIST_OK )
      {
         if ( listErr == LIST_KEY_FOUND )
            errcode = VDS_ITEM_ALREADY_PRESENT;
         else if ( listErr == LIST_NO_MEMORY )
            errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         else
            errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_REMOVE_DATA,
                         SET_OFFSET(pHashMap),
                         VDSE_IDENT_HASH_MAP,
                         SET_OFFSET(pHashItem),
                         0,
                         pContext );
      if ( rc != 0 )
      {
         vdseHashDeleteAt( &pHashMap->hashObj, 
                           bucket,
                           pNewHashItem,
                           pContext );
         goto the_exit;
      }
      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD_DATA,
                         SET_OFFSET(pHashMap),
                         VDSE_IDENT_HASH_MAP,
                         SET_OFFSET(pNewHashItem),
                         0,
                         pContext );
      if ( rc != 0 )
      {
         vdseHashDeleteAt( &pHashMap->hashObj, 
                           bucket,
                           pNewHashItem,
                           pContext );
         vdseTxRemoveLastOps( (vdseTx*)pContext->pTransaction, pContext );
         goto the_exit;
      }
      
      txItemStatus = &pHashItem->txStatus;
      vdseTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );
      vdseTxStatusMarkAsDestroyed( txItemStatus );

      txItemStatus = &pNewHashItem->txStatus;
      vdseTxStatusInit( txItemStatus, SET_OFFSET(pContext->pTransaction) );

      pHashMap->nodeObject.txCounter += 2;

      vdseUnlock( &pHashMap->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;

the_exit:

   vdseUnlock( &pHashMap->memObject, pContext );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashMapRollbackAdd( vdseHashMap        * pHashMap, 
                             ptrdiff_t            itemOffset,
                             vdseSessionContext * pContext )
{
   vdseHashItem * pHashItem;
   enum ListErrors errcode = LIST_OK;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, vdseHashItem );
   txItemStatus = &pHashItem->txStatus;
   /* 
    * A new entry that isn't yet committed cannot be accessed by some
    * other session. But it could be accessed by the current session,
    * for example during an iteration. To rollback we need to remove it 
    * from the hash (this function will also free the memory back to 
    * the memory object).
    */
   if ( txItemStatus->usageCounter == 0 )
   {
      errcode = vdseHashDelete( &pHashMap->hashObj, 
                                pHashItem->key,
                                pHashItem->keyLength, 
                                pContext );
      pHashMap->nodeObject.txCounter--;
   
      VDS_POST_CONDITION( errcode == LIST_OK );
      
      /*
       * Do we need to resize? We need both conditions here:
       *
       *   - txHashMapStatus->usageCounter someone has a pointer to the data
       *
       *   - nodeObject.txCounter: offset to some of our data is part of a
       *                           transaction.
       *
       * Note: we do not check the return value of vdseHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
      if ( (txHashMapStatus->usageCounter == 0) &&
         (pHashMap->nodeObject.txCounter == 0 ) )
      {
         if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE )
            vdseHashResize( &pHashMap->hashObj, pContext );
      }
   }
   else
   {
      vdseTxStatusCommitRemove( txItemStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashMapRollbackRemove( vdseHashMap        * pHashMap, 
                                ptrdiff_t            itemOffset,
                                vdseSessionContext * pContext  )
{
   vdseHashItem * pHashItem;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, vdseHashItem );
   txItemStatus = &pHashItem->txStatus;

   /*
    * This call resets the transaction (to "none") and remove the 
    * bit that flag this data as being in the process of being removed.
    */
   vdseTxStatusUnmarkAsDestroyed(  txItemStatus );
   pHashMap->nodeObject.txCounter--;
   
   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txHashMapStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of vdseHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   if ( (txHashMapStatus->usageCounter == 0) &&
      (pHashMap->nodeObject.txCounter == 0 ) )
   {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE )
         vdseHashResize( &pHashMap->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseHashMapStatus( vdseHashMap  * pHashMap,
                        vdsObjStatus * pStatus )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pStatus  != NULL );
   
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

