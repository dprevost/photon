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

bool vdseMapCopy( vdseMap            * pOldMap, 
                  vdseMap            * pNewMap,
                  vdseHashItem       * pHashItem,
                  const char         * origName,
                  vdseSessionContext * pContext )
{
   int errcode;
   vdseFieldDef * oldDef, * newDef;
   
   VDS_PRE_CONDITION( pOldMap   != NULL );
   VDS_PRE_CONDITION( pNewMap   != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   
   errcode = vdseMemObjectInit( &pNewMap->memObject, 
                                VDSE_IDENT_MAP,
                                &pNewMap->blockGroup,
                                pOldMap->memObject.totalBlocks );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return false;
   }

   vdseTreeNodeInit( &pNewMap->nodeObject,
                     SET_OFFSET(&pHashItem->txStatus),
                     pOldMap->nodeObject.myNameLength,
                     SET_OFFSET(origName),
                     pOldMap->nodeObject.myParentOffset,
                     SET_OFFSET(pHashItem) );

   pNewMap->numFields = pOldMap->numFields;
   newDef = (vdseFieldDef *) vdseMalloc( &pNewMap->memObject, 
                                         pNewMap->numFields* sizeof(vdseFieldDef),
                                         pContext );
   if ( newDef == NULL ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      return false;
   }
   pNewMap->dataDefOffset = SET_OFFSET(newDef);
   oldDef = GET_PTR_FAST( pOldMap->dataDefOffset, vdseFieldDef );
   memcpy( newDef, oldDef, pNewMap->numFields* sizeof(vdseFieldDef) );
   memcpy( &pNewMap->keyDef, &pOldMap->keyDef, sizeof(vdsKeyDefinition) );

   errcode = vdseHashInit( &pNewMap->hashObj,
                           SET_OFFSET(&pNewMap->memObject),
                           pOldMap->hashObj.numberOfItems,
                           pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    errcode );
      return false;
   }

   errcode = vdseHashCopy( &pOldMap->hashObj, &pNewMap->hashObj, pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }
   pNewMap->latestVersion = pOldMap->latestVersion;
   pOldMap->editVersion = SET_OFFSET( pHashItem );
   pNewMap->editVersion = SET_OFFSET( pHashItem );
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapDelete( vdseMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    vdseSessionContext * pContext )
{
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   
   found = vdseHashDelWithKey( &pHashMap->hashObj, 
                               (unsigned char *)pKey,
                               keyLength,
                               pContext );
   if ( ! found ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NO_SUCH_ITEM );
      return false;
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

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapEmpty( vdseMap            * pHashMap,
                   vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   
   vdseHashEmpty( &pHashMap->hashObj, pContext );

   /*
    * Note: we do not check the return value of vdseHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
      vdseHashResize( &pHashMap->hashObj, pContext );
   }
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

bool vdseMapGet( vdseMap            * pHashMap,
                 const void         * pKey,
                 size_t               keyLength, 
                 vdseHashItem      ** ppHashItem,
                 size_t               bufferLength,
                 vdseSessionContext * pContext )
{
   vdseHashItem* pHashItem = NULL;
   vdsErrors errcode;
   vdseTxStatus * txHashMapStatus;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pKey       != NULL );
   VDS_PRE_CONDITION( ppHashItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( txHashMapStatus->status & VDSE_TXS_DESTROYED || 
      txHashMapStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
      errcode = VDS_OBJECT_IS_DELETED;
      goto the_exit;
   }
   
   found = vdseHashGet( &pHashMap->hashObj, 
                        (unsigned char *)pKey, 
                        keyLength,
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
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
      
   /*
    * We must increment the usage counter since we are passing back
    * a pointer to the data, not a copy. 
    */
   txHashMapStatus->usageCounter++;
   *ppHashItem = pHashItem;
   
   return true;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapGetFirst( vdseMap            * pHashMap,
                      vdseHashMapItem    * pItem,
                      size_t               keyLength,
                      size_t               bufferLength,
                      vdseSessionContext * pContext )
{
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus * txHashMapStatus;
   ptrdiff_t  firstItemOffset;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txHashMapStatus->status & VDSE_TXS_DESTROYED || 
      txHashMapStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_IS_DELETED );
      return false;
   }
   
   found = vdseHashGetFirst( &pHashMap->hashObj, 
                             &firstItemOffset );
   if ( ! found ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );
      return false;
   }
   
   GET_PTR( pHashItem, firstItemOffset, vdseHashItem );

   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   pItem->itemOffset = firstItemOffset;

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapGetNext( vdseMap            * pHashMap,
                     vdseHashMapItem    * pItem,
                     size_t               keyLength,
                     size_t               bufferLength,
                     vdseSessionContext * pContext )
{
   vdseHashItem * pHashItem = NULL;
   vdseHashItem * previousHashItem = NULL;
   vdseTxStatus * txHashMapStatus;
   ptrdiff_t  itemOffset;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != VDSE_NULL_OFFSET );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   if ( txHashMapStatus->status & VDSE_TXS_DESTROYED || 
      txHashMapStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_IS_DELETED );
      return false;
   }
   
   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   found = vdseHashGetNext( &pHashMap->hashObj, 
                            itemOffset,
                            &itemOffset );
   if ( ! found ) {
      /* 
       * If we come here, there are no additional data items to retrieve. As 
       * long as we clearly say that the internal iterator is reset (in case a 
       * "Get Previous" is implemented later), we can just release the iterator
       * at this point.
       */
      pItem->pHashItem = NULL;
      pItem->itemOffset = VDSE_NULL_OFFSET;
      vdseMapReleaseNoLock( pHashMap, previousHashItem, pContext );
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );
      return false;
   }
   
   GET_PTR( pHashItem, itemOffset, vdseHashItem );

   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   pItem->itemOffset = itemOffset;
   vdseMapReleaseNoLock( pHashMap, previousHashItem, pContext );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapInit( vdseMap             * pHashMap,
                  ptrdiff_t             parentOffset,
                  size_t                numberOfBlocks,
                  size_t                expectedNumOfItems,
                  vdseTxStatus        * pTxStatus,
                  size_t                origNameLength,
                  char                * origName,
                  ptrdiff_t             hashItemOffset,
                  vdsObjectDefinition * pDefinition,
                  vdseSessionContext  * pContext )
{
   vdsErrors errcode;
   vdseFieldDef * ptr;
   unsigned int i;
   
   VDS_PRE_CONDITION( pHashMap     != NULL );
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( pTxStatus    != NULL );
   VDS_PRE_CONDITION( origName     != NULL );
   VDS_PRE_CONDITION( pDefinition  != NULL );
   VDS_PRE_CONDITION( hashItemOffset != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( parentOffset   != VDSE_NULL_OFFSET );
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
      return false;
   }

   vdseTreeNodeInit( &pHashMap->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = vdseHashInit( &pHashMap->hashObj, 
                           SET_OFFSET(&pHashMap->memObject),
                           expectedNumOfItems, 
                           pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return false;
   }
   
   pHashMap->numFields = (uint16_t) pDefinition->numFields;

   ptr = (vdseFieldDef*) vdseMalloc( &pHashMap->memObject, 
                                     pHashMap->numFields* sizeof(vdseFieldDef),
                                     pContext );
   if ( ptr == NULL ) {
      vdscSetError( &pContext->errorHandler, 
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

   pHashMap->latestVersion = hashItemOffset;
   pHashMap->editVersion = VDSE_NULL_OFFSET;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapInsert( vdseMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    const void         * pItem,
                    size_t               itemLength,
                    vdseSessionContext * pContext )
{
   vdseHashItem* pHashItem = NULL;
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   errcode = vdseHashInsert( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pItem, 
                             itemLength,
                             &pHashItem,
                             pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }
      
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapRelease( vdseMap            * pHashMap,
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
   return true;
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
   vdseTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

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
   if ( txHashMapStatus->usageCounter == 0 ) {
      if ( pHashMap->hashObj.enumResize != VDSE_HASH_NO_RESIZE ) {
         vdseHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseMapReplace( vdseMap            * pHashMap,
                     const void         * pKey,
                     size_t               keyLength, 
                     const void         * pData,
                     size_t               dataLength,
                     vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pData    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );

   errcode = vdseHashUpdate( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pData,
                             dataLength,
                             pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMapStatus( vdseMap      * pHashMap,
                    vdsObjStatus * pStatus )
{
   vdseHashItem* pHashItem = NULL;
   ptrdiff_t  firstItemOffset;
   vdseTxStatus  * txStatus;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pStatus  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == VDSE_IDENT_MAP );
   
   GET_PTR( txStatus, pHashMap->nodeObject.txStatusOffset, vdseTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   found = vdseHashGetFirst( &pHashMap->hashObj, &firstItemOffset );
   while ( found ) {
      GET_PTR( pHashItem, firstItemOffset, vdseHashItem );
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      found = vdseHashGetNext( &pHashMap->hashObj, 
                               firstItemOffset,
                               &firstItemOffset );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

