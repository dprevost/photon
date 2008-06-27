/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/Map.h"
#include "Engine/Transaction.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration of static functions */

static
void vdseMapReleaseNoLock( vdseMap            * pHashMap,
                           vdseHashItem       * pHashItem,
                           vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapCommitAdd( vdseMap            * pHashMap, 
                       ptrdiff_t            itemOffset,
                       vdseSessionContext * pContext  )
{
   vdseHashItem * pHashItem;
   vdseTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( itemOffset != NULL_OFFSET );

   GET_PTR( pHashItem, itemOffset, vdseHashItem );

   pHashItem->txStatus.txOffset = NULL_OFFSET;
   pHashItem->txStatus.enumStatus = VDSE_TXS_OK;
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
                   (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) { 
         vdseHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapCommitRemove( vdseMap            * pHashMap, 
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
   if ( txItemStatus->usageCounter == 0 ) {
      errcode = vdseHashDelete( &pHashMap->hashObj, 
                                pHashItem->key,
                                pHashItem->keyLength,
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
       * Note: we do not check the return value of vdseHashResize since the
       *       current function returns void. Let's someone else find that 
       *       we are getting low on memory...
       */
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
         GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
         if ( (txHashMapStatus->usageCounter == 0) &&
            (pHashMap->nodeObject.txCounter == 0 ) ) {
            vdseHashResize( &pHashMap->hashObj, pContext );
         }
      }
   }
   else {
      txItemStatus->enumStatus = VDSE_TXS_DESTROYED_COMMITTED;
   }
   
   VDS_POST_CONDITION( errcode == LIST_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapDelete( vdseMap            * pHashMap,
                   const void         * pKey,
                   size_t               keyLength, 
                   vdseSessionContext * pContext )
{
//   int rc;
//   vdsErrors errcode = VDS_OK;
   enum ListErrors listErr = LIST_OK;
//   vdseHashItem* pHashItem = NULL;
//   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   
   listErr = vdseHashDeleteRaw( &pHashMap->hashObj, 
                                (unsigned char *)pKey,
                                keyLength,
                                pContext );
   if ( listErr != LIST_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NO_SUCH_ITEM );
      return -1;
   }

   /*
    * Note: we do not check the return value of vdseHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
      vdseHashResize( &pHashMap->hashObj, pContext );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapFini( vdseMap        * pHashMap,
                      vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   vdseHashFini( &pHashMap->hashObj );
   vdseTreeNodeFini( &pHashMap->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   vdseMemObjectFini(  &pHashMap->memObject, VDSE_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapGet( vdseMap            * pHashMap,
                const void         * pKey,
                size_t               keyLength, 
                vdseHashItem      ** ppHashItem,
                size_t               bufferLength,
                vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL; //, * previousItem = NULL;
   vdsErrors errcode;
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pKey       != NULL );
   VDS_PRE_CONDITION( ppHashItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
      || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
      errcode = VDS_OBJECT_IS_DELETED;
      goto the_exit;
   }

   listErr = vdseHashGet( &pHashMap->hashObj, 
                          (unsigned char *)pKey, 
                          keyLength,
                          &pHashItem,
                          pContext,
                          NULL );
   if ( listErr != LIST_OK ) {
      errcode = VDS_NO_SUCH_ITEM;
      goto the_exit;
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
    * We must increment the usage counter since we are passing back
    * a pointer to the data, not a copy. 
    */
   txItemStatus->usageCounter++;
   txHashMapStatus->usageCounter++;
   *ppHashItem = pHashItem;
   
   return 0;

the_exit:

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

int vdseMapGetFirst( vdseMap            * pHashMap,
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
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
      || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_IS_DELETED );
      return -1;
   }

   listErr = vdseHashGetFirst( &pHashMap->hashObj, 
                               &bucket,
                               &firstItemOffset );
   if ( listErr != LIST_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );
      return -1;
   }
   
   GET_PTR( pHashItem, firstItemOffset, vdseHashItem );
   txItemStatus = &pHashItem->txStatus;

   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return -1;
   }
   if ( keyLength < pHashItem->keyLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return -1;
   }

   txItemStatus->usageCounter++;
   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   pItem->bucket = bucket;
   pItem->itemOffset = firstItemOffset;

   return 0;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapGetNext( vdseMap            * pHashMap,
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
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != NULL_OFFSET );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( ! vdseTxStatusIsValid( txHashMapStatus, SET_OFFSET(pContext->pTransaction) ) 
      || vdseTxStatusIsMarkedAsDestroyed( txHashMapStatus ) ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_IS_DELETED );
      return -1;
   }

   bucket           = pItem->bucket;
   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   listErr = vdseHashGetNext( &pHashMap->hashObj, 
                              bucket,
                              itemOffset,
                              &bucket,
                              &itemOffset );
   if ( listErr != LIST_OK ) {
      /* 
       * If we come here, there are no additional data items to retrieve. As 
       * long as we clearly say that the internal iterator is reset (in case a 
       * "Get Previous" is implemented later), we can just release the iterator
       * at this point.
       */
      pItem->pHashItem = NULL;
      pItem->bucket = 0;
      pItem->itemOffset = NULL_OFFSET;
      vdseMapReleaseNoLock( pHashMap, previousHashItem, pContext );
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );
      return -1;
   }
   
   GET_PTR( pHashItem, itemOffset, vdseHashItem );
   txItemStatus = &pHashItem->txStatus;

   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return -1;
   }
   if ( keyLength < pHashItem->keyLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return -1;
   }

   txItemStatus->usageCounter++;
   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   pItem->bucket = bucket;
   pItem->itemOffset = itemOffset;
   vdseMapReleaseNoLock( pHashMap, previousHashItem, pContext );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapInit( vdseMap             * pHashMap,
                 ptrdiff_t             parentOffset,
                 size_t                numberOfBlocks,
                 size_t                expectedNumOfItems,
                 vdseTxStatus        * pTxStatus,
                 size_t                origNameLength,
                 char                * origName,
                 ptrdiff_t             keyOffset,
                 vdsObjectDefinition * pDefinition,
                 vdseSessionContext  * pContext )
{
   vdsErrors errcode;
   enum ListErrors listErr;
   vdseFieldDef * ptr;
   unsigned int i;
   
   VDS_PRE_CONDITION( pHashMap     != NULL );
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( pTxStatus    != NULL );
   VDS_PRE_CONDITION( origName     != NULL );
   VDS_PRE_CONDITION( pDefinition  != NULL );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   VDS_PRE_CONDITION( pDefinition->numFields > 0 );
   
   errcode = vdseMemObjectInit( &pHashMap->memObject, 
                                VDSE_IDENT_MAP,
                                &pHashMap->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK ) {
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
   if ( listErr != LIST_OK ) {
      if ( listErr == LIST_NO_MEMORY ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
      }
      else {
         errcode = VDS_INTERNAL_ERROR;
      }
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }
   
   pHashMap->numFields = (uint16_t) pDefinition->numFields;

   ptr = (vdseFieldDef*) vdseMalloc( &pHashMap->memObject, 
                                     pHashMap->numFields* sizeof(vdseFieldDef),
                                     pContext );
   if ( ptr == NULL ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      return -1;
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

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapInsert( vdseMap            * pHashMap,
                   const void         * pKey,
                   size_t               keyLength, 
                   const void         * pItem,
                   size_t               itemLength,
                   vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL; //, * previousHashItem = NULL;
//   vdsErrors errcode = VDS_OK;
//   vdseTxStatus * txItemStatus, * txHashMapStatus;
//   int rc;
//   size_t bucket;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

//   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   listErr = vdseHashInsert( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pItem, 
                             itemLength,
                             &pHashItem,
                             pContext );
   if ( listErr != LIST_OK ) {
      if ( listErr == LIST_NO_MEMORY ) {
         vdscSetError( &pContext->errorHandler, 
            g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      }
      else {
         vdscSetError( &pContext->errorHandler, 
            g_vdsErrorHandle, VDS_ITEM_ALREADY_PRESENT );
      }
      return -1;
   }
      
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapRelease( vdseMap            * pHashMap,
                    vdseHashItem       * pHashItem,
                    vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   vdseMapReleaseNoLock( pHashMap,
                         pHashItem,
                         pContext );
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * This version of Release is only called internally, by the current
 * object. Always use the standard one when calling from the API.
 */
static
void vdseMapReleaseNoLock( vdseMap            * pHashMap,
                           vdseHashItem       * pHashItem,
                           vdseSessionContext * pContext )
{
   vdseTxStatus * txItemStatus, * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   txItemStatus = &pHashItem->txStatus;
   
   txItemStatus->usageCounter--;
   txHashMapStatus->usageCounter--;

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
      (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
         vdseHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMapReplace( vdseMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    const void         * pData,
                    size_t               dataLength,
                    vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdsErrors errcode = VDS_OK;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pData    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   listErr = vdseHashUpdate( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pData,
                             dataLength,
                             pContext );
   if ( listErr != LIST_OK ) {
      if ( listErr == LIST_KEY_NOT_FOUND ) {
         errcode = VDS_NO_SUCH_ITEM;
      }
      else if ( listErr == LIST_NO_MEMORY ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
      }
      else {
         errcode = VDS_INTERNAL_ERROR;
      }
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapRollbackAdd( vdseMap        * pHashMap, 
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
   if ( txItemStatus->usageCounter == 0 ) {
      errcode = vdseHashDelete( &pHashMap->hashObj, 
                                pHashItem->key,
                                pHashItem->keyLength, 
                                pHashItem,
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
                     (pHashMap->nodeObject.txCounter == 0 ) ) {
         if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
            vdseHashResize( &pHashMap->hashObj, pContext );
         }
      }
   }
   else {
      vdseTxStatusCommitRemove( txItemStatus );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapRollbackRemove( vdseMap            * pHashMap, 
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
   vdseTxStatusUnmarkAsDestroyed( txItemStatus );
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
      (pHashMap->nodeObject.txCounter == 0 ) ) {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
         vdseHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapStatus( vdseMap      * pHashMap,
                    vdsObjStatus * pStatus )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL;
   size_t     bucket;
   ptrdiff_t  firstItemOffset;
   vdseTxStatus  * txStatus;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pStatus  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   
   GET_PTR( txStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   pStatus->status = txStatus->enumStatus;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   listErr = vdseHashGetFirst( &pHashMap->hashObj, 
                               &bucket,
                               &firstItemOffset );
   while ( listErr == LIST_OK ) {
      GET_PTR( pHashItem, firstItemOffset, vdseHashItem );
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      listErr = vdseHashGetNext( &pHashMap->hashObj, 
                                 bucket,
                                 firstItemOffset,
                                 &bucket,
                                 &firstItemOffset );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

