/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/Folder.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/FastMap.h"
#include "Nucleus/Queue.h"
#include "Nucleus/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* isRemoved can be NULL */
static
void psonFolderReleaseNoLock( psonFolder         * pFolder,
                              psonHashTxItem     * pHashItemItem,
                              bool               * isRemoved,
                              psonSessionContext * pContext );

static 
psoErrors psonValidateString( const char * objectName,
                              uint32_t     strLength, 
                              uint32_t   * pPartialLength,
                              bool       * pLastIteration );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The new object created by this function is a child of the folder 
 */
bool psonAPIFolderCreateFolder( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;
   psoObjectDefinition definition = { PSO_FOLDER, 0, 0, 0 };
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( name[i] == '/' || name[i] == '\\' ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock(&pFolder->memObject, pContext) ) {
      ok = psonFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   &definition,
                                   NULL,
                                   NULL,
                                   1, /* numBlocks, */
                                   0, /* expectedNumOfChilds, */
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

/*
 * The new object created by this function is a child of the folder 
 */
bool psonAPIFolderCreateObject( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                psonDataDefinition  * pDataDefinition,
                                psonKeyDefinition   * pKeyDefinition,
                                psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pDefinition->type > 0 && 
                      pDefinition->type < PSO_LAST_OBJECT_TYPE );
   if ( pDefinition->type != PSO_FOLDER ) {
      PSO_PRE_CONDITION( pDataDefinition  != NULL );
   }
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( name[i] == '/' || name[i] == '\\' ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock(&pFolder->memObject, pContext) ) {
      ok = psonFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   pDefinition,
                                   pDataDefinition,
                                   pKeyDefinition,
                                   1, /* numBlocks, */
                                   0, /* expectedNumOfChilds, */
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

bool psonAPIFolderDestroyObject( psonFolder         * pFolder,
                                 const char         * objectName,
                                 uint32_t             nameLengthInBytes,
                                 psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( name[i] == '/' || name[i] == '\\' ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock(&pFolder->memObject, pContext) ) {
      ok = psonFolderDeleteObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength,
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

bool psonAPIFolderGetDefinition( psonFolder          * pFolder,
                                 const char          * objectName,
                                 uint32_t              strLength,
                                 psoObjectDefinition * pDefinition,
                                 psonDataDefinition ** ppDataDefinition,
                                 psonKeyDefinition  ** ppKeyDefinition,
                                 psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   bool ok;
   char * lowerName = NULL;
   int i;
   
   PSO_PRE_CONDITION( pFolder          != NULL );
   PSO_PRE_CONDITION( objectName       != NULL )
   PSO_PRE_CONDITION( pDefinition      != NULL );
   PSO_PRE_CONDITION( ppKeyDefinition  != NULL );
   PSO_PRE_CONDITION( ppDataDefinition != NULL );
   PSO_PRE_CONDITION( pContext         != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   if ( strLength > PSO_MAX_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( objectName[i] == '/' || objectName[i] == '\\' ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( objectName[i] );
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderGetDefinition() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock(&pFolder->memObject, pContext) ) {
      ok = psonFolderGetDefinition( pFolder,
                                    lowerName, 
                                    strLength, 
                                    pDefinition,
                                    ppDataDefinition,
                                    ppKeyDefinition,
                                    pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

bool psonAPIFolderGetFirst( psonFolder         * pFolder,
                            psonFolderItem     * pItem,
                            psonSessionContext * pContext )
{
   psonHashTxItem* pHashItem = NULL;
   psonTxStatus * txItemStatus;
   psonTxStatus * txFolderStatus;
   ptrdiff_t  firstItemOffset;
   bool found;

   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );

   if ( psonLock( &pFolder->memObject, pContext ) ) {
      /*
       * We loop on all data items until we find one which is visible to the
       * current session (its transaction field equal to zero or to our 
       * transaction) AND is not marked as destroyed.
       */
      found = psonHashTxGetFirst( &pFolder->hashObj, &firstItemOffset );
      while ( found ) {
         GET_PTR( pHashItem, firstItemOffset, psonHashTxItem );
         txItemStatus = &pHashItem->txStatus;

        if ( psonTxTestObjectStatus( txItemStatus, 
            SET_OFFSET(pContext->pTransaction) ) == PSO_OK ) {

            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = firstItemOffset;
            pItem->status = txItemStatus->status;
            
            psonUnlock( &pFolder->memObject, pContext );
            
            return true;
         }
  
         found = psonHashTxGetNext( &pFolder->hashObj, 
                                  firstItemOffset,
                                  &firstItemOffset );
      }
      
      psonUnlock( &pFolder->memObject, pContext );
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );

      return false;
   }

   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );

   return false;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonAPIFolderGetNext( psonFolder         * pFolder,
                           psonFolderItem     * pItem,
                           psonSessionContext * pContext )
{
   psonHashTxItem * pHashItem = NULL;
   psonHashTxItem * previousHashItem = NULL;
   psonTxStatus * txItemStatus;
   psonTxStatus * txFolderStatus;
   ptrdiff_t  itemOffset;
   bool found;

   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );
   PSO_PRE_CONDITION( pItem->pHashItem  != NULL );
   PSO_PRE_CONDITION( pItem->itemOffset != PSON_NULL_OFFSET );
   
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );

   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      /*
       * We loop on all data items until we find one which is visible to the
       * current session (its transaction field equal to zero or to our 
       * transaction) AND is not marked as destroyed.
       */
      found = psonHashTxGetNext( &pFolder->hashObj, 
                               itemOffset,
                               &itemOffset );
      while ( found ) {
         GET_PTR( pHashItem, itemOffset, psonHashTxItem );
         txItemStatus = &pHashItem->txStatus;

         if ( psonTxTestObjectStatus( txItemStatus, 
            SET_OFFSET(pContext->pTransaction) ) == PSO_OK ) {
 
            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = itemOffset;
            pItem->status = txItemStatus->status;

            psonFolderReleaseNoLock( pFolder, previousHashItem, NULL, pContext );

            psonUnlock( &pFolder->memObject, pContext );
            
            return true;
         }
  
         found = psonHashTxGetNext( &pFolder->hashObj, 
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
   psonFolderReleaseNoLock( pFolder, previousHashItem, NULL, pContext );
    
   psonUnlock( &pFolder->memObject, pContext );
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonAPIFolderStatus( psonFolder   * pFolder,
                          psoObjStatus * pStatus )
{
   psonTxStatus  * txStatus;

   PSO_PRE_CONDITION( pFolder != NULL );
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   GET_PTR( txStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pFolder->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
}

/* End of psonAPIFolder functions */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFolderCommitEdit( psonFolder         * pFolder,
                           psonHashTxItem     * pHashItem, 
                           enum psoObjectType   objectType,
                           psonMemObject     ** ppOldMemObj,
                           psonSessionContext * pContext )
{
   psonObjectDescriptor * pDesc, * pDescLatest;
   psonFastMap * pMapLatest, * pMapEdit;
   psonHashTxItem * pHashItemLatest;
   psonTreeNode * node;
   psonTxStatus * tx;
   bool isRemoved = false;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pHashItem   != NULL );
   PSO_PRE_CONDITION( ppOldMemObj != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType == PSON_IDENT_MAP );

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );

   /* The edit version which is about to become the latest */
   pMapEdit = GET_PTR_FAST( pDesc->offset, psonFastMap );

   if ( pMapEdit->hashObj.enumResize != PSON_HASH_NO_RESIZE ) {
      psonHashResize( &pMapEdit->hashObj, pContext );
   }
   
   PSO_INV_CONDITION( pMapEdit->editVersion == SET_OFFSET(pHashItem) );
   
   pHashItemLatest = GET_PTR_FAST( pMapEdit->latestVersion, psonHashTxItem );
   pDescLatest = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                               psonObjectDescriptor );
   /* The current latest which is about to become old. */
   pMapLatest = GET_PTR_FAST( pDescLatest->offset, psonFastMap );

   pHashItemLatest->nextSameKey = SET_OFFSET(pHashItem);
   pMapLatest->editVersion = PSON_NULL_OFFSET;
   pMapEdit->editVersion   = PSON_NULL_OFFSET;
   pMapLatest->latestVersion = SET_OFFSET(pHashItem);
   pMapEdit->latestVersion = SET_OFFSET(pHashItem);

   node = GET_PTR_FAST( pDesc->nodeOffset, psonTreeNode );
   tx = GET_PTR_FAST( node->txStatusOffset, psonTxStatus );

   psonTxStatusCommitEdit( &pHashItemLatest->txStatus, tx );

   /* Reminder: pHashItemLatest is now the old version */
   psonFolderReleaseNoLock( pFolder,
                            pHashItemLatest,
                            &isRemoved,
                            pContext );
   if ( isRemoved ) *ppOldMemObj = &pMapLatest->memObject; /* The old copy */
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
bool psonFolderDeletable( psonFolder         * pFolder,
                          psonSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   psonHashTxItem * pItem;
   ptrdiff_t txOffset = SET_OFFSET( pContext->pTransaction );
   bool found;
   
   /* The easy case */
   if ( pFolder->hashObj.numberOfItems == 0 ) return true;
   
   /*
    * We loop on all the hash items until either:
    * - we find one item which is not marked as deleted by the
    *   current transaction (we return false)
    * - or the end (we return true)
    */
   
   found = psonHashTxGetFirst( &pFolder->hashObj, &offset );
   while ( found ) {
      GET_PTR( pItem, offset, psonHashTxItem );
      if ( pItem->txStatus.txOffset != txOffset ) return false;
      if ( ! psonTxStatusIsMarkedAsDestroyed( &pItem->txStatus ) ) {
         return false;
      }
      
      previousOffset = offset;
      
      found = psonHashTxGetNext( &pFolder->hashObj,
                               previousOffset,
                               &offset );
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderDeleteObject( psonFolder         * pFolder,
                             const char         * objectName,
                             uint32_t             strLength, 
                             psonSessionContext * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psoErrors errcode = PSO_OK;
   psonObjectDescriptor* pDesc = NULL;
   psonHashTxItem* pHashItem = NULL;
   psonTxStatus* txStatus;
   psonFolder * pNextFolder, *pDeletedFolder;
   psonMemObject * pMemObj;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;

   found = psonHashTxGet( &pFolder->hashObj, 
                          (unsigned char *)objectName, 
                          partialLength * sizeof(char),
                          &pHashItem,
                          &bucket,
                          pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }

   txStatus = &pHashItem->txStatus;
   
   if ( lastIteration ) {
      if ( pFolder->isSystemObject ) {
         errcode = PSO_SYSTEM_OBJECT;
         goto the_exit;
      }

      /* 
       * If the transaction id of the object is non-zero, a big no-no - 
       * we do not support two transactions on the same data
       * (and if remove is committed - the data is "non-existent").
       */
      if ( txStatus->txOffset != PSON_NULL_OFFSET ) {
         if ( txStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_NO_SUCH_OBJECT;
         }
         else {
            errcode = PSO_OBJECT_IS_IN_USE;
         }
         goto the_exit;
      }

      GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
      
      /*
       * A special case - folders cannot be deleted if they are not empty
       * (unless all other objects are marked as deleted by the current
       * transaction).
       *
       * Other objects contain data, not objects. 
       */
      if ( pDesc->apiType == PSO_FOLDER ) {
         GET_PTR( pDeletedFolder, pDesc->offset, psonFolder );
         if ( ! psonFolderDeletable( pDeletedFolder, pContext ) ) {
            errcode = PSO_FOLDER_IS_NOT_EMPTY;
            goto the_exit;
         }
      }

      GET_PTR( pMemObj, pDesc->memOffset, psonMemObject );
      
      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_REMOVE_OBJECT,
                         SET_OFFSET(pFolder),
                         PSON_IDENT_FOLDER,
                         SET_OFFSET(pHashItem),
                         pMemObj->objType,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto the_exit;
      
      psonTxStatusSetTx( txStatus, SET_OFFSET(pContext->pTransaction) );
      psonTxStatusMarkAsDestroyed( txStatus );
      pFolder->nodeObject.txCounter++;
      
      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }

   /* If we come here, this was not the last iteration, so we continue */

   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   /* 
    * If the transaction id of the next folder is not either equal to
    * zero or to the current transaction id, then it belongs to 
    * another transaction - uncommitted. For this transaction it is
    * as if it does not exist.
    * Similarly, if we are marked as destroyed... can't access that folder
    * (to have the id as ok and be marked as destroyed is a rare case - 
    * it would require that the current transaction deleted the folder and 
    * than tries to access it).
    */
   if ( ! psonTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction)) 
        || psonTxStatusIsMarkedAsDestroyed( txStatus ) ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );
   if ( ! psonLock(&pNextFolder->memObject, pContext) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   psonUnlock( &pFolder->memObject, pContext );

   ok = psonFolderDeleteObject( pNextFolder,
                                &objectName[partialLength+1],
                                strLength - partialLength - 1,
                                pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   return ok;
   
the_exit:

   /* psocSetError might have been already called by some other function */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderEditObject( psonFolder         * pFolder,
                           const char         * objectName,
                           uint32_t             strLength,
                           enum psoObjectType   objectType,
                           psonFolderItem     * pFolderItem,
                           psonSessionContext * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   size_t bucket = 0, descLength;
   psonObjectDescriptor * pDescOld = NULL, * pDescNew = NULL;
   psonHashTxItem * pHashItemOld = NULL, * pHashItemNew = NULL;
   psoErrors errcode;
   psonTxStatus * txStatus, * newTxStatus;
   psonTxStatus * txFolderStatus;
   psonFolder * pNextFolder;
   psonMemObject * pMemObject;
   unsigned char * ptr;
   psonMemObjIdent memObjType = PSON_IDENT_LAST;
   psonFastMap * pMap;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL )
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psonHashTxGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItemOld,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItemOld->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItemOld, pHashItemOld->nextSameKey, psonHashTxItem );
   }

   txStatus = &pHashItemOld->txStatus;

   GET_PTR( pDescOld, pHashItemOld->dataOffset, psonObjectDescriptor );
   
   if ( lastIteration ) {
      if ( pFolder->isSystemObject ) {
         errcode = PSO_SYSTEM_OBJECT;
         goto the_exit;
      }
      GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );
      /* 
       * If the transaction id of the object (to open) is equal to the 
       * current transaction id AND the object is marked as deleted... error.
       *
       * If the transaction id of the object (to retrieve) is NOT equal to the 
       * current transaction id AND the object is added... error.
       *
       * If the object is flagged as deleted and committed, it does not exists
       * from the API point of view.
       */
      if ( txStatus->txOffset != PSON_NULL_OFFSET ) {
         if ( txStatus->status & PSON_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_NO_SUCH_OBJECT;
            goto the_exit;
         }
         if ( txStatus->status & PSON_TXS_EDIT ) {
            errcode = PSO_A_SINGLE_UPDATER_IS_ALLOWED;
            goto the_exit;
         }
         if ( txStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
            if ( txStatus->status & PSON_TXS_DESTROYED ) {
               errcode = PSO_OBJECT_IS_DELETED;
               goto the_exit;
            }
            else if ( ! (txStatus->status & PSON_TXS_ADDED) ) {
               errcode = PSO_OBJECT_IS_IN_USE;
               goto the_exit;
            }
         }
         else {
            errcode = PSO_OBJECT_IS_IN_USE;
            goto the_exit;
         }
      }
      if ( objectType != pDescOld->apiType ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto the_exit;
      }

      switch( pDescOld->apiType ) {
      case PSO_FAST_MAP:
         pMap = GET_PTR_FAST( pDescOld->offset, psonFastMap );
         if ( pMap->editVersion != PSON_NULL_OFFSET ) {
            errcode = PSO_A_SINGLE_UPDATER_IS_ALLOWED;
            goto the_exit;
         }
         memObjType = PSON_IDENT_MAP;
         break;
      default:
         errcode = PSO_INTERNAL_ERROR;
         goto the_exit;
      }

      pMemObject = GET_PTR_FAST( pDescOld->memOffset, psonMemObject );
      
      ptr = (unsigned char*) psonMallocBlocks( pContext->pAllocator,
                                               PSON_ALLOC_API_OBJ,
                                               pMemObject->totalBlocks,
                                               pContext );
      if ( ptr == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
      
      descLength = offsetof(psonObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDescNew = (psonObjectDescriptor *) malloc( descLength );
      if ( pDescNew == NULL ) {
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memcpy( pDescNew, pDescOld, descLength );
      pDescNew->offset = SET_OFFSET( ptr );

      errcode = psonHashTxInsert( &pFolder->hashObj, 
                                  bucket,
                                  (unsigned char *)objectName, 
                                  partialLength * sizeof(char), 
                                  (void*)pDescNew, 
                                  descLength,
                                  &pHashItemNew,
                                  pContext );
      free( pDescNew );
      pDescNew = NULL;
      if ( errcode != PSO_OK ) {
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         free( pDescNew );
         goto the_exit;
      }

      pDescNew = GET_PTR_FAST(pHashItemNew->dataOffset, psonObjectDescriptor);

      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_EDIT_OBJECT,
                         SET_OFFSET(pFolder),
                         PSON_IDENT_FOLDER,
                         SET_OFFSET(pHashItemNew),
                         memObjType,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psonHashTxDelete( &pFolder->hashObj, 
                           pHashItemNew,
                           pContext );
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         goto the_exit;
      }
      
      newTxStatus = &pHashItemNew->txStatus;
      psonTxStatusInit( newTxStatus, SET_OFFSET(pContext->pTransaction) );
      txStatus->txOffset = SET_OFFSET(pContext->pTransaction);
      txStatus->status |= PSON_TXS_EDIT;
      newTxStatus->status = txStatus->status;
      newTxStatus->txOffset = SET_OFFSET(pContext->pTransaction);
      
      switch ( memObjType ) {
      case PSON_IDENT_MAP:
         ok = psonFastMapCopy( pMap, /* old, */
                               (psonFastMap *)ptr,
                               pHashItemNew,
                               pDescNew->originalName,
                               pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         pDescNew->nodeOffset = SET_OFFSET(ptr) + offsetof(psonFastMap,nodeObject);
         pDescNew->memOffset  = SET_OFFSET(ptr) + offsetof(psonFastMap,memObject);
         break;

      default:
         errcode = PSO_INTERNAL_ERROR;
         goto the_exit;
      }

      if ( ! ok ) {
         psonTxRemoveLastOps( (psonTx*)pContext->pTransaction, pContext );
         psonHashTxDelete( &pFolder->hashObj, 
                           pHashItemNew,
                           pContext );
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         goto the_exit;
      }

      pFolder->nodeObject.txCounter++;
      txFolderStatus->usageCounter++;
      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItemNew;

      pHashItemNew->txStatus.parentCounter = 1;
      pHashItemNew->txStatus.usageCounter = 0;

      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDescOld->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psonTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDescOld->offset, psonFolder );
   if ( ! psonLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psonUnlock( &pFolder->memObject, pContext );
     
   ok = psonFolderEditObject( pNextFolder,
                              &objectName[partialLength+1], 
                              strLength - partialLength - 1, 
                              objectType,
                              pFolderItem,
                              pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFolderFini( psonFolder         * pFolder,
                     psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   psonHashTxFini( &pFolder->hashObj );
   psonTreeNodeFini( &pFolder->nodeObject );
   
   /* This call must be last - put a barrier here ? */ 
   psonMemObjectFini(  &pFolder->memObject, PSON_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderFindObject( psonFolder         * pFolder,
                           const char         * objectName,
                           uint32_t             strLength,
                           psonHashTxItem    ** ppFoundItem,
                           psonSessionContext * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psoErrors errcode = PSO_OK;
   psonObjectDescriptor* pDesc = NULL;
   psonHashTxItem* pHashItem = NULL;
   psonTxStatus* txStatus;
   psonFolder * pNextFolder;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( ppFoundItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;

   found = psonHashTxGet( &pFolder->hashObj, 
                          (unsigned char *)objectName, 
                          partialLength * sizeof(char),
                          &pHashItem,
                          &bucket,
                          pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }
   
   if ( lastIteration ) {
      if ( pFolder->isSystemObject ) {
         errcode = PSO_SYSTEM_OBJECT;
         goto the_exit;
      }

      *ppFoundItem = pHashItem;      

      return true;
   }

   /* If we come here, this was not the last iteration, so we continue */

   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   /* 
    * If the transaction id of the next folder is not either equal to
    * zero or to the current transaction id, then it belongs to 
    * another transaction - uncommitted. For this transaction it is
    * as if it does not exist.
    * Similarly, if we are marked as destroyed... can't access that folder
    * (to have the id as ok and be marked as destroyed is a rare case - 
    * it would require that the current transaction deleted the folder and 
    * than tries to access it).
    */
   txStatus = &pHashItem->txStatus;

   if ( ! psonTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction)) 
        || psonTxStatusIsMarkedAsDestroyed( txStatus ) ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );
   if ( ! psonLock(&pNextFolder->memObject, pContext) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   psonUnlock( &pFolder->memObject, pContext );

   ok = psonFolderFindObject( pNextFolder,
                               &objectName[partialLength+1],
                               strLength - partialLength - 1,
                               ppFoundItem,
                               pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   return ok;
   
the_exit:

   /* psocSetError might have been already called by some other function */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderGetDefinition( psonFolder          * pFolder,
                              const char          * objectName,
                              uint32_t              strLength,
                              psoObjectDefinition * pDefinition,
                              psonDataDefinition ** ppDataDefinition,
                              psonKeyDefinition  ** ppKeyDefinition,
                              psonSessionContext  * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psonObjectDescriptor * pDesc = NULL;
   psonHashTxItem * pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txStatus;
   psonFolder * pNextFolder;
   psonMemObject * pMemObject;
   size_t bucket;
   bool found, ok;
#ifdef USE_DBC
   int pDesc_invalid_api_type = 0;
#endif
   
   PSO_PRE_CONDITION( pFolder          != NULL );
   PSO_PRE_CONDITION( objectName       != NULL )
   PSO_PRE_CONDITION( pDefinition      != NULL );
   PSO_PRE_CONDITION( ppKeyDefinition  != NULL );
   PSO_PRE_CONDITION( ppDataDefinition != NULL );
   PSO_PRE_CONDITION( pContext         != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psonHashTxGet( &pFolder->hashObj, 
                          (unsigned char *)objectName, 
                          partialLength * sizeof(char), 
                          &pHashItem,
                          &bucket,
                          pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = psonTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, psonMemObject );
      if ( psonLock( pMemObject, pContext ) ) {

         pDefinition->type = pDesc->apiType;

         switch( pDesc->apiType ) {

         case PSO_FOLDER:
            break;
         case PSO_HASH_MAP:
            {
               psonHashMap * p = GET_PTR_FAST( pDesc->offset, psonHashMap);
               
               *ppKeyDefinition = GET_PTR_FAST( p->keyDefOffset, psonKeyDefinition );
               *ppDataDefinition = GET_PTR_FAST( p->dataDefOffset, psonDataDefinition );
            }
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            {
               psonQueue * p = GET_PTR_FAST( pDesc->offset, psonQueue);
               
               *ppDataDefinition = GET_PTR_FAST( p->dataDefOffset, psonDataDefinition );
            }
            break;
         case PSO_FAST_MAP:
            {
               psonFastMap * p = GET_PTR_FAST( pDesc->offset, psonFastMap);
               
               *ppKeyDefinition = GET_PTR_FAST( p->keyDefOffset, psonKeyDefinition );
               *ppDataDefinition = GET_PTR_FAST( p->dataDefOffset, psonDataDefinition );
            }
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psonUnlock( pMemObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      if ( errcode != PSO_OK ) goto the_exit;
      
      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psonTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );
   if ( ! psonLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psonUnlock( &pFolder->memObject, pContext );
     
   ok = psonFolderGetDefinition( pNextFolder,
                                 &objectName[partialLength+1], 
                                 strLength - partialLength - 1, 
                                 pDefinition,
                                 ppDataDefinition,
                                 ppKeyDefinition,
                                 pContext );
   PSO_POST_CONDITION( ok == true || ok == false );

   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderGetDefLength( psonFolder          * pFolder,
                             const char          * objectName,
                             uint32_t              strLength,
                             uint32_t            * pDataDefLength,
                             uint32_t            * pKeyDefLength,
                             psonSessionContext  * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psonObjectDescriptor * pDesc = NULL;
   psonHashTxItem * pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txStatus;
   psonFolder * pNextFolder;
   psonMemObject * pMemObject;
   size_t bucket;
   bool found, ok;
#ifdef USE_DBC
   int pDesc_invalid_api_type = 0;
#endif
   
   PSO_PRE_CONDITION( pFolder        != NULL );
   PSO_PRE_CONDITION( objectName     != NULL )
   PSO_PRE_CONDITION( pKeyDefLength  != NULL );
   PSO_PRE_CONDITION( pDataDefLength != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psonHashTxGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = psonTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, psonMemObject );
      if ( psonLock( pMemObject, pContext ) ) {

         switch( pDesc->apiType ) {

         case PSO_FOLDER:
            break;
         case PSO_HASH_MAP:
            {
//               psonHashMap * p = GET_PTR_FAST( pDesc->offset, psonHashMap);
               
//               *pKeyDefLength = p->keyDefLength;
//               *pDataDefLength = p->dataDefLength;
            }
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            {
//               psonQueue * p = GET_PTR_FAST( pDesc->offset, psonQueue);
               
//               *pDataDefLength = p->dataDefLength;
            }
            break;
         case PSO_FAST_MAP:
            {
//               psonFastMap * p = GET_PTR_FAST( pDesc->offset, psonFastMap);
               
//               *pKeyDefLength = p->keyDefLength;
//               *pDataDefLength = p->dataDefLength;
            }
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psonUnlock( pMemObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      if ( errcode != PSO_OK ) goto the_exit;
      
      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psonTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );
   if ( ! psonLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psonUnlock( &pFolder->memObject, pContext );
     
   ok = psonFolderGetDefLength( pNextFolder,
                                &objectName[partialLength+1], 
                                strLength - partialLength - 1, 
                                pDataDefLength,
                                pKeyDefLength,
                                pContext );
   PSO_POST_CONDITION( ok == true || ok == false );

   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderGetObject( psonFolder         * pFolder,
                          const char         * objectName,
                          uint32_t             strLength,
                          enum psoObjectType   objectType,
                          psonFolderItem     * pFolderItem,
                          psonSessionContext * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psonObjectDescriptor* pDesc = NULL;
   psonHashTxItem* pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txStatus;
   psonTxStatus * txFolderStatus;
   psonFolder* pNextFolder;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL )
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psonHashTxGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   
   if ( lastIteration ) {
      GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );

      errcode = psonTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      if ( objectType != pDesc->apiType ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto the_exit;
      }

      txFolderStatus->usageCounter++;
      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItem;

      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }
   
   errcode = psonTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );
   if ( ! psonLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psonUnlock( &pFolder->memObject, pContext );
     
   ok = psonFolderGetObject( pNextFolder,
                             &objectName[partialLength+1], 
                             strLength - partialLength - 1, 
                             objectType,
                             pFolderItem,
                             pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderGetStatus( psonFolder         * pFolder,
                          const char         * objectName,
                          uint32_t             strLength, 
                          psoObjStatus       * pStatus,
                          psonSessionContext * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psonObjectDescriptor * pDesc = NULL;
   psonHashTxItem * pHashItem = NULL;
   psoErrors errcode;
   psonTxStatus * txStatus;
   psonFolder * pNextFolder;
   psonMemObject * pMemObject;
   size_t bucket;
   bool found, ok;
#ifdef USE_DBC
   int pDesc_invalid_api_type = 0;
#endif

   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL )
   PSO_PRE_CONDITION( pStatus    != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psonHashTxGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = PSO_NO_SUCH_OBJECT;
      }
      else {
         errcode = PSO_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = psonTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, psonMemObject );
      if ( psonLock( pMemObject, pContext ) ) {
         psonMemObjectStatus( pMemObject, pStatus );
         pStatus->type = pDesc->apiType;
         pStatus->status = txStatus->status;

         switch( pDesc->apiType ) {

         case PSO_FOLDER:
            psonAPIFolderStatus( GET_PTR_FAST( pDesc->memOffset, psonFolder ),
                                 pStatus );
            break;
         case PSO_HASH_MAP:
            psonHashMapStatus( GET_PTR_FAST( pDesc->memOffset, psonHashMap ),
                               pStatus );
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            psonQueueStatus( GET_PTR_FAST( pDesc->memOffset, psonQueue ),
                             pStatus );
            break;
         case PSO_FAST_MAP:
            psonFastMapStatus( GET_PTR_FAST( pDesc->memOffset, psonFastMap ), pStatus );
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psonUnlock( pMemObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      
      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psonTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );
   if ( ! psonLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psonUnlock( &pFolder->memObject, pContext );
     
   ok = psonFolderGetStatus( pNextFolder,
                             &objectName[partialLength+1], 
                             strLength - partialLength - 1, 
                             pStatus,
                             pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderInit( psonFolder         * pFolder,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfChilds,
                     psonTxStatus       * pTxStatus,
                     uint32_t             origNameLength,
                     char               * origName,
                     ptrdiff_t            hashItemOffset,
                     psonSessionContext * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pTxStatus != NULL );
   PSO_PRE_CONDITION( origName  != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks  > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   
   errcode = psonMemObjectInit( &pFolder->memObject, 
                                PSON_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonTreeNodeInit( &pFolder->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = psonHashTxInit( &pFolder->hashObj,
                           SET_OFFSET(&pFolder->memObject),
                           expectedNumOfChilds, 
                           pContext );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }
   
   pFolder->isSystemObject = false;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderInsertObject( psonFolder          * pFolder,
                             const char          * objectName,
                             const char          * originalName,
                             uint32_t              strLength, 
                             psoObjectDefinition * pDefinition,
                             psonDataDefinition  * pDataDefinition,
                             psonKeyDefinition   * pKeyDefinition,
                             size_t                numBlocks,
                             size_t                expectedNumOfChilds,
                             psonSessionContext  * pContext )
{
   bool lastIteration = true;
   uint32_t partialLength = 0;
   psonHashTxItem * pHashItem, * previousHashItem = NULL;
   psoErrors errcode = PSO_OK;
   psonObjectDescriptor* pDesc = NULL;
   size_t descLength;
   unsigned char* ptr = NULL;
   psonFolder* pNextFolder;
   psonTxStatus* objTxStatus;  /* txStatus of the created object */
   psonMemObjIdent memObjType = PSON_IDENT_LAST;
   size_t bucket;
   bool found, ok;
#ifdef USE_DBC
   int invalid_object_type = 0;
#endif

   PSO_PRE_CONDITION( pFolder      != NULL );
   PSO_PRE_CONDITION( objectName   != NULL )
   PSO_PRE_CONDITION( originalName != NULL )
   PSO_PRE_CONDITION( pContext     != NULL );
   PSO_PRE_CONDITION( pDefinition  != NULL );
   if ( pDefinition->type != PSO_FOLDER ) {
      PSO_PRE_CONDITION( pDataDefinition != NULL );
   }
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   errcode = psonValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   if ( lastIteration ) {
      if ( pFolder->isSystemObject ) {
         errcode = PSO_SYSTEM_OBJECT;
         goto the_exit;
      }
      /* 
       * We are now ready to create the object. The steps require for this
       * are:
       *  - check to see if the object exists already
       *  - allocate blocks of memory
       *  - insert the Descriptor in the hash of the current folder
       *  - add an Ops to the transaction object.
       *  - initialize the object
       *
       * The operations are done in the order shown above since each step
       * adds additional information needed for the next step (except between
       * step 3 and 4 - we initialize the object last because it might
       * become a pain to handle an error (rolling back the Init() calls)
       * once we have many types of objects
       */

      found = psonHashTxGet( &pFolder->hashObj, 
                           (unsigned char *)objectName, 
                           partialLength * sizeof(char), 
                           &previousHashItem,
                           &bucket,
                           pContext );
      if ( found ) {
         while ( previousHashItem->nextSameKey != PSON_NULL_OFFSET ) {
            GET_PTR( previousHashItem, previousHashItem->nextSameKey, psonHashTxItem );
         }
         objTxStatus = &previousHashItem->txStatus;
         if ( ! (objTxStatus->status & PSON_TXS_DESTROYED_COMMITTED) ) {
            errcode = PSO_OBJECT_ALREADY_PRESENT;
            goto the_exit;
         }
      }

      ptr = (unsigned char*) psonMallocBlocks( pContext->pAllocator,
                                               PSON_ALLOC_API_OBJ,
                                               numBlocks,
                                               pContext );
      if ( ptr == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
      descLength = offsetof(psonObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDesc = (psonObjectDescriptor *) malloc( descLength );
      if ( pDesc == NULL ) {
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memset( pDesc, 0, descLength );
      pDesc->apiType = pDefinition->type;
      pDesc->offset = SET_OFFSET( ptr );
      pDesc->nameLengthInBytes = partialLength * sizeof(char);
      memcpy( pDesc->originalName, originalName, pDesc->nameLengthInBytes );

      errcode = psonHashTxInsert( &pFolder->hashObj, 
                                  bucket,
                                  (unsigned char *)objectName, 
                                  partialLength * sizeof(char), 
                                  (void*)pDesc, 
                                  descLength,
                                  &pHashItem,
                                  pContext );
      if ( errcode != PSO_OK ) {
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         free( pDesc );
         goto the_exit;
      }

      switch( pDefinition->type ) {
      case PSO_FOLDER:
         memObjType = PSON_IDENT_FOLDER;
         break;
      case PSO_HASH_MAP:
         memObjType = PSON_IDENT_HASH_MAP;
         break;
      case PSO_FAST_MAP:
         memObjType = PSON_IDENT_MAP;
         break;
      case PSO_QUEUE:
      case PSO_LIFO:
         memObjType = PSON_IDENT_QUEUE;
         break;
      default:
         PSO_POST_CONDITION( invalid_object_type );
      }
      
      ok = psonTxAddOps( (psonTx*)pContext->pTransaction,
                         PSON_TX_ADD_OBJECT,
                         SET_OFFSET(pFolder),
                         PSON_IDENT_FOLDER,
                         SET_OFFSET(pHashItem),
                         memObjType,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      free( pDesc ); 
      pDesc = NULL;
      if ( ! ok ) {
         psonHashTxDelete( &pFolder->hashObj, 
                           pHashItem,
                           pContext );
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         goto the_exit;
      }
      
      objTxStatus = &pHashItem->txStatus;
      psonTxStatusInit( objTxStatus, SET_OFFSET(pContext->pTransaction) );
      objTxStatus->status = PSON_TXS_ADDED;
      
      GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
      switch ( memObjType ) {

      case PSON_IDENT_QUEUE:
         ok = psonQueueInit( (psonQueue *)ptr,
                             SET_OFFSET(pFolder),
                             numBlocks,
                             objTxStatus,
                             partialLength,
                             pDesc->originalName,
                             SET_OFFSET(pHashItem),
                             pDefinition,
                             pDataDefinition,
                             pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psonQueue,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psonQueue,memObject);
         break;

      case PSON_IDENT_FOLDER:
         ok = psonFolderInit( (psonFolder*)ptr,
                              SET_OFFSET(pFolder),
                              numBlocks,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              SET_OFFSET(pHashItem),
                              pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psonFolder,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psonFolder,memObject);
         break;
      
      case PSON_IDENT_HASH_MAP:
         ok = psonHashMapInit( (psonHashMap *)ptr,
                               SET_OFFSET(pFolder),
                               numBlocks,
                               expectedNumOfChilds,
                               objTxStatus,
                               partialLength,
                               pDesc->originalName,
                               SET_OFFSET(pHashItem),
                               pDefinition,
                               pKeyDefinition,
                               pDataDefinition,
                               pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psonHashMap,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psonHashMap,memObject);
         break;

      case PSON_IDENT_MAP:
         ok = psonFastMapInit( (psonFastMap *)ptr,
                               SET_OFFSET(pFolder),
                               numBlocks,
                               expectedNumOfChilds,
                               objTxStatus,
                               partialLength,
                               pDesc->originalName,
                               SET_OFFSET(pHashItem),
                               pDefinition,
                               pKeyDefinition,
                               pDataDefinition,
                               pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psonFastMap,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psonFastMap,memObject);
         break;

      default:
         errcode = PSO_INTERNAL_ERROR;
         goto the_exit;
      }
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) {
         psonTxRemoveLastOps( (psonTx*)pContext->pTransaction, pContext );
         psonHashTxDelete( &pFolder->hashObj,
                           pHashItem,
                           pContext );
         psonFreeBlocks( pContext->pAllocator, PSON_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         goto the_exit;
      }
      pFolder->nodeObject.txCounter++;
      if ( previousHashItem != NULL ) {
         previousHashItem->nextSameKey = SET_OFFSET(pHashItem);
      }
      psonUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* If we come here, this was not the last iteration, so we continue */
   found = psonHashTxGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSON_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psonHashTxItem );
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }
   
   errcode = psonTxTestObjectStatus( &pHashItem->txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psonFolder );   
   if ( ! psonLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   psonUnlock( &pFolder->memObject, pContext );

   ok = psonFolderInsertObject( pNextFolder,
                                &objectName[partialLength+1],
                                &originalName[partialLength+1],
                                strLength - partialLength - 1,
                                pDefinition,
                                pDataDefinition,
                                pKeyDefinition,
                                numBlocks,
                                expectedNumOfChilds,
                                pContext );
   PSO_POST_CONDITION( ok == true || ok == false );

   return ok;
   
the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psonUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonFolderRelease( psonFolder         * pFolder,
                        psonFolderItem     * pFolderItem,
                        psonSessionContext * pContext )
{
   psonTxStatus * txItemStatus, * txFolderStatus;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   txItemStatus = &pFolderItem->pHashItem->txStatus;
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      psonFolderReleaseNoLock( pFolder,
                               pFolderItem->pHashItem,
                               NULL,
                               pContext );

      psonUnlock( &pFolder->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psonFolderReleaseNoLock( psonFolder         * pFolder,
                              psonHashTxItem     * pHashItem,
                              bool               * isRemoved,
                              psonSessionContext * pContext )
{
   psonTxStatus * txItemStatus, * txFolderStatus;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   txItemStatus = &pHashItem->txStatus;
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );
   
   txItemStatus->parentCounter--;
   txFolderStatus->usageCounter--;

   /* 
    * if parentCounter is equal to zero, the object is not open. Since 
    * we hold the lock on the folder, no session can therefore open it
    * or use it in an iteration. We can remove it without problems if
    * a remove was indeed committed.
    */
   if ( (txItemStatus->parentCounter == 0) && 
      (txItemStatus->usageCounter == 0) ) {
      if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ||
         txItemStatus->status & PSON_TXS_EDIT_COMMITTED ) {
         /* Time to really delete the record! */
         psonFolderRemoveObject( pFolder,
                                 pHashItem,
                                 pContext );
         if ( isRemoved ) *isRemoved = true;
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * lock on the folder is the responsability of the caller.
 */
void psonFolderRemoveObject( psonFolder         * pFolder,
                             psonHashTxItem     * pHashItem,
                             psonSessionContext * pContext )
{
   psonHashTxItem * previousItem = NULL;
   psonObjectDescriptor * pDesc;
   void * ptrObject;
   size_t bucket;
   bool found;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSON_IDENT_FOLDER );

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
   GET_PTR( ptrObject, pDesc->offset, void );

   /* We search for the bucket */
   found = psonHashTxGet( &pFolder->hashObj, 
                        pHashItem->key, 
                        pHashItem->keyLength, 
                        &previousItem,
                        &bucket,
                        pContext );
   PSO_POST_CONDITION( found );

   /* 
    * Time to really delete the record!
    *
    * Note: the hash array will release the memory of the hash item.
    */
   psonHashTxDelete( &pFolder->hashObj, 
                     pHashItem,
                     pContext );

   pFolder->nodeObject.txCounter--;

   /* If needed */
   psonFolderResize( pFolder, pContext );

   /*
    * Since the object is now remove from the hash, all we need
    * to do is reclaim the memory (which is done in the destructor
    * of the memory object).
    */
   switch ( pDesc->apiType ) {
   case PSO_FOLDER:
      psonFolderFini( (psonFolder*)ptrObject, pContext );
      break;
   case PSO_HASH_MAP:
      psonHashMapFini( (psonHashMap *)ptrObject, pContext );
      break;
   case PSO_QUEUE:
   case PSO_LIFO:
      psonQueueFini( (psonQueue *)ptrObject, pContext );
      break;
   case PSO_FAST_MAP:
      psonFastMapFini( (psonFastMap *)ptrObject, pContext );
      break;
   case PSO_LAST_OBJECT_TYPE:
      ;
   }        
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFolderResize( psonFolder         * pFolder, 
                       psonSessionContext * pContext  )
{
   psonTxStatus * txFolderStatus;
   
   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txStatus->usageCounter: someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    */
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psonTxStatus );
   if ( (txFolderStatus->usageCounter == 0) &&
      (pFolder->nodeObject.txCounter == 0 ) ) {
      if ( pFolder->hashObj.enumResize != PSON_HASH_NO_RESIZE )
         psonHashTxResize( &pFolder->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonFolderRollbackEdit( psonFolder         * pFolder,
                             psonHashTxItem     * pHashItem, 
                             enum psoObjectType   objectType,
                             bool               * isRemoved,
                             psonSessionContext * pContext )
{
   psonObjectDescriptor * pDesc, * pDescLatest;
   psonFastMap * pMapLatest, * pMapEdit;
   psonHashTxItem * pHashItemLatest;
   psonTreeNode * tree;
   psonTxStatus * tx;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( objectType == PSON_IDENT_MAP );

   GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );

   pMapEdit = GET_PTR_FAST( pDesc->offset, psonFastMap );
   
   PSO_INV_CONDITION( pMapEdit->editVersion == SET_OFFSET(pHashItem) );
   
   pHashItemLatest = GET_PTR_FAST( pMapEdit->latestVersion, psonHashTxItem );
   pDescLatest = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                               psonObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDescLatest->offset, psonFastMap );
   
   pMapLatest->editVersion = PSON_NULL_OFFSET;

   /* We remove our edit version from the folder - this one is pretty
    * obvious. We also release the latest version, which basically means
    * we decriment some counters and in some limit case we might also
    * remove the map entirely (if it was destroyed - not open and the
    * current edit session was the only thing standing in the way
    */
   psonHashTxDelete( &pFolder->hashObj, pHashItem, pContext );
   /* If needed */
   psonFolderResize( pFolder, pContext );

   psonFastMapFini( pMapEdit, pContext );
      
   tree = GET_PTR_FAST( pDescLatest->nodeOffset, psonTreeNode );
   tx = GET_PTR_FAST( tree->txStatusOffset, psonTxStatus );
   psonTxStatusRollbackEdit( tx );

   psonFolderReleaseNoLock( pFolder,
                            pHashItemLatest,
                            isRemoved,
                            pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

psoErrors psonValidateString( const char * objectName,
                              uint32_t     strLength, 
                              uint32_t   * pPartialLength,
                              bool       * pLastIteration )
{
   uint32_t i;
   bool last = true;
   
   /* The first char is always special - it cannot be '/' */
   if ( ! isalpha( (int) objectName[0] )  ) {
      return PSO_INVALID_OBJECT_NAME;
   }
   
   for ( i = 1; i < strLength; ++i ) {
      if ( objectName[i] == '/' || objectName[i] == '\\' ) {
         last = false;
         /* Strip the last character if it is a separator (in other words */
         /* we keep lastIteration to true - we have found the end of the */
         /* "path". */
         if ( i == (strLength-1) ) last = true;
         break;
      }
      if ( !( isalnum((int) objectName[i]) || (objectName[i] == '_') ) ) {
         return PSO_INVALID_OBJECT_NAME;
      }
   }
   if ( i > PSO_MAX_NAME_LENGTH ) {
      return PSO_OBJECT_NAME_TOO_LONG;
   }
   
   *pPartialLength = i;
   *pLastIteration = last;

   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

