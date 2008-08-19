/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "Nucleus/Folder.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/Map.h"
#include "Nucleus/Queue.h"
#include "Nucleus/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psnFolderReleaseNoLock( psnFolder         * pFolder,
                              psnHashItem       * pHashItemItem,
                              psnSessionContext * pContext );

static 
psoErrors psnValidateString( const char * objectName,
                              size_t            strLength, 
                              size_t          * pPartialLength,
                              bool            * pLastIteration );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnFolderCommitEdit( psnFolder         * pFolder,
                           psnHashItem       * pHashItem, 
                           enum psoObjectType   objectType,
                           psnSessionContext * pContext )
{
   psnObjectDescriptor * pDesc, * pDescLatest;
   psnMap * pMapLatest, * pMapEdit;
   psnHashItem * pHashItemLatest;
   psnTreeNode * node;
   psnTxStatus * tx;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( objectType == PSN_IDENT_MAP );

   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );

   pMapEdit = GET_PTR_FAST( pDesc->offset, psnMap );
   
   PSO_INV_CONDITION( pMapEdit->editVersion == SET_OFFSET(pHashItem) );
   
   pHashItemLatest = GET_PTR_FAST( pMapEdit->latestVersion, psnHashItem );
   pDescLatest = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                               psnObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDescLatest->offset, psnMap );

   pHashItemLatest->nextSameKey = SET_OFFSET(pHashItem);
   pMapLatest->editVersion = PSN_NULL_OFFSET;
   pMapEdit->editVersion   = PSN_NULL_OFFSET;
   pMapLatest->latestVersion = SET_OFFSET(pHashItem);
   pMapEdit->latestVersion = SET_OFFSET(pHashItem);

   node = GET_PTR_FAST( pDesc->nodeOffset, psnTreeNode );
   tx = GET_PTR_FAST( node->txStatusOffset, psnTxStatus );

   psnTxStatusCommitEdit( &pHashItemLatest->txStatus, tx );

   /* Reminder: pHashItemLatest is now the old version */
   psnFolderReleaseNoLock( pFolder,
                            pHashItemLatest,
                            pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The new object created by this function is a child of the folder 
 */
bool psnFolderCreateObject( psnFolder          * pFolder,
                             const char          * objectName,
                             size_t                nameLengthInBytes,
                             psoObjectDefinition * pDefinition,
                             psnSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pDefinition->type > 0 && 
                      pDefinition->type < PSO_LAST_OBJECT_TYPE );
   
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

   /* lowecase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( name[i] == '/' || name[i] == '\\' ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /*
    * There is no psnUnlock here - the recursive nature of the 
    * function psnFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psnLock(&pFolder->memObject, pContext) ) {
      ok = psnFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   pDefinition,
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
bool psnFolderDeletable( psnFolder         * pFolder,
                          psnSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   psnHashItem * pItem;
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
   
   found = psnHashGetFirst( &pFolder->hashObj, &offset );
   while ( found ) {
      GET_PTR( pItem, offset, psnHashItem );
      if ( pItem->txStatus.txOffset != txOffset ) return false;
      if ( ! psnTxStatusIsMarkedAsDestroyed( &pItem->txStatus ) ) {
         return false;
      }
      
      previousOffset = offset;
      
      found = psnHashGetNext( &pFolder->hashObj,
                               previousOffset,
                               &offset );
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderDeleteObject( psnFolder         * pFolder,
                             const char         * objectName,
                             size_t               strLength, 
                             psnSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   psoErrors errcode = PSO_OK;
   psnObjectDescriptor* pDesc = NULL;
   psnHashItem* pHashItem = NULL;
   psnTxStatus* txStatus;
   psnFolder * pNextFolder, *pDeletedFolder;
   psnMemObject * pMemObj;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   errcode = psnValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;

   found = psnHashGet( &pFolder->hashObj, 
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
   while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
   }

   txStatus = &pHashItem->txStatus;
   
   if ( lastIteration ) {
      /* 
       * If the transaction id of the object is non-zero, a big no-no - 
       * we do not support two transactions on the same data
       * (and if remove is committed - the data is "non-existent").
       */
      if ( txStatus->txOffset != PSN_NULL_OFFSET ) {
         if ( txStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_NO_SUCH_OBJECT;
         }
         else {
            errcode = PSO_OBJECT_IS_IN_USE;
         }
         goto the_exit;
      }

      GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
      
      /*
       * A special case - folders cannot be deleted if they are not empty
       * (unless all other objects are marked as deleted by the current
       * transaction).
       *
       * Other objects contain data, not objects. 
       */
      if ( pDesc->apiType == PSO_FOLDER ) {
         GET_PTR( pDeletedFolder, pDesc->offset, psnFolder );
         if ( ! psnFolderDeletable( pDeletedFolder, pContext ) ) {
            errcode = PSO_FOLDER_IS_NOT_EMPTY;
            goto the_exit;
         }
      }
      GET_PTR( pMemObj, pDesc->memOffset, psnMemObject );
      
      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_REMOVE_OBJECT,
                         SET_OFFSET(pFolder),
                         PSN_IDENT_FOLDER,
                         SET_OFFSET(pHashItem),
                         pMemObj->objType,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto the_exit;
      
      psnTxStatusSetTx( txStatus, SET_OFFSET(pContext->pTransaction) );
      psnTxStatusMarkAsDestroyed( txStatus );
      pFolder->nodeObject.txCounter++;
      
      psnUnlock( &pFolder->memObject, pContext );

      return true;
   }

   /* If we come here, this was not the last iteration, so we continue */

   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
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
   if ( ! psnTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction)) 
        || psnTxStatusIsMarkedAsDestroyed( txStatus ) ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psnFolder );
   if ( ! psnLock(&pNextFolder->memObject, pContext) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   psnUnlock( &pFolder->memObject, pContext );

   ok = psnFolderDeleteObject( pNextFolder,
                                &objectName[partialLength+1],
                                strLength - partialLength - 1,
                                pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   return ok;
   
the_exit:

   /* pscSetError might have been already called by some other function */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderDestroyObject( psnFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              psnSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   size_t first = 0;
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
    * There is no psnUnlock here - the recursive nature of the 
    * function psnFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psnLock(&pFolder->memObject, pContext) ) {
      ok = psnFolderDeleteObject( pFolder,
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderEditObject( psnFolder         * pFolder,
                           const char         * objectName,
                           size_t               strLength,
                           enum psoObjectType   objectType,
                           psnFolderItem     * pFolderItem,
                           psnSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0, bucket = 0, descLength;
   psnObjectDescriptor * pDescOld = NULL, * pDescNew = NULL;
   psnHashItem * pHashItemOld = NULL, * pHashItemNew = NULL;
   psoErrors errcode;
   psnTxStatus * txStatus, * newTxStatus;
   psnTxStatus * txFolderStatus;
   psnFolder * pNextFolder;
   psnMemObject * pMemObject;
   unsigned char * ptr;
   psnMemObjIdent memObjType = PSN_IDENT_LAST;
   psnMap * pMap;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL )
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   errcode = psnValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psnHashGet( &pFolder->hashObj, 
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
   while ( pHashItemOld->nextSameKey != PSN_NULL_OFFSET ) {
      GET_PTR( pHashItemOld, pHashItemOld->nextSameKey, psnHashItem );
   }

   txStatus = &pHashItemOld->txStatus;

   GET_PTR( pDescOld, pHashItemOld->dataOffset, psnObjectDescriptor );
   
   if ( lastIteration ) {
      GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );
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
      if ( txStatus->txOffset != PSN_NULL_OFFSET ) {
         if ( txStatus->status & PSN_TXS_DESTROYED_COMMITTED ) {
            errcode = PSO_NO_SUCH_OBJECT;
            goto the_exit;
         }
         if ( txStatus->status & PSN_TXS_EDIT ) {
            errcode = PSO_A_SINGLE_UPDATER_IS_ALLOWED;
            goto the_exit;
         }
         if ( txStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
            if ( txStatus->status & PSN_TXS_DESTROYED ) {
               errcode = PSO_OBJECT_IS_DELETED;
               goto the_exit;
            }
            else if ( ! (txStatus->status & PSN_TXS_ADDED) ) {
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
         pMap = GET_PTR_FAST( pDescOld->offset, psnMap );
         if ( pMap->editVersion != PSN_NULL_OFFSET ) {
            errcode = PSO_A_SINGLE_UPDATER_IS_ALLOWED;
            goto the_exit;
         }
         memObjType = PSN_IDENT_MAP;
         break;
      default:
         errcode = PSO_INTERNAL_ERROR;
         goto the_exit;
      }

      pMemObject = GET_PTR_FAST( pDescOld->memOffset, psnMemObject );
      
      ptr = (unsigned char*) psnMallocBlocks( pContext->pAllocator,
                                               PSN_ALLOC_API_OBJ,
                                               pMemObject->totalBlocks,
                                               pContext );
      if ( ptr == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
      
      descLength = offsetof(psnObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDescNew = (psnObjectDescriptor *) malloc( descLength );
      if ( pDescNew == NULL ) {
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memcpy( pDescNew, pDescOld, descLength );
      pDescNew->offset = SET_OFFSET( ptr );

      errcode = psnHashInsertAt( &pFolder->hashObj, 
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
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         free( pDescNew );
         goto the_exit;
      }

      pDescNew = GET_PTR_FAST(pHashItemNew->dataOffset, psnObjectDescriptor);

      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_ADD_EDIT_OBJECT,
                         SET_OFFSET(pFolder),
                         PSN_IDENT_FOLDER,
                         SET_OFFSET(pHashItemNew),
                         memObjType,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         psnHashDelWithItem( &pFolder->hashObj, 
                              pHashItemNew,
                              pContext );
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         goto the_exit;
      }
      
      newTxStatus = &pHashItemNew->txStatus;
      psnTxStatusInit( newTxStatus, SET_OFFSET(pContext->pTransaction) );
      txStatus->txOffset = SET_OFFSET(pContext->pTransaction);
      txStatus->status |= PSN_TXS_EDIT;
      newTxStatus->status = txStatus->status;
      
      switch ( memObjType ) {
      case PSN_IDENT_MAP:
         ok = psnMapCopy( pMap, /* old, */
                           (psnMap *)ptr,
                           pHashItemNew,
                           pDescNew->originalName,
                           pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         pDescNew->nodeOffset = SET_OFFSET(ptr) + offsetof(psnMap,nodeObject);
         pDescNew->memOffset  = SET_OFFSET(ptr) + offsetof(psnMap,memObject);
         break;

      default:
         errcode = PSO_INTERNAL_ERROR;
         goto the_exit;
      }

      if ( ! ok ) {
         psnTxRemoveLastOps( (psnTx*)pContext->pTransaction, pContext );
         psnHashDelWithItem( &pFolder->hashObj, 
                              pHashItemNew,
                              pContext );
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         goto the_exit;
      }

      pFolder->nodeObject.txCounter++;
      txFolderStatus->usageCounter++;
      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItemNew;

      pHashItemNew->txStatus.parentCounter = 1;
      pHashItemNew->txStatus.usageCounter = 0;

      psnUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDescOld->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psnTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDescOld->offset, psnFolder );
   if ( ! psnLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psnUnlock( &pFolder->memObject, pContext );
     
   ok = psnFolderEditObject( pNextFolder,
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnFolderFini( psnFolder         * pFolder,
                     psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   psnHashFini( &pFolder->hashObj );
   psnTreeNodeFini( &pFolder->nodeObject );
   
   /* This call must be last - put a barrier here ? */ 
   psnMemObjectFini(  &pFolder->memObject, PSN_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderGetDefinition( psnFolder          * pFolder,
                              const char          * objectName,
                              size_t                strLength,
                              psoObjectDefinition * pDefinition,
                              psnFieldDef       ** ppInternalDef,
                              psnSessionContext  * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   psnObjectDescriptor * pDesc = NULL;
   psnHashItem * pHashItem = NULL;
   psoErrors errcode;
   psnTxStatus * txStatus;
   psnFolder * pNextFolder;
   psnMemObject * pMemObject;
   int pDesc_invalid_api_type = 0;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder       != NULL );
   PSO_PRE_CONDITION( objectName    != NULL )
   PSO_PRE_CONDITION( pDefinition   != NULL );
   PSO_PRE_CONDITION( ppInternalDef != NULL );
   PSO_PRE_CONDITION( pContext      != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   errcode = psnValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psnHashGet( &pFolder->hashObj, 
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
   while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = psnTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, psnMemObject );
      if ( psnLock( pMemObject, pContext ) ) {

         pDefinition->type = pDesc->apiType;

         switch( pDesc->apiType ) {

         case PSO_FOLDER:
            break;
         case PSO_HASH_MAP:
            *ppInternalDef = GET_PTR_FAST(
               GET_PTR_FAST( pDesc->offset, psnHashMap)->dataDefOffset,
               psnFieldDef );
            pDefinition->numFields = 
               GET_PTR_FAST( pDesc->offset, psnHashMap)->numFields;
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            pDefinition->numFields = 
               GET_PTR_FAST( pDesc->offset, psnQueue)->numFields;
            break;
         case PSO_FAST_MAP:
            *ppInternalDef = GET_PTR_FAST(
               GET_PTR_FAST( pDesc->offset, psnMap)->dataDefOffset,
               psnFieldDef );
            pDefinition->numFields = 
               GET_PTR_FAST( pDesc->offset, psnMap)->numFields;
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psnUnlock( pMemObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      
      psnUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psnTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psnFolder );
   if ( ! psnLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psnUnlock( &pFolder->memObject, pContext );
     
   ok = psnFolderGetDefinition( pNextFolder,
                                 &objectName[partialLength+1], 
                                 strLength - partialLength - 1, 
                                 pDefinition,
                                 ppInternalDef,
                                 pContext );
   PSO_POST_CONDITION( ok == true || ok == false );

   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderGetFirst( psnFolder         * pFolder,
                         psnFolderItem     * pItem,
                         psnSessionContext * pContext )
{
   psnHashItem* pHashItem = NULL;
   psnTxStatus * txItemStatus;
   psnTxStatus * txFolderStatus;
   ptrdiff_t  firstItemOffset;
   bool found;

   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL )
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );

   if ( psnLock( &pFolder->memObject, pContext ) ) {
      /*
       * We loop on all data items until we find one which is visible to the
       * current session (its transaction field equal to zero or to our 
       * transaction) AND is not marked as destroyed.
       */
      found = psnHashGetFirst( &pFolder->hashObj, &firstItemOffset );
      while ( found ) {
         GET_PTR( pHashItem, firstItemOffset, psnHashItem );
         txItemStatus = &pHashItem->txStatus;

        if ( psnTxTestObjectStatus( txItemStatus, 
            SET_OFFSET(pContext->pTransaction) ) == PSO_OK ) {

            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = firstItemOffset;
            pItem->status = txItemStatus->status;
            
            psnUnlock( &pFolder->memObject, pContext );
            
            return true;
         }
  
         found = psnHashGetNext( &pFolder->hashObj, 
                                  firstItemOffset,
                                  &firstItemOffset );
      }
   }
   else {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_IS_EMPTY );

   return false;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderGetNext( psnFolder         * pFolder,
                        psnFolderItem     * pItem,
                        psnSessionContext * pContext )
{
   psnHashItem * pHashItem = NULL;
   psnHashItem * previousHashItem = NULL;
   psnTxStatus * txItemStatus;
   psnTxStatus * txFolderStatus;
   ptrdiff_t  itemOffset;
   bool found;

   PSO_PRE_CONDITION( pFolder  != NULL );
   PSO_PRE_CONDITION( pItem    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );
   PSO_PRE_CONDITION( pItem->pHashItem  != NULL );
   PSO_PRE_CONDITION( pItem->itemOffset != PSN_NULL_OFFSET );
   
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );

   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( psnLock( &pFolder->memObject, pContext ) ) {
      /*
       * We loop on all data items until we find one which is visible to the
       * current session (its transaction field equal to zero or to our 
       * transaction) AND is not marked as destroyed.
       */
      found = psnHashGetNext( &pFolder->hashObj, 
                               itemOffset,
                               &itemOffset );
      while ( found ) {
         GET_PTR( pHashItem, itemOffset, psnHashItem );
         txItemStatus = &pHashItem->txStatus;

         if ( psnTxTestObjectStatus( txItemStatus, 
            SET_OFFSET(pContext->pTransaction) ) == PSO_OK ) {
 
            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = itemOffset;
            pItem->status = txItemStatus->status;

            psnFolderReleaseNoLock( pFolder, previousHashItem, pContext );

            psnUnlock( &pFolder->memObject, pContext );
            
            return true;
         }
  
         found = psnHashGetNext( &pFolder->hashObj, 
                                  itemOffset,
                                  &itemOffset );
      }
   }
   else {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
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
   psnFolderReleaseNoLock( pFolder, previousHashItem, pContext );
    
   psnUnlock( &pFolder->memObject, pContext );
   pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_REACHED_THE_END );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderGetObject( psnFolder         * pFolder,
                          const char         * objectName,
                          size_t               strLength,
                          enum psoObjectType   objectType,
                          psnFolderItem     * pFolderItem,
                          psnSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   psnObjectDescriptor* pDesc = NULL;
   psnHashItem* pHashItem = NULL;
   psoErrors errcode;
   psnTxStatus * txStatus;
   psnTxStatus * txFolderStatus;
   psnFolder* pNextFolder;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL )
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   errcode = psnValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psnHashGet( &pFolder->hashObj, 
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
   while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
   
   if ( lastIteration ) {
      GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );

      errcode = psnTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      if ( objectType != pDesc->apiType ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto the_exit;
      }

      txFolderStatus->usageCounter++;
      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItem;

      psnUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }
   
   errcode = psnTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psnFolder );
   if ( ! psnLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psnUnlock( &pFolder->memObject, pContext );
     
   ok = psnFolderGetObject( pNextFolder,
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderGetStatus( psnFolder         * pFolder,
                          const char         * objectName,
                          size_t               strLength, 
                          psoObjStatus       * pStatus,
                          psnSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   psnObjectDescriptor * pDesc = NULL;
   psnHashItem * pHashItem = NULL;
   psoErrors errcode;
   psnTxStatus * txStatus;
   psnFolder * pNextFolder;
   psnMemObject * pMemObject;
   int pDesc_invalid_api_type = 0;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL )
   PSO_PRE_CONDITION( pStatus    != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   errcode = psnValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   found = psnHashGet( &pFolder->hashObj, 
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
   while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = psnTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != PSO_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, psnMemObject );
      if ( psnLock( pMemObject, pContext ) ) {
         psnMemObjectStatus( pMemObject, pStatus );
         pStatus->type = pDesc->apiType;
         pStatus->status = txStatus->status;

         switch( pDesc->apiType ) {

         case PSO_FOLDER:
            psnFolderMyStatus( GET_PTR_FAST( pDesc->memOffset, psnFolder ),
                                pStatus );
            break;
         case PSO_HASH_MAP:
            psnHashMapStatus( GET_PTR_FAST( pDesc->memOffset, psnHashMap ),
                               pStatus );
            break;
         case PSO_QUEUE:
         case PSO_LIFO:
            psnQueueStatus( GET_PTR_FAST( pDesc->memOffset, psnQueue ),
                             pStatus );
            break;
         case PSO_FAST_MAP:
            psnMapStatus( GET_PTR_FAST( pDesc->memOffset, psnMap ), pStatus );
            break;
         default:
            PSO_INV_CONDITION( pDesc_invalid_api_type );
         }
         psnUnlock( pMemObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      
      psnUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = psnTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      if ( errcode == PSO_NO_SUCH_OBJECT) errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psnFolder );
   if ( ! psnLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   psnUnlock( &pFolder->memObject, pContext );
     
   ok = psnFolderGetStatus( pNextFolder,
                             &objectName[partialLength+1], 
                             strLength - partialLength - 1, 
                             pStatus,
                             pContext );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderInit( psnFolder         * pFolder,
                     ptrdiff_t            parentOffset,
                     size_t               numberOfBlocks,
                     size_t               expectedNumOfChilds,
                     psnTxStatus       * pTxStatus,
                     size_t               origNameLength,
                     char               * origName,
                     ptrdiff_t            hashItemOffset,
                     psnSessionContext * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pTxStatus != NULL );
   PSO_PRE_CONDITION( origName  != NULL );
   PSO_PRE_CONDITION( hashItemOffset != PSN_NULL_OFFSET );
   PSO_PRE_CONDITION( parentOffset   != PSN_NULL_OFFSET );
   PSO_PRE_CONDITION( numberOfBlocks  > 0 );
   PSO_PRE_CONDITION( origNameLength > 0 );
   
   errcode = psnMemObjectInit( &pFolder->memObject, 
                                PSN_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psnTreeNodeInit( &pFolder->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = psnHashInit( &pFolder->hashObj,
                           SET_OFFSET(&pFolder->memObject),
                           expectedNumOfChilds, 
                           pContext );
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderInsertObject( psnFolder          * pFolder,
                             const char          * objectName,
                             const char          * originalName,
                             size_t                strLength, 
                             psoObjectDefinition * pDefinition,
                             size_t                numBlocks,
                             size_t                expectedNumOfChilds,
                             psnSessionContext  * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   psnHashItem * pHashItem, * previousHashItem = NULL;
   psoErrors errcode = PSO_OK;
   psnObjectDescriptor* pDesc = NULL;
   size_t descLength;
   unsigned char* ptr = NULL;
   psnFolder* pNextFolder;
   psnTxStatus* objTxStatus;  /* txStatus of the created object */
   psnMemObjIdent memObjType = PSN_IDENT_LAST;
   int invalid_object_type = 0;
   size_t bucket;
   bool found, ok;
   
   PSO_PRE_CONDITION( pFolder      != NULL );
   PSO_PRE_CONDITION( objectName   != NULL )
   PSO_PRE_CONDITION( originalName != NULL )
   PSO_PRE_CONDITION( pContext     != NULL );
   PSO_PRE_CONDITION( pDefinition  != NULL );
   PSO_PRE_CONDITION( strLength > 0 );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   errcode = psnValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != PSO_OK ) goto the_exit;
   
   if ( lastIteration ) {
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

      found = psnHashGet( &pFolder->hashObj, 
                           (unsigned char *)objectName, 
                           partialLength * sizeof(char), 
                           &previousHashItem,
                           &bucket,
                           pContext );
      if ( found ) {
         while ( previousHashItem->nextSameKey != PSN_NULL_OFFSET ) {
            GET_PTR( previousHashItem, previousHashItem->nextSameKey, psnHashItem );
         }
         objTxStatus = &previousHashItem->txStatus;
         if ( ! (objTxStatus->status & PSN_TXS_DESTROYED_COMMITTED) ) {
            errcode = PSO_OBJECT_ALREADY_PRESENT;
            goto the_exit;
         }
      }

      ptr = (unsigned char*) psnMallocBlocks( pContext->pAllocator,
                                               PSN_ALLOC_API_OBJ,
                                               numBlocks,
                                               pContext );
      if ( ptr == NULL ) {
         errcode = PSO_NOT_ENOUGH_PSO_MEMORY;
         goto the_exit;
      }
      descLength = offsetof(psnObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDesc = (psnObjectDescriptor *) malloc( descLength );
      if ( pDesc == NULL ) {
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memset( pDesc, 0, descLength );
      pDesc->apiType = pDefinition->type;
      pDesc->offset = SET_OFFSET( ptr );
      pDesc->nameLengthInBytes = partialLength * sizeof(char);
      memcpy( pDesc->originalName, originalName, pDesc->nameLengthInBytes );

      errcode = psnHashInsertAt( &pFolder->hashObj, 
                                  bucket,
                                  (unsigned char *)objectName, 
                                  partialLength * sizeof(char), 
                                  (void*)pDesc, 
                                  descLength,
                                  &pHashItem,
                                  pContext );
      if ( errcode != PSO_OK ) {
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         free( pDesc );
         goto the_exit;
      }

      switch( pDefinition->type ) {
      case PSO_FOLDER:
         memObjType = PSN_IDENT_FOLDER;
         break;
      case PSO_HASH_MAP:
         memObjType = PSN_IDENT_HASH_MAP;
         break;
      case PSO_FAST_MAP:
         memObjType = PSN_IDENT_MAP;
         break;
      case PSO_QUEUE:
      case PSO_LIFO:
         memObjType = PSN_IDENT_QUEUE;
         break;
      default:
         PSO_POST_CONDITION( invalid_object_type );
      }
      
      ok = psnTxAddOps( (psnTx*)pContext->pTransaction,
                         PSN_TX_ADD_OBJECT,
                         SET_OFFSET(pFolder),
                         PSN_IDENT_FOLDER,
                         SET_OFFSET(pHashItem),
                         memObjType,
                         pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      free( pDesc ); 
      pDesc = NULL;
      if ( ! ok ) {
         psnHashDelWithItem( &pFolder->hashObj, 
                              pHashItem,
                              pContext );
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         goto the_exit;
      }
      
      objTxStatus = &pHashItem->txStatus;
      psnTxStatusInit( objTxStatus, SET_OFFSET(pContext->pTransaction) );
      objTxStatus->status = PSN_TXS_ADDED;
      
      GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
      switch ( memObjType ) {

      case PSN_IDENT_QUEUE:
         ok = psnQueueInit( (psnQueue *)ptr,
                             SET_OFFSET(pFolder),
                             numBlocks,
                             objTxStatus,
                             partialLength,
                             pDesc->originalName,
                             SET_OFFSET(pHashItem),
                             pDefinition,
                             pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psnQueue,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psnQueue,memObject);
         break;

      case PSN_IDENT_FOLDER:
         ok = psnFolderInit( (psnFolder*)ptr,
                              SET_OFFSET(pFolder),
                              numBlocks,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              SET_OFFSET(pHashItem),
                              pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psnFolder,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psnFolder,memObject);
         break;
      
      case PSN_IDENT_HASH_MAP:
         ok = psnHashMapInit( (psnHashMap *)ptr,
                               SET_OFFSET(pFolder),
                               numBlocks,
                               expectedNumOfChilds,
                               objTxStatus,
                               partialLength,
                               pDesc->originalName,
                               SET_OFFSET(pHashItem),
                               pDefinition,
                               pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psnHashMap,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psnHashMap,memObject);
         break;

      case PSN_IDENT_MAP:
         ok = psnMapInit( (psnMap *)ptr,
                           SET_OFFSET(pFolder),
                           numBlocks,
                           expectedNumOfChilds,
                           objTxStatus,
                           partialLength,
                           pDesc->originalName,
                           SET_OFFSET(pHashItem),
                           pDefinition,
                           pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(psnMap,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(psnMap,memObject);
         break;

      default:
         errcode = PSO_INTERNAL_ERROR;
         goto the_exit;
      }
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) {
         psnTxRemoveLastOps( (psnTx*)pContext->pTransaction, pContext );
         psnHashDelWithItem( &pFolder->hashObj,
                              pHashItem,
                              pContext );
         psnFreeBlocks( pContext->pAllocator, PSN_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         goto the_exit;
      }
      pFolder->nodeObject.txCounter++;
      if ( previousHashItem != NULL ) {
         previousHashItem->nextSameKey = SET_OFFSET(pHashItem);
      }
      psnUnlock( &pFolder->memObject, pContext );

      return true;
   }
   
   /* If we come here, this was not the last iteration, so we continue */
   found = psnHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != PSN_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, psnHashItem );
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
   if ( pDesc->apiType != PSO_FOLDER ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }
   
   errcode = psnTxTestObjectStatus( &pHashItem->txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != PSO_OK ) {
      errcode = PSO_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, psnFolder );   
   if ( ! psnLock( &pNextFolder->memObject, pContext ) ) {
      errcode = PSO_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   psnUnlock( &pFolder->memObject, pContext );

   ok = psnFolderInsertObject( pNextFolder,
                                &objectName[partialLength+1],
                                &originalName[partialLength+1],
                                strLength - partialLength - 1,
                                pDefinition,
                                numBlocks,
                                expectedNumOfChilds,
                                pContext );
   PSO_POST_CONDITION( ok == true || ok == false );

   return ok;
   
the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   psnUnlock( &pFolder->memObject, pContext );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnFolderMyStatus( psnFolder   * pFolder,
                         psoObjStatus * pStatus )
{
   psnTxStatus  * txStatus;

   PSO_PRE_CONDITION( pFolder != NULL );
   PSO_PRE_CONDITION( pStatus != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   GET_PTR( txStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pFolder->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnFolderRelease( psnFolder         * pFolder,
                        psnFolderItem     * pFolderItem,
                        psnSessionContext * pContext )
{
   psnTxStatus * txItemStatus, * txFolderStatus;
   
   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   txItemStatus = &pFolderItem->pHashItem->txStatus;
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );
   
   if ( psnLock( &pFolder->memObject, pContext ) ) {
      psnFolderReleaseNoLock( pFolder,
                               pFolderItem->pHashItem,
                               pContext );

      psnUnlock( &pFolder->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_OBJECT_CANNOT_GET_LOCK );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psnFolderReleaseNoLock( psnFolder         * pFolder,
                              psnHashItem       * pHashItem,
                              psnSessionContext * pContext )
{
   psnTxStatus * txItemStatus, * txFolderStatus;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   txItemStatus = &pHashItem->txStatus;
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );
   
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
      if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ||
         txItemStatus->status & PSN_TXS_EDIT_COMMITTED ) {
         /* Time to really delete the record! */
         psnFolderRemoveObject( pFolder,
                                 pHashItem,
                                 pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * lock on the folder is the responsability of the caller.
 */
void psnFolderRemoveObject( psnFolder         * pFolder,
                             psnHashItem       * pHashItem,
                             psnSessionContext * pContext )
{
   psnHashItem * previousItem = NULL;
   psnObjectDescriptor * pDesc;
   void * ptrObject;
   size_t bucket;
   bool found;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( pFolder->memObject.objType == PSN_IDENT_FOLDER );

   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
   GET_PTR( ptrObject, pDesc->offset, void );

   /* We search for the bucket */
   found = psnHashGet( &pFolder->hashObj, 
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
   psnHashDelWithItem( &pFolder->hashObj, 
                        pHashItem,
                        pContext );

   pFolder->nodeObject.txCounter--;

   /* If needed */
   psnFolderResize( pFolder, pContext );

   /*
    * Since the object is now remove from the hash, all we need
    * to do is reclaim the memory (which is done in the destructor
    * of the memory object).
    */
   switch ( pDesc->apiType ) {
   case PSO_FOLDER:
      psnFolderFini( (psnFolder*)ptrObject, pContext );
      break;
   case PSO_HASH_MAP:
      psnHashMapFini( (psnHashMap *)ptrObject, pContext );
      break;
   case PSO_QUEUE:
   case PSO_LIFO:
      psnQueueFini( (psnQueue *)ptrObject, pContext );
      break;
   case PSO_FAST_MAP:
      psnMapFini( (psnMap *)ptrObject, pContext );
      break;
   case PSO_LAST_OBJECT_TYPE:
      ;
   }        
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnFolderResize( psnFolder         * pFolder, 
                       psnSessionContext * pContext  )
{
   psnTxStatus * txFolderStatus;
   
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
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, psnTxStatus );
   if ( (txFolderStatus->usageCounter == 0) &&
      (pFolder->nodeObject.txCounter == 0 ) ) {
      if ( pFolder->hashObj.enumResize != PSN_HASH_NO_RESIZE )
         psnHashResize( &pFolder->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnFolderRollbackEdit( psnFolder         * pFolder,
                             psnHashItem       * pHashItem, 
                             enum psoObjectType   objectType,
                             psnSessionContext * pContext )
{
   psnObjectDescriptor * pDesc, * pDescLatest;
   psnMap * pMapLatest, * pMapEdit;
   psnHashItem * pHashItemLatest;
   psnTreeNode * tree;
   psnTxStatus * tx;
   
   PSO_PRE_CONDITION( pFolder   != NULL );
   PSO_PRE_CONDITION( pHashItem != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( objectType == PSN_IDENT_MAP );

   GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );

   pMapEdit = GET_PTR_FAST( pDesc->offset, psnMap );
   
   PSO_INV_CONDITION( pMapEdit->editVersion == SET_OFFSET(pHashItem) );
   
   pHashItemLatest = GET_PTR_FAST( pMapEdit->latestVersion, psnHashItem );
   pDescLatest = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                               psnObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDescLatest->offset, psnMap );
   
   pMapLatest->editVersion = PSN_NULL_OFFSET;

   /* We remove our edit version from the folder - this one is pretty
    * obvious. We also release the latest version, which basically means
    * we decriment some counters and in some limit case we might also
    * remove the map entirely (if it was destroyed - not open and the
    * current edit session was the only thing standing in the way
    */
   psnHashDelWithItem( &pFolder->hashObj, pHashItem, pContext );
   /* If needed */
   psnFolderResize( pFolder, pContext );

   psnMapFini( pMapEdit, pContext );
      
   tree = GET_PTR_FAST( pDescLatest->nodeOffset, psnTreeNode );
   tx = GET_PTR_FAST( tree->txStatusOffset, psnTxStatus );
   psnTxStatusRollbackEdit( tx );

   psnFolderReleaseNoLock( pFolder,
                            pHashItemLatest,
                            pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderCloseObject( psnFolderItem     * pFolderItem,
                               psnSessionContext * pContext )
{
   psnFolder   * parentFolder;
   psnTreeNode * pNode;
   psnTxStatus * txItemStatus, * txFolderStatus;
   psnObjectDescriptor * pDesc;
   
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );

   GET_PTR( pDesc, pFolderItem->pHashItem->dataOffset, psnObjectDescriptor );
   GET_PTR( pNode, pDesc->nodeOffset, psnTreeNode);
   
   /* Special case, the top folder */
   if ( pNode->myParentOffset == PSN_NULL_OFFSET ) return 0;

   GET_PTR( parentFolder, pNode->myParentOffset, psnFolder );
   GET_PTR( txFolderStatus, parentFolder->nodeObject.txStatusOffset, psnTxStatus );
   
   if ( psnLock( &parentFolder->memObject, pContext ) ) {
      GET_PTR( txItemStatus, pNode->txStatusOffset, psnTxStatus );
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
         if ( txItemStatus->status & PSN_TXS_DESTROYED_COMMITTED ||
            txItemStatus->status & PSN_TXS_EDIT_COMMITTED ) {
            /* Time to really delete the object and the record! */
            psnFolderRemoveObject( parentFolder,
                                    pFolderItem->pHashItem,
                                    pContext );
         }
      }
      psnUnlock( &parentFolder->memObject, pContext );

      return true;
   }
   
   pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_ENGINE_BUSY );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderCreateObject( psnFolder          * pFolder,
                                const char          * objectName,
                                size_t                nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                psnSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   bool ok;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pDefinition->type > 0 && 
                      pDefinition->type < PSO_LAST_OBJECT_TYPE );
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
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

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psnUnlock here - the recursive nature of the 
    * function psnFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psnLock( &pFolder->memObject, pContext ) ) {
      ok = psnFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   pDefinition,
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderDestroyObject( psnFolder         * pFolder,
                                 const char         * objectName,
                                 size_t               nameLengthInBytes,
                                 psnSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
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

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psnUnlock here - the recursive nature of the 
    * function psnFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psnLock( &pFolder->memObject, pContext ) ) {
      ok = psnFolderDeleteObject( pFolder,
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
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderEditObject( psnFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psnFolderItem     * pFolderItem,
                              psnSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   bool ok;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   strLength = nameLengthInBytes;
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
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

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* 
       * Opening the top folder (special case). No lock is needed here
       * since all we do is to retrieve the pointer (and we do not 
       * count access since the object is undeletable).
       */
      if ( objectType != PSO_FOLDER ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((psnMemoryHeader *) g_pBaseAddr)->topHashItem;
   }
   else {
      /*
       * There is no psnUnlock here - the recursive nature of the 
       * function psnFolderEditObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psnLock( &pFolder->memObject, pContext ) ) {
         ok = psnFolderEditObject( pFolder,
                                    &(lowerName[first]), 
                                    strLength, 
                                    objectType,
                                    pFolderItem,
                                    pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderGetDef( psnFolder          * pFolder,
                          const char          * objectName,
                          size_t                nameLengthInBytes,
                          psoObjectDefinition * pDefinition,
                          psnFieldDef       ** ppInternalDef,
                          psnSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   bool ok;
   size_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder       != NULL );
   PSO_PRE_CONDITION( objectName    != NULL );
   PSO_PRE_CONDITION( pDefinition   != NULL );
   PSO_PRE_CONDITION( ppInternalDef != NULL );
   PSO_PRE_CONDITION( pContext      != NULL );

   *ppInternalDef = NULL;
   
   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
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

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* Getting the status of the top folder (special case). */
      if ( psnLock( &pFolder->memObject, pContext ) ) {
         pDefinition->type = PSO_FOLDER;
         
         psnUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no psnUnlock here - the recursive nature of the 
       * function psnFolderGetDefinition() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psnLock( &pFolder->memObject, pContext ) ) {
         ok = psnFolderGetDefinition( pFolder,
                                       &(lowerName[first]), 
                                       strLength, 
                                       pDefinition,
                                       ppInternalDef,
                                       pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderGetStatus( psnFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             psoObjStatus       * pStatus,
                             psnSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   bool ok;
   size_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( pStatus    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
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

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* Getting the status of the top folder (special case). */
      if ( psnLock( &pFolder->memObject, pContext ) ) {
         psnMemObjectStatus( &pFolder->memObject, pStatus );
         pStatus->type = PSO_FOLDER;
         
         psnFolderMyStatus( pFolder, pStatus );

         psnUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no psnUnlock here - the recursive nature of the 
       * function psnFolderGetStatus() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psnLock( &pFolder->memObject, pContext ) ) {
         ok = psnFolderGetStatus( pFolder,
                                   &(lowerName[first]), 
                                   strLength, 
                                   pStatus,
                                   pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTopFolderOpenObject( psnFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psnFolderItem     * pFolderItem,
                              psnSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   size_t strLength, i;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   strLength = nameLengthInBytes;
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
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

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* 
       * Opening the top folder (special case). No lock is needed here
       * since all we do is to retrieve the pointer (and we do not 
       * count access since the object is undeletable).
       */
      if ( objectType != PSO_FOLDER ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((psnMemoryHeader *) g_pBaseAddr)->topHashItem;
   }
   else {
      /*
       * There is no psnUnlock here - the recursive nature of the 
       * function psnFolderGetObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psnLock( &pFolder->memObject, pContext ) ) {
         ok = psnFolderGetObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength, 
                                   objectType,
                                   pFolderItem,
                                   pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called pscSetError. 
    */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

psoErrors psnValidateString( const char * objectName,
                              size_t       strLength, 
                              size_t     * pPartialLength,
                              bool       * pLastIteration )
{
   size_t i;
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

