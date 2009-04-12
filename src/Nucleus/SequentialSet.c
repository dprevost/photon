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

#include "Nucleus/SequentialSet.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetCopy( psonSeqSet         * pOldSet, 
                     psonSeqSet         * pNewSet,
                     psonHashTxItem     * pHashItem,
                     const char         * origName,
                     psonSessionContext * pContext )
{
   int errcode;
   
   PSO_PRE_CONDITION( pOldSet   != NULL );
   PSO_PRE_CONDITION( pNewSet   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( origName  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   
   errcode = psonMemObjectInit( &pNewSet->memObject, 
                                PSON_IDENT_MAP,
                                &pNewSet->blockGroup,
                                pOldSet->memObject.totalBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pNewSet->nodeObject,
                     SET_OFFSET(&pHashItem->txStatus),
                     pOldSet->nodeObject.myNameLength,
                     SET_OFFSET(origName),
                     pOldSet->nodeObject.myParentOffset,
                     SET_OFFSET(pHashItem) );
   
   errcode = psonHashInit( &pNewSet->hashObj,
                           SET_OFFSET(&pNewSet->memObject),
                           pOldSet->hashObj.numberOfItems,
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }

   pNewSet->dataDefOffset = pOldSet->dataDefOffset;
   pNewSet->keyDefOffset = pOldSet->keyDefOffset;
   pNewSet->flags = pOldSet->flags;
   
   errcode = psonHashCopy( &pOldSet->hashObj, &pNewSet->hashObj, pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }
   pNewSet->latestVersion = pOldSet->latestVersion;
   pOldSet->editVersion = SET_OFFSET( pHashItem );
   pNewSet->editVersion = SET_OFFSET( pHashItem );
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetDelete( psonSeqSet         * pSeqSet,
                       const void         * pKey,
                       uint32_t             keyLength, 
                       psonSessionContext * pContext )
{
   bool found;
   
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pKey     != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );
   
   found = psonHashDelWithKey( &pSeqSet->hashObj, 
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
   if ( pSeqSet->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pSeqSet->hashObj, pContext );
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonSeqSetEmpty( psonSeqSet         * pSeqSet,
                      psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );
   
   psonHashEmpty( &pSeqSet->hashObj, pContext );

   /*
    * Note: we do not check the return value of psonHashResize since the
    *       current function removes memory. It would make little sense
    *       to return "not enough memory", specially since the call 
    *       itself did work!
    */
   if ( pSeqSet->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pSeqSet->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonSeqSetFini( psonSeqSet         * pSeqSet,
                     psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );

   psonHashFini( &pSeqSet->hashObj );
   psonTreeNodeFini( &pSeqSet->nodeObject );
   
   /* 
    * Must be the last call since it will release the blocks of
    * memory to the allocator.
    */
   psonMemObjectFini(  &pSeqSet->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetGet( psonSeqSet         * pSeqSet,
                    const void         * pKey,
                    uint32_t             keyLength, 
                    psonHashItem      ** ppHashItem,
                    uint32_t             bufferLength,
                    psonSessionContext * pContext )
{
   psonHashItem* pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txSeqSetStatus;
   size_t bucket;
   bool found;
   
   PSO_PRE_CONDITION( pSeqSet   != NULL );
   PSO_PRE_CONDITION( pKey       != NULL );
   PSO_PRE_CONDITION( ppHashItem != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );

   GET_PTR( txSeqSetStatus, pSeqSet->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( txSeqSetStatus->status & PSON_TXS_DESTROYED || 
      txSeqSetStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      errcode = PSO_OBJECT_IS_DELETED;
      goto the_exit;
   }
   
   found = psonHashGet( &pSeqSet->hashObj, 
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
   txSeqSetStatus->usageCounter++;
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

bool psonSeqSetGetFirst( psonSeqSet         * pSeqSet,
                         psonSeqSetItem     * pItem,
                         uint32_t             keyLength,
                         uint32_t             bufferLength,
                         psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonTxStatus * txSeqSetStatus;
   bool found;
   
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );

   GET_PTR( txSeqSetStatus, pSeqSet->nodeObject.txStatusOffset, psonTxStatus );

   if ( txSeqSetStatus->status & PSON_TXS_DESTROYED || 
      txSeqSetStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_IS_DELETED );
      return false;
   }
   
   found = psonHashGetFirst( &pSeqSet->hashObj, 
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

   txSeqSetStatus->usageCounter++;
   pItem->pHashItem = pHashItem;

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetGetNext( psonSeqSet         * pSeqSet,
                        psonSeqSetItem     * pItem,
                        uint32_t             keyLength,
                        uint32_t             bufferLength,
                        psonSessionContext * pContext )
{
   psonHashItem * pHashItem = NULL;
   psonHashItem * previousHashItem = NULL;
   psonTxStatus * txSeqSetStatus;
   bool found;
   
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );
   PSO_PRE_CONDITION( pItem->pHashItem  != NULL );
   
   GET_PTR( txSeqSetStatus, pSeqSet->nodeObject.txStatusOffset, psonTxStatus );

   if ( txSeqSetStatus->status & PSON_TXS_DESTROYED || 
      txSeqSetStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_IS_DELETED );
      return false;
   }
   
   previousHashItem = pItem->pHashItem;
   
   found = psonHashGetNext( &pSeqSet->hashObj, 
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
      psonSeqSetReleaseNoLock( pSeqSet, previousHashItem, pContext );
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

   txSeqSetStatus->usageCounter++;
   pItem->pHashItem = pHashItem;
   psonSeqSetReleaseNoLock( pSeqSet, previousHashItem, pContext );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetInit( psonSeqSet          * pSeqSet,
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
   
   PSO_PRE_CONDITION( pSeqSet        != NULL );
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
   
   errcode = psonMemObjectInit( &pSeqSet->memObject, 
                                PSON_IDENT_MAP,
                                &pSeqSet->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pSeqSet->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = psonHashInit( &pSeqSet->hashObj, 
                           SET_OFFSET(&pSeqSet->memObject),
                           expectedNumOfItems, 
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }
   
   pSeqSet->dataDefOffset = SET_OFFSET(pDataDefinition);
   pSeqSet->keyDefOffset  = SET_OFFSET(pKeyDefinition);
   pSeqSet->latestVersion = hashItemOffset;
   pSeqSet->editVersion   = PSON_NULL_OFFSET;
   pSeqSet->flags = pDefinition->flags;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetInsert( psonSeqSet         * pSeqSet,
                       const void         * pKey,
                       uint32_t             keyLength, 
                       const void         * pItem,
                       uint32_t             itemLength,
                       psonDataDefinition * pDefinition,
                       psonSessionContext * pContext )
{
   psoErrors errcode;
   psonHashItem * pHashItem = NULL;
   
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( itemLength > 0 );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );

   errcode = psonHashInsert( &pSeqSet->hashObj, 
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

void psonSeqSetRelease( psonSeqSet         * pSeqSet,
                        psonHashItem       * pHashItem,
                        psonSessionContext * pContext )
{
   psonTxStatus * txSeqSetStatus;
   
   PSO_PRE_CONDITION( pSeqSet  != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );

   GET_PTR( txSeqSetStatus, pSeqSet->nodeObject.txStatusOffset, psonTxStatus );

   txSeqSetStatus->usageCounter--;

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txSeqSetStatus->usageCounter someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    *
    * Note: we do not check the return value of psonHashResize since the
    *       current function returns void. Let's someone else find that 
    *       we are getting low on memory...
    */
   if ( txSeqSetStatus->usageCounter == 0 ) {
      if ( pSeqSet->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
         psonHashResize( &pSeqSet->hashObj, pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonSeqSetReplace( psonSeqSet        * pSeqSet,
                         const void         * pKey,
                         uint32_t             keyLength, 
                         const void         * pData,
                         uint32_t             dataLength,
                         psonDataDefinition * pDefinition,
                         psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   psonHashItem * pHashItem = NULL;

   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pKey     != NULL )
   PSO_PRE_CONDITION( pData    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( keyLength  > 0 );
   PSO_PRE_CONDITION( dataLength > 0 );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );

   errcode = psonHashUpdate( &pSeqSet->hashObj, 
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

void psonSeqSetStatus( psonSeqSet  * pSeqSet,
                        psoObjStatus * pStatus )
{
   psonHashItem * pHashItem = NULL;
   psonTxStatus  * txStatus;
   bool found;
   
   PSO_PRE_CONDITION( pSeqSet != NULL );
   PSO_PRE_CONDITION( pStatus  != NULL );
   PSO_PRE_CONDITION( pSeqSet->memObject.objType == PSON_IDENT_MAP );
   
   GET_PTR( txStatus, pSeqSet->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pSeqSet->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
   if ( pStatus->numDataItem == 0 ) return;

   found = psonHashGetFirst( &pSeqSet->hashObj, &pHashItem );
   while ( found ) {
      if ( pHashItem->dataLength > pStatus->maxDataLength ) {
         pStatus->maxDataLength = pHashItem->dataLength;
      }
      if ( pHashItem->keyLength > pStatus->maxKeyLength ) {
         pStatus->maxKeyLength = pHashItem->keyLength;
      }

      found = psonHashGetNext( &pSeqSet->hashObj, 
                               pHashItem,
                               &pHashItem );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

