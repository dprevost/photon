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
void psnMapReleaseNoLock( psnMap            * pHashMap,
                           psnHashItem       * pHashItem,
                           psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapCopy( psnMap            * pOldMap, 
                  psnMap            * pNewMap,
                  psnHashItem       * pHashItem,
                  const char         * origName,
                  psnSessionContext * pContext )
{
   int errcode;
   psnFieldDef * oldDef, * newDef;
   
   VDS_PRE_CONDITION( pOldMap   != NULL );
   VDS_PRE_CONDITION( pNewMap   != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   
   errcode = psnMemObjectInit( &pNewMap->memObject, 
                                PSN_IDENT_MAP,
                                &pNewMap->blockGroup,
                                pOldMap->memObject.totalBlocks );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return false;
   }

   psnTreeNodeInit( &pNewMap->nodeObject,
                     SET_OFFSET(&pHashItem->txStatus),
                     pOldMap->nodeObject.myNameLength,
                     SET_OFFSET(origName),
                     pOldMap->nodeObject.myParentOffset,
                     SET_OFFSET(pHashItem) );

   pNewMap->numFields = pOldMap->numFields;
   newDef = (psnFieldDef *) psnMalloc( &pNewMap->memObject, 
                                         pNewMap->numFields* sizeof(psnFieldDef),
                                         pContext );
   if ( newDef == NULL ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );
      return false;
   }
   pNewMap->dataDefOffset = SET_OFFSET(newDef);
   oldDef = GET_PTR_FAST( pOldMap->dataDefOffset, psnFieldDef );
   memcpy( newDef, oldDef, pNewMap->numFields* sizeof(psnFieldDef) );
   memcpy( &pNewMap->keyDef, &pOldMap->keyDef, sizeof(vdsKeyDefinition) );

   errcode = psnHashInit( &pNewMap->hashObj,
                           SET_OFFSET(&pNewMap->memObject),
                           pOldMap->hashObj.numberOfItems,
                           pContext );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    errcode );
      return false;
   }

   errcode = psnHashCopy( &pOldMap->hashObj, &pNewMap->hashObj, pContext );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }
   pNewMap->latestVersion = pOldMap->latestVersion;
   pOldMap->editVersion = SET_OFFSET( pHashItem );
   pNewMap->editVersion = SET_OFFSET( pHashItem );
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapDelete( psnMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    psnSessionContext * pContext )
{
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );
   
   found = psnHashDelWithKey( &pHashMap->hashObj, 
                               (unsigned char *)pKey,
                               keyLength,
                               pContext );
   if ( ! found ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NO_SUCH_ITEM );
      return false;
   }

   /*
    * Note: we do not check the return value of psnHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
      psnHashResize( &pHashMap->hashObj, pContext );
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnMapEmpty( psnMap            * pHashMap,
                   psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );
   
   psnHashEmpty( &pHashMap->hashObj, pContext );

   /*
    * Note: we do not check the return value of psnHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
      psnHashResize( &pHashMap->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnMapFini( psnMap        * pHashMap,
                      psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   psnHashFini( &pHashMap->hashObj );
   psnTreeNodeFini( &pHashMap->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   psnMemObjectFini(  &pHashMap->memObject, PSN_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapGet( psnMap            * pHashMap,
                 const void         * pKey,
                 size_t               keyLength, 
                 psnHashItem      ** ppHashItem,
                 size_t               bufferLength,
                 psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL;
   vdsErrors errcode;
   psnTxStatus * txHashMapStatus;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap   != NULL );
   VDS_PRE_CONDITION( pKey       != NULL );
   VDS_PRE_CONDITION( ppHashItem != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );
   
   if ( txHashMapStatus->status & PSN_TXS_DESTROYED || 
      txHashMapStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapGetFirst( psnMap            * pHashMap,
                      psnHashMapItem    * pItem,
                      size_t               keyLength,
                      size_t               bufferLength,
                      psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL;
   psnTxStatus * txHashMapStatus;
   ptrdiff_t  firstItemOffset;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( txHashMapStatus->status & PSN_TXS_DESTROYED || 
      txHashMapStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_IS_DELETED );
      return false;
   }
   
   found = psnHashGetFirst( &pHashMap->hashObj, 
                             &firstItemOffset );
   if ( ! found ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );
      return false;
   }
   
   GET_PTR( pHashItem, firstItemOffset, psnHashItem );

   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   pItem->itemOffset = firstItemOffset;

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapGetNext( psnMap            * pHashMap,
                     psnHashMapItem    * pItem,
                     size_t               keyLength,
                     size_t               bufferLength,
                     psnSessionContext * pContext )
{
   psnHashItem * pHashItem = NULL;
   psnHashItem * previousHashItem = NULL;
   psnTxStatus * txHashMapStatus;
   ptrdiff_t  itemOffset;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != PSN_NULL_OFFSET );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   if ( txHashMapStatus->status & PSN_TXS_DESTROYED || 
      txHashMapStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_IS_DELETED );
      return false;
   }
   
   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   found = psnHashGetNext( &pHashMap->hashObj, 
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
      pItem->itemOffset = PSN_NULL_OFFSET;
      psnMapReleaseNoLock( pHashMap, previousHashItem, pContext );
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );
      return false;
   }
   
   GET_PTR( pHashItem, itemOffset, psnHashItem );

   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      pscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   pItem->itemOffset = itemOffset;
   psnMapReleaseNoLock( pHashMap, previousHashItem, pContext );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapInit( psnMap             * pHashMap,
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
                                PSN_IDENT_MAP,
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

   pHashMap->latestVersion = hashItemOffset;
   pHashMap->editVersion = PSN_NULL_OFFSET;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapInsert( psnMap            * pHashMap,
                    const void         * pKey,
                    size_t               keyLength, 
                    const void         * pItem,
                    size_t               itemLength,
                    psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL;
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( itemLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   errcode = psnHashInsert( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pItem, 
                             itemLength,
                             &pHashItem,
                             pContext );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }
      
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapRelease( psnMap            * pHashMap,
                     psnHashItem       * pHashItem,
                     psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   psnMapReleaseNoLock( pHashMap,
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
void psnMapReleaseNoLock( psnMap            * pHashMap,
                           psnHashItem       * pHashItem,
                           psnSessionContext * pContext )
{
   psnTxStatus * txHashMapStatus;
   
   VDS_PRE_CONDITION( pHashMap  != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   txHashMapStatus->usageCounter--;

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
   if ( txHashMapStatus->usageCounter == 0 ) {
      if ( pHashMap->hashObj.enumResize != PSN_HASH_NO_RESIZE ) {
         psnHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnMapReplace( psnMap            * pHashMap,
                     const void         * pKey,
                     size_t               keyLength, 
                     const void         * pData,
                     size_t               dataLength,
                     psnSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pKey     != NULL )
   VDS_PRE_CONDITION( pData    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( keyLength  > 0 );
   VDS_PRE_CONDITION( dataLength > 0 );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );

   errcode = psnHashUpdate( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pData,
                             dataLength,
                             pContext );
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnMapStatus( psnMap      * pHashMap,
                    vdsObjStatus * pStatus )
{
   psnHashItem* pHashItem = NULL;
   ptrdiff_t  firstItemOffset;
   psnTxStatus  * txStatus;
   bool found;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pStatus  != NULL );
   VDS_PRE_CONDITION( pHashMap->memObject.objType == PSN_IDENT_MAP );
   
   GET_PTR( txStatus, pHashMap->nodeObject.txStatusOffset, psnTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   found = psnHashGetFirst( &pHashMap->hashObj, &firstItemOffset );
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

