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

#include "Engine/Folder.h"
#include "Engine/Transaction.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/HashMap.h"
#include "Engine/Map.h"
#include "Engine/Queue.h"
#include "Engine/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void vdseFolderReleaseNoLock( vdseFolder         * pFolder,
                              vdseHashItem       * pHashItemItem,
                              vdseSessionContext * pContext );

static 
vdsErrors vdseValidateString( const char * objectName,
                              size_t            strLength, 
                              size_t          * pPartialLength,
                              bool            * pLastIteration );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFolderCommitEdit( vdseFolder         * pFolder,
                           vdseHashItem       * pHashItem, 
                           enum vdsObjectType   objectType,
                           vdseSessionContext * pContext )
{
   vdseObjectDescriptor * pDesc, * pDescLatest;
   vdseMap * pMapLatest, * pMapEdit;
   vdseHashItem * pHashItemLatest;
   vdseTreeNode * node;
   vdseTxStatus * tx;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( objectType == VDSE_IDENT_MAP );

   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );

   pMapEdit = GET_PTR_FAST( pDesc->offset, vdseMap );
   
   VDS_INV_CONDITION( pMapEdit->editVersion == SET_OFFSET(pHashItem) );
   
   pHashItemLatest = GET_PTR_FAST( pMapEdit->latestVersion, vdseHashItem );
   pDescLatest = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                               vdseObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDescLatest->offset, vdseMap );

   pHashItemLatest->nextSameKey = SET_OFFSET(pHashItem);
   pMapLatest->editVersion = VDSE_NULL_OFFSET;
   pMapEdit->editVersion   = VDSE_NULL_OFFSET;
   pMapLatest->latestVersion = SET_OFFSET(pHashItem);
   pMapEdit->latestVersion = SET_OFFSET(pHashItem);

   node = GET_PTR_FAST( pDesc->nodeOffset, vdseTreeNode );
   tx = GET_PTR_FAST( node->txStatusOffset, vdseTxStatus );

   vdseTxStatusCommitEdit( &pHashItemLatest->txStatus, tx );

   /* Reminder: pHashItemLatest is now the old version */
   vdseFolderReleaseNoLock( pFolder,
                            pHashItemLatest,
                            pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The new object created by this function is a child of the folder 
 */
bool vdseFolderCreateObject( vdseFolder          * pFolder,
                             const char          * objectName,
                             size_t                nameLengthInBytes,
                             vdsObjectDefinition * pDefinition,
                             vdseSessionContext  * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( objectName  != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( pDefinition != NULL );
   VDS_PRE_CONDITION( pDefinition->type > 0 && 
                      pDefinition->type < VDS_LAST_OBJECT_TYPE );
   
   strLength = nameLengthInBytes;

   if ( strLength > VDS_MAX_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( name[i] == '/' || name[i] == '\\' ) {
         errcode = VDS_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /*
    * There is no vdseUnlock here - the recursive nature of the 
    * function vdseFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( vdseLock(&pFolder->memObject, pContext) ) {
      rc = vdseFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   pDefinition,
                                   1, /* numBlocks, */
                                   0, /* expectedNumOfChilds, */
                                   pContext );
      if ( rc != 0 ) goto error_handler;
   }
   else {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

static
bool vdseFolderDeletable( vdseFolder         * pFolder,
                          vdseSessionContext * pContext )
{
   ptrdiff_t offset, previousOffset;
   vdseHashItem * pItem;
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
   
   found = vdseHashGetFirst( &pFolder->hashObj, &offset );
   while ( found ) {
      GET_PTR( pItem, offset, vdseHashItem );
      if ( pItem->txStatus.txOffset != txOffset ) return false;
      if ( ! vdseTxStatusIsMarkedAsDestroyed( &pItem->txStatus ) ) {
         return false;
      }
      
      previousOffset = offset;
      
      found = vdseHashGetNext( &pFolder->hashObj,
                               previousOffset,
                               &offset );
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderDeleteObject( vdseFolder         * pFolder,
                            const char         * objectName,
                            size_t               strLength, 
                            vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   int rc;
   vdsErrors errcode = VDS_OK;
   vdseObjectDescriptor* pDesc = NULL;
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus* txStatus;
   vdseFolder * pNextFolder, *pDeletedFolder;
   vdseMemObject * pMemObj;
   size_t bucket;
   bool found, ok;
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   errcode = vdseValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != VDS_OK ) goto the_exit;

   found = vdseHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char),
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = VDS_NO_SUCH_OBJECT;
      }
      else {
         errcode = VDS_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != VDSE_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, vdseHashItem );
   }

   txStatus = &pHashItem->txStatus;
   
   if ( lastIteration ) {
      /* 
       * If the transaction id of the object is non-zero, a big no-no - 
       * we do not support two transactions on the same data
       * (and if remove is committed - the data is "non-existent").
       */
      if ( txStatus->txOffset != VDSE_NULL_OFFSET ) {
         if ( txStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
            errcode = VDS_NO_SUCH_OBJECT;
         }
         else {
            errcode = VDS_OBJECT_IS_IN_USE;
         }
         goto the_exit;
      }

      GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
      
      /*
       * A special case - folders cannot be deleted if they are not empty
       * (unless all other objects are marked as deleted by the current
       * transaction).
       *
       * Other objects contain data, not objects. 
       */
      if ( pDesc->apiType == VDS_FOLDER ) {
         GET_PTR( pDeletedFolder, pDesc->offset, vdseFolder );
         if ( ! vdseFolderDeletable( pDeletedFolder, pContext ) ) {
            errcode = VDS_FOLDER_IS_NOT_EMPTY;
            goto the_exit;
         }
      }
      GET_PTR( pMemObj, pDesc->memOffset, vdseMemObject );
      
      ok = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_REMOVE_OBJECT,
                         SET_OFFSET(pFolder),
                         VDSE_IDENT_FOLDER,
                         SET_OFFSET(pHashItem),
                         pMemObj->objType,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto the_exit;
      
      vdseTxStatusSetTx( txStatus, SET_OFFSET(pContext->pTransaction) );
      vdseTxStatusMarkAsDestroyed( txStatus );
      pFolder->nodeObject.txCounter++;
      
      vdseUnlock( &pFolder->memObject, pContext );

      return 0;
   }

   /* If we come here, this was not the last iteration, so we continue */

   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
   if ( pDesc->apiType != VDS_FOLDER ) {
      errcode = VDS_NO_SUCH_FOLDER;
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
   if ( ! vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction)) 
        || vdseTxStatusIsMarkedAsDestroyed( txStatus ) ) {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, vdseFolder );
   if ( ! vdseLock(&pNextFolder->memObject, pContext) ) {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   vdseUnlock( &pFolder->memObject, pContext );

   rc = vdseFolderDeleteObject( pNextFolder,
                                &objectName[partialLength+1],
                                strLength - partialLength - 1,
                                pContext );
//   VDS_POST_CONDITION( ok == true || ok == false );
   return rc;
   
the_exit:

   /* vdscSetError might have been already called by some other function */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderDestroyObject( vdseFolder         * pFolder,
                              const char         * objectName,
                              size_t               nameLengthInBytes,
                              vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > VDS_MAX_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string and check for separators */
   for ( i = 0; i < strLength; ++i ) {
      if ( name[i] == '/' || name[i] == '\\' ) {
         errcode = VDS_INVALID_OBJECT_NAME;
         goto error_handler;
      }
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /*
    * There is no vdseUnlock here - the recursive nature of the 
    * function vdseFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( vdseLock(&pFolder->memObject, pContext) ) {
      rc = vdseFolderDeleteObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength,
                                   pContext );
//      VDS_POST_CONDITION( ok == true || ok == false );
//      if ( ! ok ) goto error_handler;
      if ( rc != 0 ) goto error_handler;
   }
   else {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

int vdseFolderEditObject( vdseFolder         * pFolder,
                           const char         * objectName,
                           size_t               strLength,
                           enum vdsObjectType   objectType,
                           vdseFolderItem     * pFolderItem,
                           vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0, bucket = 0, descLength;
   vdseObjectDescriptor * pDescOld = NULL, * pDescNew = NULL;
   vdseHashItem * pHashItemOld = NULL, * pHashItemNew = NULL;
   int rc;
   vdsErrors errcode;
   vdseTxStatus * txStatus, * newTxStatus;
   vdseTxStatus * txFolderStatus;
   vdseFolder * pNextFolder;
   vdseMemObject * pMemObject;
   unsigned char * ptr;
   vdseMemObjIdent memObjType = VDSE_IDENT_LAST;
   vdseMap * pMap;
   bool found, ok;
   
   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( objectName  != NULL )
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );

   errcode = vdseValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != VDS_OK ) goto the_exit;
   
   found = vdseHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItemOld,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = VDS_NO_SUCH_OBJECT;
      }
      else {
         errcode = VDS_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItemOld->nextSameKey != VDSE_NULL_OFFSET ) {
      GET_PTR( pHashItemOld, pHashItemOld->nextSameKey, vdseHashItem );
   }

   txStatus = &pHashItemOld->txStatus;

   GET_PTR( pDescOld, pHashItemOld->dataOffset, vdseObjectDescriptor );
   
   if ( lastIteration ) {
      GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );
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
      if ( txStatus->txOffset != VDSE_NULL_OFFSET ) {
         if ( txStatus->status & VDSE_TXS_DESTROYED_COMMITTED ) {
            errcode = VDS_NO_SUCH_OBJECT;
            goto the_exit;
         }
         if ( txStatus->status & VDSE_TXS_EDIT ) {
            errcode = VDS_A_SINGLE_UPDATER_IS_ALLOWED;
            goto the_exit;
         }
         if ( txStatus->txOffset == SET_OFFSET(pContext->pTransaction) ) {
            if ( txStatus->status & VDSE_TXS_DESTROYED ) {
               errcode = VDS_OBJECT_IS_DELETED;
               goto the_exit;
            }
            else if ( ! (txStatus->status & VDSE_TXS_ADDED) ) {
               errcode = VDS_OBJECT_IS_IN_USE;
               goto the_exit;
            }
         }
         else {
            errcode = VDS_OBJECT_IS_IN_USE;
            goto the_exit;
         }
      }
      if ( objectType != pDescOld->apiType ) {
         errcode = VDS_WRONG_OBJECT_TYPE;
         goto the_exit;
      }

      switch( pDescOld->apiType ) {
      case VDS_FAST_MAP:
         pMap = GET_PTR_FAST( pDescOld->offset, vdseMap );
         if ( pMap->editVersion != VDSE_NULL_OFFSET ) {
            errcode = VDS_A_SINGLE_UPDATER_IS_ALLOWED;
            goto the_exit;
         }
         memObjType = VDSE_IDENT_MAP;
         break;
      default:
         errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      pMemObject = GET_PTR_FAST( pDescOld->memOffset, vdseMemObject );
      
      ptr = (unsigned char*) vdseMallocBlocks( pContext->pAllocator,
                                               VDSE_ALLOC_API_OBJ,
                                               pMemObject->totalBlocks,
                                               pContext );
      if ( ptr == NULL ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
      
      descLength = offsetof(vdseObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDescNew = (vdseObjectDescriptor *) malloc( descLength );
      if ( pDescNew == NULL ) {
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memcpy( pDescNew, pDescOld, descLength );
      pDescNew->offset = SET_OFFSET( ptr );

      errcode = vdseHashInsertAt( &pFolder->hashObj, 
                                  bucket,
                                  (unsigned char *)objectName, 
                                  partialLength * sizeof(char), 
                                  (void*)pDescNew, 
                                  descLength,
                                  &pHashItemNew,
                                  pContext );
      free( pDescNew );
      pDescNew = NULL;
      if ( errcode != VDS_OK ) {
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         free( pDescNew );
         goto the_exit;
      }

      pDescNew = GET_PTR_FAST(pHashItemNew->dataOffset, vdseObjectDescriptor);

      ok = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD_EDIT_OBJECT,
                         SET_OFFSET(pFolder),
                         VDSE_IDENT_FOLDER,
                         SET_OFFSET(pHashItemNew),
                         memObjType,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         vdseHashDelWithItem( &pFolder->hashObj, 
                              pHashItemNew,
                              pContext );
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         goto the_exit;
      }
      
      newTxStatus = &pHashItemNew->txStatus;
      vdseTxStatusInit( newTxStatus, SET_OFFSET(pContext->pTransaction) );
      txStatus->txOffset = SET_OFFSET(pContext->pTransaction);
      txStatus->status |= VDSE_TXS_EDIT;
      newTxStatus->status = txStatus->status;
      
      switch ( memObjType ) {
      case VDSE_IDENT_MAP:
         rc = vdseMapCopy( pMap, /* old, */
                           (vdseMap *)ptr,
                           pHashItemNew,
                           pDescNew->originalName,
                           pContext );
         pDescNew->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseMap,nodeObject);
         pDescNew->memOffset  = SET_OFFSET(ptr) + offsetof(vdseMap,memObject);
         break;

      default:
         errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      if ( rc != 0 ) {
         vdseTxRemoveLastOps( (vdseTx*)pContext->pTransaction, pContext );
         vdseHashDelWithItem( &pFolder->hashObj, 
                              pHashItemNew,
                              pContext );
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, pMemObject->totalBlocks, pContext );
         goto the_exit;
      }

      pFolder->nodeObject.txCounter++;
      txFolderStatus->usageCounter++;
      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItemNew;

      pHashItemNew->txStatus.parentCounter = 1;
      pHashItemNew->txStatus.usageCounter = 0;

      vdseUnlock( &pFolder->memObject, pContext );

      return 0;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDescOld->apiType != VDS_FOLDER ) {
      errcode = VDS_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = vdseTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != VDS_OK ) {
      if ( errcode == VDS_NO_SUCH_OBJECT) errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDescOld->offset, vdseFolder );
   if ( ! vdseLock( &pNextFolder->memObject, pContext ) ) {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   vdseUnlock( &pFolder->memObject, pContext );
     
   rc = vdseFolderEditObject( pNextFolder,
                              &objectName[partialLength+1], 
                              strLength - partialLength - 1, 
                              objectType,
                              pFolderItem,
                              pContext );
//   VDS_POST_CONDITION( ok == true || ok == false );
   
   return rc;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFolderFini( vdseFolder         * pFolder,
                     vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   vdseHashFini( &pFolder->hashObj );
   vdseTreeNodeFini( &pFolder->nodeObject );
   
   /* This call must be last - put a barrier here ? */ 
   vdseMemObjectFini(  &pFolder->memObject, VDSE_ALLOC_API_OBJ, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderGetDefinition( vdseFolder          * pFolder,
                              const char          * objectName,
                              size_t                strLength,
                              vdsObjectDefinition * pDefinition,
                              vdseFieldDef       ** ppInternalDef,
                              vdseSessionContext  * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   vdseObjectDescriptor * pDesc = NULL;
   vdseHashItem * pHashItem = NULL;
   int rc;
   vdsErrors errcode;
   vdseTxStatus * txStatus;
   vdseFolder * pNextFolder;
   vdseMemObject * pMemObject;
   int pDesc_invalid_api_type = 0;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pFolder       != NULL );
   VDS_PRE_CONDITION( objectName    != NULL )
   VDS_PRE_CONDITION( pDefinition   != NULL );
   VDS_PRE_CONDITION( ppInternalDef != NULL );
   VDS_PRE_CONDITION( pContext      != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   errcode = vdseValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != VDS_OK ) goto the_exit;
   
   found = vdseHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = VDS_NO_SUCH_OBJECT;
      }
      else {
         errcode = VDS_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != VDSE_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, vdseHashItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = vdseTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != VDS_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, vdseMemObject );
      if ( vdseLock( pMemObject, pContext ) ) {

         pDefinition->type = pDesc->apiType;

         switch( pDesc->apiType ) {

         case VDS_FOLDER:
            break;
         case VDS_HASH_MAP:
            *ppInternalDef = GET_PTR_FAST(
               GET_PTR_FAST( pDesc->offset, vdseHashMap)->dataDefOffset,
               vdseFieldDef );
            pDefinition->numFields = 
               GET_PTR_FAST( pDesc->offset, vdseHashMap)->numFields;
            break;
         case VDS_QUEUE:
         case VDS_LIFO:
            pDefinition->numFields = 
               GET_PTR_FAST( pDesc->offset, vdseQueue)->numFields;
            break;
         case VDS_FAST_MAP:
            *ppInternalDef = GET_PTR_FAST(
               GET_PTR_FAST( pDesc->offset, vdseMap)->dataDefOffset,
               vdseFieldDef );
            pDefinition->numFields = 
               GET_PTR_FAST( pDesc->offset, vdseMap)->numFields;
            break;
         default:
            VDS_INV_CONDITION( pDesc_invalid_api_type );
         }
         vdseUnlock( pMemObject, pContext );
      }
      else {
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      
      vdseUnlock( &pFolder->memObject, pContext );

      return 0;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != VDS_FOLDER ) {
      errcode = VDS_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = vdseTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != VDS_OK ) {
      if ( errcode == VDS_NO_SUCH_OBJECT) errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, vdseFolder );
   if ( ! vdseLock( &pNextFolder->memObject, pContext ) ) {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   vdseUnlock( &pFolder->memObject, pContext );
     
   rc = vdseFolderGetDefinition( pNextFolder,
                                 &objectName[partialLength+1], 
                                 strLength - partialLength - 1, 
                                 pDefinition,
                                 ppInternalDef,
                                 pContext );
 //  VDS_POST_CONDITION( ok == true || ok == false );

   return rc;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseFolderGetFirst( vdseFolder         * pFolder,
                              vdseFolderItem     * pItem,
                              vdseSessionContext * pContext )
{
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txFolderStatus;
   ptrdiff_t  firstItemOffset;
   bool found;

   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pFolder->memObject, pContext ) ) {
      /*
       * We loop on all data items until we find one which is visible to the
       * current session (its transaction field equal to zero or to our 
       * transaction) AND is not marked as destroyed.
       */
      found = vdseHashGetFirst( &pFolder->hashObj, &firstItemOffset );
      while ( found ) {
         GET_PTR( pHashItem, firstItemOffset, vdseHashItem );
         txItemStatus = &pHashItem->txStatus;

        if ( vdseTxTestObjectStatus( txItemStatus, 
            SET_OFFSET(pContext->pTransaction) ) == VDS_OK ) {

            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = firstItemOffset;
            pItem->status = txItemStatus->status;
            
            vdseUnlock( &pFolder->memObject, pContext );
            
            return VDS_OK;
         }
  
         found = vdseHashGetNext( &pFolder->hashObj, 
                                  firstItemOffset,
                                  &firstItemOffset );
      }
   }
   else {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return VDS_OBJECT_CANNOT_GET_LOCK;
   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );

   return VDS_IS_EMPTY;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseFolderGetNext( vdseFolder         * pFolder,
                             vdseFolderItem     * pItem,
                             vdseSessionContext * pContext )
{
   vdseHashItem * pHashItem = NULL;
   vdseHashItem * previousHashItem = NULL;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txFolderStatus;
   ptrdiff_t  itemOffset;
   bool found;

   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != VDSE_NULL_OFFSET );
   
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( vdseLock( &pFolder->memObject, pContext ) ) {
      /*
       * We loop on all data items until we find one which is visible to the
       * current session (its transaction field equal to zero or to our 
       * transaction) AND is not marked as destroyed.
       */
      found = vdseHashGetNext( &pFolder->hashObj, 
                               itemOffset,
                               &itemOffset );
      while ( found ) {
         GET_PTR( pHashItem, itemOffset, vdseHashItem );
         txItemStatus = &pHashItem->txStatus;

         if ( vdseTxTestObjectStatus( txItemStatus, 
            SET_OFFSET(pContext->pTransaction) ) == VDS_OK ) {
 
            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->itemOffset = itemOffset;
            pItem->status = txItemStatus->status;

            vdseFolderReleaseNoLock( pFolder, previousHashItem, pContext );

            vdseUnlock( &pFolder->memObject, pContext );
            
            return VDS_OK;
         }
  
         found = vdseHashGetNext( &pFolder->hashObj, 
                                  itemOffset,
                                  &itemOffset );
      }
   }
   else {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return VDS_OBJECT_CANNOT_GET_LOCK;
   }
   
   /* 
    * If we come here, there are no additional data items to retrieve. As 
    * long as we clearly say that the internal iterator is reset (in case a 
    * "Get Previous" is implemented later), we can just release the iterator
    * at this point.
    */
   pItem->pHashItem = NULL;
   pItem->itemOffset = VDSE_NULL_OFFSET;
   vdseFolderReleaseNoLock( pFolder, previousHashItem, pContext );
    
   vdseUnlock( &pFolder->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );

   return VDS_REACHED_THE_END;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseFolderGetObject( vdseFolder         * pFolder,
                               const char         * objectName,
                               size_t               strLength,
                               enum vdsObjectType   objectType,
                               vdseFolderItem     * pFolderItem,
                               vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   vdseObjectDescriptor* pDesc = NULL;
   vdseHashItem* pHashItem = NULL;
   vdsErrors errcode;
   vdseTxStatus * txStatus;
   vdseTxStatus * txFolderStatus;
   vdseFolder* pNextFolder;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( objectName  != NULL )
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );

   errcode = vdseValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != VDS_OK ) goto the_exit;
   
   found = vdseHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = VDS_NO_SUCH_OBJECT;
      }
      else {
         errcode = VDS_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != VDSE_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, vdseHashItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
   
   if ( lastIteration ) {
      GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );

      errcode = vdseTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != VDS_OK ) goto the_exit;

      if ( objectType != pDesc->apiType ) {
         errcode = VDS_WRONG_OBJECT_TYPE;
         goto the_exit;
      }

      txFolderStatus->usageCounter++;
      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItem;

      vdseUnlock( &pFolder->memObject, pContext );

      return VDS_OK;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != VDS_FOLDER ) {
      errcode = VDS_NO_SUCH_OBJECT;
      goto the_exit;
   }
   
   errcode = vdseTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != VDS_OK ) {
      if ( errcode == VDS_NO_SUCH_OBJECT) errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, vdseFolder );
   if ( ! vdseLock( &pNextFolder->memObject, pContext ) ) {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   vdseUnlock( &pFolder->memObject, pContext );
     
   errcode = vdseFolderGetObject( pNextFolder,
                                  &objectName[partialLength+1], 
                                  strLength - partialLength - 1, 
                                  objectType,
                                  pFolderItem,
                                  pContext );
   
   return errcode;

the_exit:

   VDS_POST_CONDITION( errcode != VDS_OK );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
//   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
//   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseFolderGetStatus( vdseFolder         * pFolder,
                               const char         * objectName,
                               size_t               strLength, 
                               vdsObjStatus       * pStatus,
                               vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   vdseObjectDescriptor * pDesc = NULL;
   vdseHashItem * pHashItem = NULL;
   vdsErrors errcode;
   vdseTxStatus * txStatus;
   vdseFolder * pNextFolder;
   vdseMemObject * pMemObject;
   int pDesc_invalid_api_type = 0;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL )
   VDS_PRE_CONDITION( pStatus    != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   errcode = vdseValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != VDS_OK ) goto the_exit;
   
   found = vdseHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      if (lastIteration) {
         errcode = VDS_NO_SUCH_OBJECT;
      }
      else {
         errcode = VDS_NO_SUCH_FOLDER;
      }
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != VDSE_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, vdseHashItem );
   }

   txStatus = &pHashItem->txStatus;

   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
   
   if ( lastIteration ) {

      errcode = vdseTxTestObjectStatus( txStatus, 
                                        SET_OFFSET(pContext->pTransaction) );
      if ( errcode != VDS_OK ) goto the_exit;

      GET_PTR( pMemObject, pDesc->memOffset, vdseMemObject );
      if ( vdseLock( pMemObject, pContext ) ) {
         vdseMemObjectStatus( pMemObject, pStatus );
         pStatus->type = pDesc->apiType;
         pStatus->status = txStatus->status;

         switch( pDesc->apiType ) {

         case VDS_FOLDER:
            vdseFolderMyStatus( GET_PTR_FAST( pDesc->memOffset, vdseFolder ),
                                pStatus );
            break;
         case VDS_HASH_MAP:
            vdseHashMapStatus( GET_PTR_FAST( pDesc->memOffset, vdseHashMap ),
                               pStatus );
            break;
         case VDS_QUEUE:
         case VDS_LIFO:
            vdseQueueStatus( GET_PTR_FAST( pDesc->memOffset, vdseQueue ),
                             pStatus );
            break;
         case VDS_FAST_MAP:
            vdseMapStatus( GET_PTR_FAST( pDesc->memOffset, vdseMap ), pStatus );
            break;
         default:
            VDS_INV_CONDITION( pDesc_invalid_api_type );
         }
         vdseUnlock( pMemObject, pContext );
      }
      else {
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         goto the_exit;
      }
      
      vdseUnlock( &pFolder->memObject, pContext );

      return VDS_OK;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != VDS_FOLDER ) {
      errcode = VDS_NO_SUCH_OBJECT;
      goto the_exit;
   }

   errcode = vdseTxTestObjectStatus( txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != VDS_OK ) {
      if ( errcode == VDS_NO_SUCH_OBJECT) errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, vdseFolder );
   if ( ! vdseLock( &pNextFolder->memObject, pContext ) ) {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   vdseUnlock( &pFolder->memObject, pContext );
     
   errcode = vdseFolderGetStatus( pNextFolder,
                                  &objectName[partialLength+1], 
                                  strLength - partialLength - 1, 
                                  pStatus,
                                  pContext );
   
   return errcode;

the_exit:

   VDS_POST_CONDITION( errcode != VDS_OK );
   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
//   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
//   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderInit( vdseFolder         * pFolder,
                    ptrdiff_t            parentOffset,
                    size_t               numberOfBlocks,
                    size_t               expectedNumOfChilds,
                    vdseTxStatus       * pTxStatus,
                    size_t               origNameLength,
                    char               * origName,
                    ptrdiff_t            hashItemOffset,
                    vdseSessionContext * pContext )
{
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pTxStatus != NULL );
   VDS_PRE_CONDITION( origName  != NULL );
   VDS_PRE_CONDITION( hashItemOffset != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( parentOffset   != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   
   errcode = vdseMemObjectInit( &pFolder->memObject, 
                                VDSE_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }

   vdseTreeNodeInit( &pFolder->nodeObject,
                     SET_OFFSET(pTxStatus),
                     origNameLength,
                     SET_OFFSET(origName),
                     parentOffset,
                     hashItemOffset );

   errcode = vdseHashInit( &pFolder->hashObj,
                           SET_OFFSET(&pFolder->memObject),
                           expectedNumOfChilds, 
                           pContext );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    errcode );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderInsertObject( vdseFolder          * pFolder,
                            const char          * objectName,
                            const char          * originalName,
                            size_t                strLength, 
                            vdsObjectDefinition * pDefinition,
                            size_t                numBlocks,
                            size_t                expectedNumOfChilds,
                            vdseSessionContext  * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   vdseHashItem * pHashItem, * previousHashItem = NULL;
   int rc = 0;
   vdsErrors errcode = VDS_OK;
   vdseObjectDescriptor* pDesc = NULL;
   size_t descLength;
   unsigned char* ptr = NULL;
   vdseFolder* pNextFolder;
   vdseTxStatus* objTxStatus;  /* txStatus of the created object */
   vdseMemObjIdent memObjType = VDSE_IDENT_LAST;
   int invalid_object_type = 0;
   size_t bucket;
   bool found, ok;
   
   VDS_PRE_CONDITION( pFolder      != NULL );
   VDS_PRE_CONDITION( objectName   != NULL )
   VDS_PRE_CONDITION( originalName != NULL )
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( pDefinition  != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   errcode = vdseValidateString( objectName, 
                                 strLength, 
                                 &partialLength, 
                                 &lastIteration );
   if ( errcode != VDS_OK ) goto the_exit;
   
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

      found = vdseHashGet( &pFolder->hashObj, 
                           (unsigned char *)objectName, 
                           partialLength * sizeof(char), 
                           &previousHashItem,
                           &bucket,
                           pContext );
      if ( found ) {
         while ( previousHashItem->nextSameKey != VDSE_NULL_OFFSET ) {
            GET_PTR( previousHashItem, previousHashItem->nextSameKey, vdseHashItem );
         }
         objTxStatus = &previousHashItem->txStatus;
         if ( ! (objTxStatus->status & VDSE_TXS_DESTROYED_COMMITTED) ) {
            errcode = VDS_OBJECT_ALREADY_PRESENT;
            goto the_exit;
         }
      }

      ptr = (unsigned char*) vdseMallocBlocks( pContext->pAllocator,
                                               VDSE_ALLOC_API_OBJ,
                                               numBlocks,
                                               pContext );
      if ( ptr == NULL ) {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
      descLength = offsetof(vdseObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(char);
      pDesc = (vdseObjectDescriptor *) malloc( descLength );
      if ( pDesc == NULL ) {
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memset( pDesc, 0, descLength );
      pDesc->apiType = pDefinition->type;
      pDesc->offset = SET_OFFSET( ptr );
      pDesc->nameLengthInBytes = partialLength * sizeof(char);
      memcpy( pDesc->originalName, originalName, pDesc->nameLengthInBytes );

      errcode = vdseHashInsertAt( &pFolder->hashObj, 
                                  bucket,
                                  (unsigned char *)objectName, 
                                  partialLength * sizeof(char), 
                                  (void*)pDesc, 
                                  descLength,
                                  &pHashItem,
                                  pContext );
      if ( errcode != VDS_OK ) {
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         free( pDesc );
         goto the_exit;
      }

      switch( pDefinition->type ) {
      case VDS_FOLDER:
         memObjType = VDSE_IDENT_FOLDER;
         break;
      case VDS_HASH_MAP:
         memObjType = VDSE_IDENT_HASH_MAP;
         break;
      case VDS_FAST_MAP:
         memObjType = VDSE_IDENT_MAP;
         break;
      case VDS_QUEUE:
      case VDS_LIFO:
         memObjType = VDSE_IDENT_QUEUE;
         break;
      default:
         VDS_POST_CONDITION( invalid_object_type );
      }
      
      ok = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD_OBJECT,
                         SET_OFFSET(pFolder),
                         VDSE_IDENT_FOLDER,
                         SET_OFFSET(pHashItem),
                         memObjType,
                         pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
      free( pDesc ); 
      pDesc = NULL;
      if ( ! ok ) {
         vdseHashDelWithItem( &pFolder->hashObj, 
                              pHashItem,
                              pContext );
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         goto the_exit;
      }
      
      objTxStatus = &pHashItem->txStatus;
      vdseTxStatusInit( objTxStatus, SET_OFFSET(pContext->pTransaction) );
      objTxStatus->status = VDSE_TXS_ADDED;
      
      GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
      switch ( memObjType ) {

      ok = true;
      case VDSE_IDENT_QUEUE:
         rc = vdseQueueInit( (vdseQueue *)ptr,
                             SET_OFFSET(pFolder),
                             numBlocks,
                             objTxStatus,
                             partialLength,
                             pDesc->originalName,
                             SET_OFFSET(pHashItem),
                             pDefinition,
                             pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseQueue,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseQueue,memObject);
         break;

      case VDSE_IDENT_FOLDER:
         rc = vdseFolderInit( (vdseFolder*)ptr,
                              SET_OFFSET(pFolder),
                              numBlocks,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              SET_OFFSET(pHashItem),
                              pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseFolder,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseFolder,memObject);
         break;
      
      case VDSE_IDENT_HASH_MAP:
         ok = vdseHashMapInit( (vdseHashMap *)ptr,
                               SET_OFFSET(pFolder),
                               numBlocks,
                               expectedNumOfChilds,
                               objTxStatus,
                               partialLength,
                               pDesc->originalName,
                               SET_OFFSET(pHashItem),
                               pDefinition,
                               pContext );
         VDS_POST_CONDITION( ok == true || ok == false );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseHashMap,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseHashMap,memObject);
         break;

      case VDSE_IDENT_MAP:
         rc = vdseMapInit( (vdseMap *)ptr,
                           SET_OFFSET(pFolder),
                           numBlocks,
                           expectedNumOfChilds,
                           objTxStatus,
                           partialLength,
                           pDesc->originalName,
                           SET_OFFSET(pHashItem),
                           pDefinition,
                           pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseMap,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseMap,memObject);
         break;

      default:
         errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      if ( rc != 0 || ! ok ) {
         vdseTxRemoveLastOps( (vdseTx*)pContext->pTransaction, pContext );
         vdseHashDelWithItem( &pFolder->hashObj,
                              pHashItem,
                              pContext );
         vdseFreeBlocks( pContext->pAllocator, VDSE_ALLOC_API_OBJ,
                         ptr, numBlocks, pContext );
         goto the_exit;
      }
      pFolder->nodeObject.txCounter++;
      if ( previousHashItem != NULL ) {
         previousHashItem->nextSameKey = SET_OFFSET(pHashItem);
      }
      vdseUnlock( &pFolder->memObject, pContext );

      return VDS_OK;
   }
   
   /* If we come here, this was not the last iteration, so we continue */
   found = vdseHashGet( &pFolder->hashObj, 
                        (unsigned char *)objectName, 
                        partialLength * sizeof(char), 
                        &pHashItem,
                        &bucket,
                        pContext );
   if ( ! found ) {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }
   while ( pHashItem->nextSameKey != VDSE_NULL_OFFSET ) {
      GET_PTR( pHashItem, pHashItem->nextSameKey, vdseHashItem );
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
   if ( pDesc->apiType != VDS_FOLDER ) {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }
   
   errcode = vdseTxTestObjectStatus( &pHashItem->txStatus, 
                                     SET_OFFSET(pContext->pTransaction) );
   if ( errcode != VDS_OK ) {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   GET_PTR( pNextFolder, pDesc->offset, vdseFolder );   
   if ( ! vdseLock( &pNextFolder->memObject, pContext ) ) {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   vdseUnlock( &pFolder->memObject, pContext );

   rc = vdseFolderInsertObject( pNextFolder,
                                &objectName[partialLength+1],
                                &originalName[partialLength+1],
                                strLength - partialLength - 1,
                                pDefinition,
                                numBlocks,
                                expectedNumOfChilds,
                                pContext );
   return rc;
   
the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   vdseUnlock( &pFolder->memObject, pContext );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFolderMyStatus( vdseFolder   * pFolder,
                         vdsObjStatus * pStatus )
{
   vdseTxStatus  * txStatus;

   VDS_PRE_CONDITION( pFolder != NULL );
   VDS_PRE_CONDITION( pStatus != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   GET_PTR( txStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   pStatus->status = txStatus->status;
   pStatus->numDataItem = pFolder->hashObj.numberOfItems;
   pStatus->maxDataLength = 0;
   pStatus->maxKeyLength  = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderRelease( vdseFolder         * pFolder,
                       vdseFolderItem     * pFolderItem,
                       vdseSessionContext * pContext )
{
   vdseTxStatus * txItemStatus, * txFolderStatus;
   
   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   txItemStatus = &pFolderItem->pHashItem->txStatus;
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( vdseLock( &pFolder->memObject, pContext ) ) {
      vdseFolderReleaseNoLock( pFolder,
                               pFolderItem->pHashItem,
                               pContext );

      vdseUnlock( &pFolder->memObject, pContext );
   }
   else {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void vdseFolderReleaseNoLock( vdseFolder         * pFolder,
                              vdseHashItem       * pHashItem,
                              vdseSessionContext * pContext )
{
   vdseTxStatus * txItemStatus, * txFolderStatus;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   txItemStatus = &pHashItem->txStatus;
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );
   
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
      if ( txItemStatus->status & VDSE_TXS_DESTROYED_COMMITTED ||
         txItemStatus->status & VDSE_TXS_EDIT_COMMITTED ) {
         /* Time to really delete the record! */
         vdseFolderRemoveObject( pFolder,
                                 pHashItem,
                                 pContext );
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * lock on the folder is the responsability of the caller.
 */
void vdseFolderRemoveObject( vdseFolder         * pFolder,
                             vdseHashItem       * pHashItem,
                             vdseSessionContext * pContext )
{
   vdseHashItem * previousItem = NULL;
   vdseObjectDescriptor * pDesc;
   void * ptrObject;
   size_t bucket;
   bool found;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
   GET_PTR( ptrObject, pDesc->offset, void );

   /* We search for the bucket */
   found = vdseHashGet( &pFolder->hashObj, 
                        pHashItem->key, 
                        pHashItem->keyLength, 
                        &previousItem,
                        &bucket,
                        pContext );
   VDS_POST_CONDITION( found );

   /* 
    * Time to really delete the record!
    *
    * Note: the hash array will release the memory of the hash item.
    */
   vdseHashDelWithItem( &pFolder->hashObj, 
                        pHashItem,
                        pContext );

   pFolder->nodeObject.txCounter--;

   /* If needed */
   vdseFolderResize( pFolder, pContext );

   /*
    * Since the object is now remove from the hash, all we need
    * to do is reclaim the memory (which is done in the destructor
    * of the memory object).
    */
   switch ( pDesc->apiType ) {
   case VDS_FOLDER:
      vdseFolderFini( (vdseFolder*)ptrObject, pContext );
      break;
   case VDS_HASH_MAP:
      vdseHashMapFini( (vdseHashMap *)ptrObject, pContext );
      break;
   case VDS_QUEUE:
   case VDS_LIFO:
      vdseQueueFini( (vdseQueue *)ptrObject, pContext );
      break;
   case VDS_FAST_MAP:
      vdseMapFini( (vdseMap *)ptrObject, pContext );
      break;
   case VDS_LAST_OBJECT_TYPE:
      ;
   }        
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFolderResize( vdseFolder         * pFolder, 
                       vdseSessionContext * pContext  )
{
   vdseTxStatus * txFolderStatus;
   
   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   /*
    * Do we need to resize? We need both conditions here:
    *
    *   - txStatus->usageCounter: someone has a pointer to the data
    *
    *   - nodeObject.txCounter: offset to some of our data is part of a
    *                           transaction.
    */
   GET_PTR( txFolderStatus, pFolder->nodeObject.txStatusOffset, vdseTxStatus );
   if ( (txFolderStatus->usageCounter == 0) &&
      (pFolder->nodeObject.txCounter == 0 ) ) {
      if ( pFolder->hashObj.enumResize != VDSE_HASH_NO_RESIZE )
         vdseHashResize( &pFolder->hashObj, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseFolderRollbackEdit( vdseFolder         * pFolder,
                             vdseHashItem       * pHashItem, 
                             enum vdsObjectType   objectType,
                             vdseSessionContext * pContext )
{
   vdseObjectDescriptor * pDesc, * pDescLatest;
   vdseMap * pMapLatest, * pMapEdit;
   vdseHashItem * pHashItemLatest;
   vdseTreeNode * tree;
   vdseTxStatus * tx;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pHashItem != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( objectType == VDSE_IDENT_MAP );

   GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );

   pMapEdit = GET_PTR_FAST( pDesc->offset, vdseMap );
   
   VDS_INV_CONDITION( pMapEdit->editVersion == SET_OFFSET(pHashItem) );
   
   pHashItemLatest = GET_PTR_FAST( pMapEdit->latestVersion, vdseHashItem );
   pDescLatest = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                               vdseObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDescLatest->offset, vdseMap );
   
   pMapLatest->editVersion = VDSE_NULL_OFFSET;

   /* We remove our edit version from the folder - this one is pretty
    * obvious. We also release the latest version, which basically means
    * we decriment some counters and in some limit case we might also
    * remove the map entirely (if it was destroyed - not open and the
    * current edit session was the only thing standing in the way
    */
   vdseHashDelWithItem( &pFolder->hashObj, pHashItem, pContext );
   /* If needed */
   vdseFolderResize( pFolder, pContext );

   vdseMapFini( pMapEdit, pContext );
      
   tree = GET_PTR_FAST( pDescLatest->nodeOffset, vdseTreeNode );
   tx = GET_PTR_FAST( tree->txStatusOffset, vdseTxStatus );
   vdseTxStatusRollbackEdit( tx );

   vdseFolderReleaseNoLock( pFolder,
                            pHashItemLatest,
                            pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTopFolderCloseObject( vdseFolderItem     * pFolderItem,
                              vdseSessionContext * pContext )
{
   vdseFolder   * parentFolder;
   vdseTreeNode * pNode;
   vdseTxStatus * txItemStatus, * txFolderStatus;
   vdseObjectDescriptor * pDesc;
   
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );

   GET_PTR( pDesc, pFolderItem->pHashItem->dataOffset, vdseObjectDescriptor );
   GET_PTR( pNode, pDesc->nodeOffset, vdseTreeNode);
   
   /* Special case, the top folder */
   if ( pNode->myParentOffset == VDSE_NULL_OFFSET ) return 0;

   GET_PTR( parentFolder, pNode->myParentOffset, vdseFolder );
   GET_PTR( txFolderStatus, parentFolder->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( vdseLock( &parentFolder->memObject, pContext ) ) {
      GET_PTR( txItemStatus, pNode->txStatusOffset, vdseTxStatus );
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
         if ( txItemStatus->status & VDSE_TXS_DESTROYED_COMMITTED ||
            txItemStatus->status & VDSE_TXS_EDIT_COMMITTED ) {
            /* Time to really delete the object and the record! */
            vdseFolderRemoveObject( parentFolder,
                                    pFolderItem->pHashItem,
                                    pContext );
         }
      }
      vdseUnlock( &parentFolder->memObject, pContext );

      return 0;
   }
   
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_ENGINE_BUSY );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTopFolderCreateObject( vdseFolder          * pFolder,
                               const char          * objectName,
                               size_t                nameLengthInBytes,
                               vdsObjectDefinition * pDefinition,
                               vdseSessionContext  * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( objectName  != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( pDefinition != NULL );
   VDS_PRE_CONDITION( pDefinition->type > 0 && 
                      pDefinition->type < VDS_LAST_OBJECT_TYPE );
   
   strLength = nameLengthInBytes;

   if ( strLength > VDS_MAX_FULL_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
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
         errcode = VDS_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no vdseUnlock here - the recursive nature of the 
    * function vdseFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( vdseLock( &pFolder->memObject, pContext ) ) {
      rc = vdseFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   pDefinition,
                                   1, /* numBlocks, */
                                   0, /* expectedNumOfChilds, */
                                   pContext );
      if ( rc != 0 ) goto error_handler;
   }
   else {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

int vdseTopFolderDestroyObject( vdseFolder         * pFolder,
                                const char         * objectName,
                                size_t               nameLengthInBytes,
                                vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
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
         errcode = VDS_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no vdseUnlock here - the recursive nature of the 
    * function vdseFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( vdseLock( &pFolder->memObject, pContext ) ) {
      rc = vdseFolderDeleteObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength,
                                   pContext );

      if ( rc != 0 ) goto error_handler;
   }
   else {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

int vdseTopFolderEditObject( vdseFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             enum vdsObjectType   objectType, 
                             vdseFolderItem     * pFolderItem,
                             vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( objectName  != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );

   strLength = nameLengthInBytes;
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
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
      if ( objectType != VDS_FOLDER ) {
         errcode = VDS_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((vdseMemoryHeader *) g_pBaseAddr)->topHashItem;
   }
   else {
      /*
       * There is no vdseUnlock here - the recursive nature of the 
       * function vdseFolderEditObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( vdseLock( &pFolder->memObject, pContext ) ) {
         rc = vdseFolderEditObject( pFolder,
                                    &(lowerName[first]), 
                                    strLength, 
                                    objectType,
                                    pFolderItem,
                                    pContext );
         if ( rc != 0 ) goto error_handler;
      }
      else {
         errcode = VDS_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

int vdseTopFolderGetDef( vdseFolder          * pFolder,
                         const char          * objectName,
                         size_t                nameLengthInBytes,
                         vdsObjectDefinition * pDefinition,
                         vdseFieldDef       ** ppInternalDef,
                         vdseSessionContext  * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder       != NULL );
   VDS_PRE_CONDITION( objectName    != NULL );
   VDS_PRE_CONDITION( pDefinition   != NULL );
   VDS_PRE_CONDITION( ppInternalDef != NULL );
   VDS_PRE_CONDITION( pContext      != NULL );

   *ppInternalDef = NULL;
   
   strLength = nameLengthInBytes;
   
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
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
      if ( vdseLock( &pFolder->memObject, pContext ) ) {
         pDefinition->type = VDS_FOLDER;
         
         vdseUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no vdseUnlock here - the recursive nature of the 
       * function vdseFolderGetDefinition() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( vdseLock( &pFolder->memObject, pContext ) ) {
         rc = vdseFolderGetDefinition( pFolder,
                                       &(lowerName[first]), 
                                       strLength, 
                                       pDefinition,
                                       ppInternalDef,
                                       pContext );
         if ( rc != 0 ) goto error_handler;
      }
      else {
         errcode = VDS_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

int vdseTopFolderGetStatus( vdseFolder         * pFolder,
                            const char         * objectName,
                            size_t               nameLengthInBytes,
                            vdsObjStatus       * pStatus,
                            vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( pStatus    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
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
      if ( vdseLock( &pFolder->memObject, pContext ) ) {
         vdseMemObjectStatus( &pFolder->memObject, pStatus );
         pStatus->type = VDS_FOLDER;
         
         vdseFolderMyStatus( pFolder, pStatus );

         vdseUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no vdseUnlock here - the recursive nature of the 
       * function vdseFolderGetStatus() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( vdseLock( &pFolder->memObject, pContext ) ) {
         rc = vdseFolderGetStatus( pFolder,
                                   &(lowerName[first]), 
                                   strLength, 
                                   pStatus,
                                   pContext );
         if ( rc != VDS_OK ) goto error_handler;
      }
      else {
         errcode = VDS_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

int vdseTopFolderOpenObject( vdseFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             enum vdsObjectType   objectType, 
                             vdseFolderItem     * pFolderItem,
                             vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( objectName  != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );

   strLength = nameLengthInBytes;
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH ) {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
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
      if ( objectType != VDS_FOLDER ) {
         errcode = VDS_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((vdseMemoryHeader *) g_pBaseAddr)->topHashItem;
   }
   else {
      /*
       * There is no vdseUnlock here - the recursive nature of the 
       * function vdseFolderGetObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( vdseLock( &pFolder->memObject, pContext ) ) {
         rc = vdseFolderGetObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength, 
                                   objectType,
                                   pFolderItem,
                                   pContext );
         if ( rc != VDS_OK ) goto error_handler;
      }
      else {
         errcode = VDS_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return 0;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

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

vdsErrors vdseValidateString( const char * objectName,
                              size_t       strLength, 
                              size_t     * pPartialLength,
                              bool       * pLastIteration )
{
   size_t i;
   bool last = true;
   
   /* The first char is always special - it cannot be '/' */
   if ( ! isalpha( (int) objectName[0] )  ) {
      return VDS_INVALID_OBJECT_NAME;
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
         return VDS_INVALID_OBJECT_NAME;
      }
   }
   if ( i > VDS_MAX_NAME_LENGTH ) {
      return VDS_OBJECT_NAME_TOO_LONG;
   }
   
   *pPartialLength = i;
   *pLastIteration = last;

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

