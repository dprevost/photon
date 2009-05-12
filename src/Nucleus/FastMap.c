/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/FastMap.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declaration of static functions */

static
void psonFastMapReleaseNoLock( psonFastMap        * pHashMap,
                               psonHashItem       * pHashItem,
                               psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapCopy( psonFastMap        * pOldMap, 
                      psonFastMap        * pNewMap,
                      psonHashTxItem     * pHashItem,
                      const char         * origName,
                      psonSessionContext * pContext )
{
   int errcode;
   
   PSO_PRE_CONDITION( pOldMap   != NULL );
   PSO_PRE_CONDITION( pNewMap   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( origName  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   
   errcode = psonMemObjectInit( &pNewMap->memObject, 
                                PSON_IDENT_MAP,
                                &pNewMap->blockGroup,
                                pOldMap->memObject.totalBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pNewMap->nodeObject,
                     SET_OFFSET(&pHashItem->txStatus),
                     pOldMap->nodeObject.myNameLength,
                     SET_OFFSET(origName),
                     pOldMap->nodeObject.myParentOffset,
                     SET_OFFSET(pHashItem) );
   
   errcode = psonHashInit( &pNewMap->hashObj,
                           SET_OFFSET(&pNewMap->memObject),
                           pOldMap->hashObj.numberOfItems,
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }

   pNewMap->dataDefOffset = pOldMap->dataDefOffset;
   pNewMap->keyDefOffset = pOldMap->keyDefOffset;
   pNewMap->flags = pOldMap->flags;
   
   errcode = psonHashCopy( &pOldMap->hashObj, &pNewMap->hashObj, pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }
   pNewMap->latestVersion = pOldMap->latestVersion;
   pOldMap->editVersion = SET_OFFSET( pHashItem );
   pNewMap->editVersion = SET_OFFSET( pHashItem );
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapDelete( psonFastMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength, 
                        psonSessionContext * pContext )
{
   bool found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   
   found = psonHashDelWithKey( &pHashMap->hashObj, 
                               (unsigned char *)pKey,
                               keyLength,
                               pContext );
   if ( ! found ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NO_SUCH_ITEM );
      return false;
   }

   /*
    * Note: we do not check the return value of psonHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pHashMap->hashObj, pContext );
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapEmpty( psonFastMap        * pHashMap,
                       psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   
   psonHashEmpty( &pHashMap->hashObj, pContext );

   /*
    * Note: we do not check the return value of psonHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pHashMap->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapFini( psonFastMap        * pHashMap,
                      psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   psonHashFini( &pHashMap->hashObj );
   psonTreeNodeFini( &pHashMap->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   psonMemObjectFini(  &pHashMap->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapGet( psonFastMap        * pHashMap,
                     const void         * pKey,
                     uint32_t             keyLength, 
                     psonHashItem      ** ppHashItem,
                     uint32_t             bufferLength,
                     psonSessionContext * pContext )
{
   psonHashItem* pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txHashMapStatus;
   size_t bucket;
   bool found;
   
   PSO_PRE_CONDITION( pHashMap   != NULL );
   PSO_PRE_CONDITION( pKey       != NULL );
   PSO_PRE_CONDITION( ppHashItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( txHashMapStatus->status & PSON_TXS_DESTROYED || 
      txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
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

   /*
    * This test cannot be done in the API (before calling the current
    * function) since we do not know the item size. It could be done
    * after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      errcode = PSO_INVALID_LENGTH;
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
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapGetFirst( psonFastMap        * pHashMap,
                          psonFastMapItem    * pItem,
                          uint32_t             keyLength,
                          uint32_t             bufferLength,
                          psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonTxStatus * txHashMapStatus;
   bool found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( txHashMapStatus->status & PSON_TXS_DESTROYED || 
      txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_IS_DELETED );
      return false;
   }
   
   found = psonHashGetFirst( &pHashMap->hashObj, 
                             &pHashItem );
   if ( ! found ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );
      return false;
   }
   
   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapGetNext( psonFastMap        * pHashMap,
                         psonFastMapItem    * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonHashItem * previousHashItem = NULL;
   psonTxStatus * txHashMapStatus;
   bool found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   PSO_PRE_CONDITION( pItem->pHashItem  != NULL );
   
   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   if ( txHashMapStatus->status & PSON_TXS_DESTROYED || 
      txHashMapStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_IS_DELETED );
      return false;
   }
   
   previousHashItem = pItem->pHashItem;
   
   found = psonHashGetNext( &pHashMap->hashObj, 
                            previousHashItem,
                            &pHashItem );
   if ( ! found ) {
      /* 
       * If we come here, there are no additional data items to retrieve. As 
       * long as we clearly say that the internal iterator is reset (in case a 
       * "Get Previous" is implemented later), we can just release the iterator
       * at this point.
       */
      pItem->pHashItem = NULL;
      psonFastMapReleaseNoLock( pHashMap, previousHashItem, pContext );
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );
      return false;
   }
   
   /*
    * These tests cannot be done in the API (before calling the 
    * current function) since we do not know the item size. They 
    * could be done after but this way makes the code faster.
    */
   if ( bufferLength < pHashItem->dataLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      return false;
   }
   if ( keyLength < pHashItem->keyLength ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      return false;
   }

   txHashMapStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   psonFastMapReleaseNoLock( pHashMap, previousHashItem, pContext );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapInit( psonFastMap         * pHashMap,
                      ptrdiff_t             parentOffset,
                      size_t                numberOfBlocks,
                      size_t                expectedNumOfItems,
                      psonTxStatus        * pTxStatus,
                      uint32_t              origNameLength,
                      char                * origName,
                      ptrdiff_t             hashItemOffset,
                      psoObjectDefinition * pDefinition,
                      psonKeyDefinition   * pKeyDefinition,
                      psonDataDefinition  * pDataDefinition,
                      psonSessionContext  * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pHashMap        != NULL );
   PSO_PRE_CONDITION( pContext        != NULL );
   PSO_PRE_CONDITION( pTxStatus       != NULL );
   PSO_PRE_CONDITION( origName        != NULL );
   PSO_PRE_CONDITION( pDefinition     != NULL );
   PSO_PRE_CONDITION( pKeyDefinition  != NULL );
   PSO_PRE_CONDITION( pDataDefinition != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   
   errcode = psonMemObjectInit( &pHashMap->memObject, 
                                PSON_IDENT_MAP,
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
   
   pHashMap->dataDefOffset = SET_OFFSET(pDataDefinition);
   pHashMap->keyDefOffset  = SET_OFFSET(pKeyDefinition);
   pHashMap->latestVersion = hashItemOffset;
   pHashMap->editVersion   = PSON_NULL_OFFSET;
   pHashMap->flags = pDefinition->flags;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapInsert( psonFastMap        * pHashMap,
                        const void         * pKey,
                        uint32_t             keyLength, 
                        const void         * pItem,
                        uint32_t             itemLength,
                        psonDataDefinition * pDefinition,
                        psonSessionContext * pContext )
{
   psoErrors errcode;
   psonHashItem * pHashItem = NULL;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( itemLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   errcode = psonHashInsert( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pItem, 
                             itemLength,
                             &pHashItem,
                             pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }
   if ( pDefinition == NULL ) {
      pHashItem->dataDefOffset = PSON_NULL_OFFSET;
   }
   else {
      pHashItem->dataDefOffset = SET_OFFSET(pDefinition);
   }
      
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapRelease( psonFastMap        * pHashMap,
                         psonHashItem       * pHashItem,
                         psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pHashMap  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   psonFastMapReleaseNoLock( pHashMap,
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
void psonFastMapReleaseNoLock( psonFastMap        * pHashMap,
                               psonHashItem       * pHashItem,
                               psonSessionContext * pContext )
{
   psonTxStatus * txHashMapStatus;
   
   PSO_PRE_CONDITION( pHashMap  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   GET_PTR( txHashMapStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   txHashMapStatus->usageCounter--;

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
   if ( txHashMapStatus->usageCounter == 0 ) {
      if ( pHashMap->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
         psonHashResize( &pHashMap->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFastMapReplace( psonFastMap        * pHashMap,
                         const void         * pKey,
                         uint32_t             keyLength, 
                         const void         * pData,
                         uint32_t             dataLength,
                         psonDataDefinition * pDefinition,
                         psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   psonHashItem * pHashItem = NULL;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pData    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( dataLength > 0 );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );

   errcode = psonHashUpdate( &pHashMap->hashObj, 
                             (unsigned char *)pKey, 
                             keyLength,
                             pData,
                             dataLength,
                             &pHashItem,
                             pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }
   if ( pDefinition == NULL ) {
      pHashItem->dataDefOffset = PSON_NULL_OFFSET;
   }
   else {
      pHashItem->dataDefOffset = SET_OFFSET(pDefinition);
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFastMapStatus( psonFastMap  * pHashMap,
                        psoObjStatus * pStatus )
{
   psonHashItem * pHashItem = NULL;
   psonTxStatus  * txStatus;
   bool found;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pStatus  != NULL );
   PSO_PRE_CONDITION( pHashMap->memObject.objType == PSON_IDENT_MAP );
   
   GET_PTR( txStatus, pHashMap->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pHashMap->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   found = psonHashGetFirst( &pHashMap->hashObj, &pHashItem );
   while ( found ) {
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      found = psonHashGetNext( &pHashMap->hashObj, 
                               pHashItem,
                               &pHashItem );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

