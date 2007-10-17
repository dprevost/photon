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

#include "Folder.h"
#include "Transaction.h"
#include "MemoryAllocator.h"
#include "HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static 
bool vdseValidateString( const vdsChar_T * objectName,
                         size_t            strLength, 
                         size_t          * pPartialLength,
                         bool            * pLastIteration );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
bool vdseFolderDeletable( vdseFolder         * pFolder,
                          vdseSessionContext * pContext )
{
   enum ListErrors listErr;
   size_t bucket, previousBucket;
   ptrdiff_t offset, previousOffset;
   vdseHashItem * pItem;
   ptrdiff_t txOffset = SET_OFFSET( pContext->pTransaction );
   
   /* The easy case */
   if ( pFolder->hashObj.numberOfItems == 0 )
      return true;
   
   /*
    * We loop on all the hash items until either:
    * - we find one item which is not marked as deleted by the
    *   current transaction (we return false)
    * - or the end (we return true)
    */
   
   listErr = vdseHashGetFirst( &pFolder->hashObj,
                               &bucket, 
                               &offset );
   while ( listErr == LIST_OK )
   {
      pItem = GET_PTR( offset, vdseHashItem );
      if ( pItem->txStatus.txOffset != txOffset )
         return false;
      if ( ! vdseTxStatusIsMarkedAsDestroyed( &pItem->txStatus ) )
         return false;
      
      previousBucket = bucket;
      previousOffset = offset;
      
      listErr = vdseHashGetNext( &pFolder->hashObj,
                                 previousBucket,
                                 previousOffset,
                                 &bucket, 
                                 &offset );
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderDeleteObject( vdseFolder         * pFolder,
                            const vdsChar_T    * objectName,
                            size_t               strLength, 
                            vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   int rc;
   vdsErrors errcode = VDS_OK;
   enum ListErrors listErr = LIST_OK;
   vdseObjectDescriptor* pDesc = NULL;
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus* txStatus;
   vdseFolder * pNextFolder, *pDeletedFolder;
   vdseMemObject * pMemObj;
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   if ( ! vdseValidateString( objectName, 
                          strLength, 
                          &partialLength, 
                          &lastIteration ) )
   {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto the_exit;
   }
   listErr = vdseHashGet( &pFolder->hashObj, 
                          (unsigned char *)objectName, 
                          partialLength * sizeof(vdsChar_T),
                          &pHashItem,
                          pContext,
                          NULL );
   if ( listErr != LIST_OK )
   {
      if (lastIteration)
         errcode = VDS_NO_SUCH_OBJECT;
      else
         errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   txStatus = &pHashItem->txStatus;
   
   if ( lastIteration )
   {      
      /* 
       * If the object (to delete) transaction id is not either equal to
       * zero or to the current transaction id, then it belongs to 
       * another transaction - uncommitted. For the current transaction it
       * is as if it does not exist.
       * Similarly, if the object is already marked as destroyed... can't 
       * remove ourselves twice...
       */
      if ( ! vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txStatus ) )
      {
         errcode = VDS_NO_SUCH_OBJECT;
         goto the_exit;
      }

      pDesc = GET_PTR( pHashItem->dataOffset, vdseObjectDescriptor );
      
      /*
       * A special case - folders cannot be deleted if they are not empty
       * (unless all other objects are marked as deleted by the current
       * transaction).
       *
       * Other objects contain data, not objects. 
       */
      if ( pDesc->apiType == VDS_FOLDER )
      {
         pDeletedFolder = GET_PTR( pDesc->offset, vdseFolder );
         if ( ! vdseFolderDeletable( pDeletedFolder, pContext ) )
         {
            errcode = VDS_FOLDER_IS_NOT_EMPTY;
            goto the_exit;
         }
      }
      pMemObj = GET_PTR( pDesc->memOffset, vdseMemObject );
      
      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_REMOVE_OBJECT,
                         SET_OFFSET(pFolder),
                         VDSE_IDENT_FOLDER,
                         pDesc->offset,
                         pMemObj->objType,
                         pContext );
      if ( rc != 0 )
         goto the_exit;
      
      vdseTxStatusSetTx( txStatus, SET_OFFSET(pContext->pTransaction) );
      vdseTxStatusMarkAsDestroyed( txStatus );
      vdseUnlock( &pFolder->memObject, pContext );

      return 0;
   }

   /* If we come here, this was not the last iteration, so we continue */

   /* This is not the last node. This node must be a folder, otherwise... */
   pDesc = GET_PTR( pHashItem->dataOffset, vdseObjectDescriptor );
   if ( pDesc->apiType != VDS_FOLDER )
   {
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
         || vdseTxStatusIsMarkedAsDestroyed( txStatus ) )
   {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }

   pNextFolder = GET_PTR( pDesc->offset, vdseFolder );
   rc = vdseLock( &pNextFolder->memObject, pContext );
   if ( rc != 0 )
   {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   vdseUnlock( &pFolder->memObject, pContext );

   rc = vdseFolderDeleteObject( pNextFolder,
                                &objectName[partialLength+1],
                                strLength - partialLength - 1,
                                pContext );
   return rc;
   
the_exit:

   /* vdscSetError might have been already called by some other function */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

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

   vdseHashFini(       &pFolder->hashObj, pContext );
   vdseTreeNodeFini(   &pFolder->nodeObject );
   vdseMemObjectFini(  &pFolder->memObject, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderGetFirst( vdseFolder         * pFolder,
                        vdseFolderItem     * pItem,
                        vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem = NULL;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txFolderStatus;
   size_t     bucket;
   ptrdiff_t  firstItemOffset;

   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pItem    != NULL )
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   txFolderStatus = GET_PTR( pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   if ( vdseLock( &pFolder->memObject, pContext ) == 0 )
   {
      listErr = vdseHashGetFirst( &pFolder->hashObj, 
                                  &bucket,
                                  &firstItemOffset );
      while ( listErr == LIST_OK )
      {
         pHashItem = GET_PTR( firstItemOffset, vdseHashItem );
         txItemStatus = &pHashItem->txStatus;
         if ( vdseTxStatusIsValid( txItemStatus, SET_OFFSET(pContext->pTransaction) ) 
             && ! vdseTxStatusIsMarkedAsDestroyed( txItemStatus ) )
         {
            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->bucket = bucket;
            pItem->itemOffset = firstItemOffset;

            vdseUnlock( &pFolder->memObject, pContext );
            
            return 0;
         }
  
         listErr = vdseHashGetNext( &pFolder->hashObj, 
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
   
   vdseUnlock( &pFolder->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_IS_EMPTY );

   return -1;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderGetNext( vdseFolder         * pFolder,
                       vdseFolderItem     * pItem,
                       vdseSessionContext * pContext )
{
   enum ListErrors listErr = LIST_OK;
   vdseHashItem * pHashItem = NULL;
   vdseHashItem * previousHashItem = NULL;
   vdseTxStatus * txItemStatus;
   vdseTxStatus * txFolderStatus;
   size_t     bucket;
   ptrdiff_t  itemOffset;

   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pItem    != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );
   VDS_PRE_CONDITION( pItem->pHashItem  != NULL );
   VDS_PRE_CONDITION( pItem->itemOffset != NULL_OFFSET );
   
   txFolderStatus = GET_PTR( pFolder->nodeObject.txStatusOffset, vdseTxStatus );

   bucket           = pItem->bucket;
   itemOffset       = pItem->itemOffset;
   previousHashItem = pItem->pHashItem;
   
   if ( vdseLock( &pFolder->memObject, pContext ) == 0 )
   {
      listErr = vdseHashGetNext( &pFolder->hashObj, 
                                 bucket,
                                 itemOffset,
                                 &bucket,
                                 &itemOffset );
      while ( listErr == LIST_OK )
      {
         pHashItem = GET_PTR( itemOffset, vdseHashItem );
         txItemStatus = &pHashItem->txStatus;
         if ( vdseTxStatusIsValid( txItemStatus, SET_OFFSET(pContext->pTransaction) ) 
             && ! vdseTxStatusIsMarkedAsDestroyed( txItemStatus ) )
         {
            txItemStatus->parentCounter++;
            txFolderStatus->usageCounter++;
            pItem->pHashItem = pHashItem;
            pItem->bucket = bucket;
            pItem->itemOffset = itemOffset;
//               vdseFolderReleaseNoLock( pFolder, previousHashItem, pContext );

            vdseUnlock( &pFolder->memObject, pContext );
            
            return 0;
         }
  
         listErr = vdseHashGetNext( &pFolder->hashObj, 
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
   
   vdseUnlock( &pFolder->memObject, pContext );
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_REACHED_THE_END );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderGetObject( vdseFolder         * pFolder,
                         const vdsChar_T    * objectName,
                         size_t               strLength, 
                         vdseFolderItem     * pFolderItem,
                         vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   enum ListErrors listErr = LIST_OK;
   vdseObjectDescriptor* pDesc = NULL;
   vdseHashItem* pHashItem = NULL;
   int rc;
   vdsErrors errcode;
   vdseTxStatus* txStatus;
   vdseFolder* pNextFolder;
   
   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( objectName  != NULL )
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   if ( ! vdseValidateString( objectName, 
                          strLength, 
                          &partialLength, 
                          &lastIteration ) )
   {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto the_exit;
   }
   
   listErr = vdseHashGet( &pFolder->hashObj, 
                          (unsigned char *)objectName, 
                          partialLength * sizeof(vdsChar_T), 
                          &pHashItem,
                          pContext,
                          NULL );
   if ( listErr != LIST_OK )
   {
      if (lastIteration) 
         errcode = VDS_NO_SUCH_OBJECT;
      else
         errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }
   txStatus = &pHashItem->txStatus;

   pDesc = GET_PTR( pHashItem->dataOffset, vdseObjectDescriptor );
   
   if ( lastIteration )
   {
      /* 
       * If the transaction id of the object (to retrieve) is not either equal
       * to zero or to the current transaction id, then it belongs to 
       * another transaction - uncommitted. For the current transaction it
       * is as if it does not exist.
       * Similarly, if the object is marked as destroyed... we can't access it. 
       * (to have the id as ok and be marked as destroyed is a rare case - 
       * it would require that the current transaction deleted the folder and 
       * than tries to access it).
       */
      if ( ! vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txStatus ) )
      {
         errcode = VDS_NO_SUCH_OBJECT;
         goto the_exit;
      }

      txStatus->parentCounter++;
      pFolderItem->pHashItem = pHashItem;
      
      vdseUnlock( &pFolder->memObject, pContext );

      return 0;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->apiType != VDS_FOLDER )
   {
      errcode = VDS_NO_SUCH_OBJECT;
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
   if ( ! vdseTxStatusIsValid( txStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( txStatus ) )
   {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }
   
   pNextFolder = GET_PTR( pDesc->offset, vdseFolder );
   rc = vdseLock( &pNextFolder->memObject, pContext );
   if ( rc != 0 )
   {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   vdseUnlock( &pFolder->memObject, pContext );
     
   rc = vdseFolderGetObject( pNextFolder,
                             &objectName[partialLength+1], 
                             strLength - partialLength - 1, 
                             pFolderItem,
                             pContext );
   
   return rc;

the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   vdseUnlock( &pFolder->memObject, pContext );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderInit( vdseFolder         * pFolder,
                    ptrdiff_t            parentOffset,
                    size_t               numberOfBlocks,
                    size_t               expectedNumOfChilds,
                    vdseTxStatus       * pTxStatus,
                    size_t               origNameLength,
                    vdsChar_T          * origName,
                    ptrdiff_t            keyOffset,
                    vdseSessionContext * pContext )
{
   vdsErrors errcode;
   enum ListErrors listErr;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pTxStatus != NULL );
   VDS_PRE_CONDITION( origName  != NULL );
   VDS_PRE_CONDITION( keyOffset    != NULL_OFFSET );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   
   errcode = vdseMemObjectInit( &pFolder->memObject, 
                                VDSE_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK )
   {
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
                     keyOffset );

   listErr = vdseHashInit( &pFolder->hashObj,
                           SET_OFFSET(&pFolder->memObject),
                           expectedNumOfChilds, 
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
// ici

int vdseFolderInsertObject( vdseFolder         * pFolder,
                            const vdsChar_T    * objectName,
                            const vdsChar_T    * originalName,
                            size_t               strLength, 
                            enum vdsObjectType   objectType,
                            size_t               numBlocks,
                            size_t               expectedNumOfChilds,
                            vdseSessionContext * pContext )
{
   bool lastIteration = true;
   size_t partialLength = 0;
   enum ListErrors listErr = LIST_OK;
   vdseHashItem* pHashItem;
   int rc;
   vdsErrors errcode = VDS_OK;
   vdseObjectDescriptor* pDesc = NULL;
   size_t descLength;
   unsigned char* ptr = NULL;
   vdseFolder* pNextFolder;
   vdseTxStatus* objTxStatus;  /* txStatus of the created object */
   vdseMemObjIdent memObjType;
   
   VDS_PRE_CONDITION( pFolder      != NULL );
   VDS_PRE_CONDITION( objectName   != NULL )
   VDS_PRE_CONDITION( originalName != NULL )
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pFolder->memObject.objType == VDSE_IDENT_FOLDER );

   if ( ! vdseValidateString( objectName, 
                          strLength, 
                          &partialLength, 
                          &lastIteration ) )
   {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto the_exit;
   }
   
   if ( lastIteration )
   {
      /* 
       * We are now ready to create the object. The steps require for this
       * are:
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
      
      ptr = (unsigned char*) vdseMallocBlocks( pContext->pAllocator,
                                               numBlocks,
                                               pContext );
      if ( ptr == NULL )
      {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
      descLength = offsetof(vdseObjectDescriptor, originalName) + 
          (partialLength+1) * sizeof(vdsChar_T);
      pDesc = (vdseObjectDescriptor *) malloc( descLength );
      if ( pDesc == NULL )
      {
         vdseFreeBlocks( pContext->pAllocator, ptr, numBlocks, pContext );
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      memset( pDesc, 0, descLength );
      pDesc->apiType = objectType;
      pDesc->offset = SET_OFFSET( ptr );
      pDesc->nameLengthInBytes = partialLength * sizeof(vdsChar_T);
      memcpy( pDesc->originalName, originalName, pDesc->nameLengthInBytes );

      listErr = vdseHashInsert( &pFolder->hashObj, 
                                (unsigned char *)objectName, 
                                partialLength * sizeof(vdsChar_T), 
                                (void*)pDesc, 
                                descLength,
                                &pHashItem,
                                pContext );
      if ( listErr != LIST_OK )
      {
         vdseFreeBlocks( pContext->pAllocator, ptr, numBlocks, pContext );
         free( pDesc );
         if ( listErr == LIST_KEY_FOUND )
            errcode = VDS_OBJECT_ALREADY_PRESENT;
         else if ( listErr == LIST_NO_MEMORY )
            errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         else
            errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      switch( objectType )
      {
      case VDS_FOLDER:
         memObjType = VDSE_IDENT_FOLDER;
         break;
      case VDS_HASH_MAP:
         memObjType = VDSE_IDENT_HASH_MAP;
         break;
      case VDS_QUEUE:
         memObjType = VDSE_IDENT_QUEUE;
         break;
      default:
         VDS_POST_CONDITION( 0 == 1 );
      }
      
      rc = vdseTxAddOps( (vdseTx*)pContext->pTransaction,
                         VDSE_TX_ADD_OBJECT,
                         SET_OFFSET(pFolder),
                         VDSE_IDENT_FOLDER,
                         pDesc->offset,
                         memObjType,
                         pContext );
      free( pDesc ); 
      pDesc = NULL;
      if ( rc != 0 )
      {
         vdseHashDelete( &pFolder->hashObj, 
                         (unsigned char*)objectName, 
                         partialLength * sizeof(vdsChar_T), 
                         pContext );
         vdseFreeBlocks( pContext->pAllocator, ptr, numBlocks, pContext );
         goto the_exit;
      }
      
      objTxStatus = &pHashItem->txStatus;
      vdseTxStatusSetTx( objTxStatus, SET_OFFSET(pContext->pTransaction) );
      
      pDesc = GET_PTR(pHashItem->dataOffset, vdseObjectDescriptor );
      switch ( memObjType )
      {
      case VDSE_IDENT_QUEUE:
         break;

      case VDSE_IDENT_FOLDER:
         rc = vdseFolderInit( (vdseFolder*)ptr,
                              SET_OFFSET(pFolder),
                              numBlocks,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              SET_OFFSET(pHashItem->key),
                              pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseFolder,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseFolder,memObject);
         break;
      
      case VDSE_IDENT_HASH_MAP:
         rc = vdseHashMapInit( (vdseHashMap *)ptr,
                              SET_OFFSET(pFolder),
                              numBlocks,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              SET_OFFSET(pHashItem->key),
                              pContext );
         pDesc->nodeOffset = SET_OFFSET(ptr) + offsetof(vdseFolder,nodeObject);
         pDesc->memOffset  = SET_OFFSET(ptr) + offsetof(vdseFolder,memObject);
         break;

      default:
         errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      if ( rc != 0 )
      {
         vdseTxRemoveLastOps( (vdseTx*)pContext->pTransaction, pContext );
         vdseHashDelete( &pFolder->hashObj, 
                         (unsigned char*)objectName, 
                         partialLength * sizeof(vdsChar_T), 
                         pContext );
         vdseFreeBlocks( pContext->pAllocator, ptr, numBlocks, pContext );
         goto the_exit;
      }

      vdseUnlock( &pFolder->memObject, pContext );
      return 0;
   }
   
   /* If we come here, this was not the last iteration, so we continue */
   listErr = vdseHashGet( &pFolder->hashObj, 
                          (unsigned char *)objectName, 
                          partialLength * sizeof(vdsChar_T), 
                          &pHashItem,
                          pContext,
                          NULL );
   if ( listErr != LIST_OK )
   {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   pDesc = GET_PTR( pHashItem->dataOffset, vdseObjectDescriptor );
   if ( pDesc->apiType != VDS_FOLDER )
   {
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
   if ( ! vdseTxStatusIsValid( &pHashItem->txStatus, SET_OFFSET(pContext->pTransaction) ) 
         || vdseTxStatusIsMarkedAsDestroyed( &pHashItem->txStatus ) )
   {
      errcode = VDS_NO_SUCH_FOLDER;
      goto the_exit;
   }
   
   pNextFolder = GET_PTR( pDesc->offset, vdseFolder );   
   rc = vdseLock( &pNextFolder->memObject, pContext );
   if ( rc != 0 )
   {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   
   vdseUnlock( &pFolder->memObject, pContext );

   rc = vdseFolderInsertObject( pNextFolder,
                                &objectName[partialLength+1],
                                &originalName[partialLength+1],
                                strLength - partialLength - 1,
                                objectType,
                                numBlocks,
                                expectedNumOfChilds,
                                pContext );
   return rc;
   
the_exit:

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   vdseUnlock( &pFolder->memObject, pContext );
   
   return -1;
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
   txFolderStatus = GET_PTR( pFolder->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( vdseLock( &pFolder->memObject, pContext ) == 0 )
   {
      txItemStatus->parentCounter--;
      txFolderStatus->usageCounter--;

      /* 
       * if parentCounter is equal to zero, the object is not open. Since 
       * we hold the lock on the folder, no session can therefore open it
       * or use it in an iteration. We can remove it without problems if
       * a remove was indeed committed.
       */
      if ( (txItemStatus->parentCounter == 0) && 
         (txItemStatus->usageCounter == 0) &&
         vdseTxStatusIsRemoveCommitted(txItemStatus) )
      {
         /* 
          * Time to really delete the record!
          *
          * Note: the hash array will release the memory of the hash item.
          */
         vdseHashDelete( &pFolder->hashObj, 
                         pFolderItem->pHashItem->key, 
                         pFolderItem->pHashItem->keyLength, 
                         pContext );

         pFolder->nodeObject.txCounter--;
      }
      vdseUnlock( &pFolder->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_OBJECT_CANNOT_GET_LOCK );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * lock on the folder is the responsability of the caller.
 */
void vdseFolderRemoveObject( vdseFolder         * pFolder,
                             const vdsChar_T    * objectName,
                             size_t               nameLength,
                             vdseSessionContext * pContext )
{
   enum ListErrors listErr;
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( nameLength > 0 );

   listErr = vdseHashDelete( &pFolder->hashObj,
                             (unsigned char*)objectName,
                             nameLength * sizeof(vdsChar_T),
                             pContext );
   
   VDS_POST_CONDITION( listErr == LIST_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 4 functions should only be used by the API, to create, destroy,
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

   pDesc = GET_PTR( pFolderItem->pHashItem->dataOffset, vdseObjectDescriptor );
   pNode = GET_PTR( pDesc->nodeOffset, vdseTreeNode);
   
   parentFolder = GET_PTR( pNode->myParentOffset, vdseFolder );
   txFolderStatus = GET_PTR( parentFolder->nodeObject.txStatusOffset, vdseTxStatus );
   
   if ( vdseLock( &parentFolder->memObject, pContext ) == 0 )
   {
      txItemStatus = GET_PTR(pNode->txStatusOffset, vdseTxStatus );
      txItemStatus->parentCounter--;
      
      /* 
       * if parentCounter is equal to zero, the object is not open. Since 
       * we hold the lock on the folder, no session can therefore open it
       * or use it in an iteration. We can remove it without problems if
       * a remove was indeed committed.
       */
      if ( (txItemStatus->parentCounter == 0) && 
         (txItemStatus->usageCounter == 0) &&
         vdseTxStatusIsRemoveCommitted(txItemStatus) )
      {
         /* 
          * Time to really delete the record!
          *
          * Note: the hash array will release the memory of the hash item.
          */
         vdseHashDelete( &parentFolder->hashObj, 
                         pFolderItem->pHashItem->key, 
                         pFolderItem->pHashItem->keyLength, 
                         pContext );

         parentFolder->nodeObject.txCounter--;
      }      

      vdseUnlock( &parentFolder->memObject, pContext );

      return 0;
   }
   
   vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_ENGINE_BUSY );
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTopFolderCreateObject( vdseFolder         * pFolder,
                               const char         * objectName,
                               size_t               nameLengthInBytes,
                               enum vdsObjectType   objectType,
                               vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   wchar_t * name = NULL, *lowerName = NULL;
   const char * strPtr;
#else
   const char * name = objectName;
   char * lowerName = NULL;
#endif

   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );

#if VDS_SUPPORT_i18n
   memset( &ps, 0, sizeof(mbstate_t) );
   strPtr = objectName;
   strLength = mbsrtowcs( NULL, &strPtr, 0, &ps );
#else
   strLength = nameLengthInBytes;
#endif

   if ( strLength > VDS_MAX_FULL_NAME_LENGTH )
   {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) 
   {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

#if VDS_SUPPORT_i18n
   name = (wchar_t*)malloc( (strLength+1)*sizeof(wchar_t) );
   if ( name == NULL ) 
   {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   lowerName = (wchar_t*)malloc( (strLength+1)*sizeof(wchar_t) );
   
   strPtr = objectName;
   mbsrtowcs( name, &strPtr, strLength, &ps );
#else
   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
#endif
   if ( lowerName == NULL ) 
   {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i )
      lowerName[i] = (vdsChar_T) vds_tolower( name[i] );
   
   /* strip the first char if a separator */
   if ( name[0] == VDS_SLASH || name[0] == VDS_BACKSLASH )
   {
      first = 1;
      --strLength;
      if ( strLength == 0 )
      {
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
   if ( vdseLock( &pFolder->memObject, pContext ) == 0 )
   {
      rc = vdseFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   objectType,
                                   1, /* numBlocks, */
                                   0, /* expectedNumOfChilds, */
                                   pContext );
      if ( rc != 0 ) goto error_handler;
   }
   else
   {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
#if VDS_SUPPORT_i18n
   free( name );
#endif
   free( lowerName );
   
   return 0;

error_handler:

#if VDS_SUPPORT_i18n
   if ( name != NULL )
      free( name );
#endif
   if ( lowerName != NULL )
      free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

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
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   wchar_t * name = NULL, * lowerName = NULL;
   const char * strPtr;
#else
   const char * name = objectName;
   char * lowerName = NULL;
#endif

   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );

#if VDS_SUPPORT_i18n
   memset( &ps, 0, sizeof(mbstate_t) );
   strPtr = objectName;
   strLength = mbsrtowcs( NULL, &strPtr, 0, &ps );
#else
   strLength = nameLengthInBytes;
#endif
   
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH )
   {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) 
   {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

#if VDS_SUPPORT_i18n
   name = (wchar_t *)malloc( (strLength+1)*sizeof(wchar_t) );
   if ( name == NULL ) 
   {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   lowerName = (wchar_t *) malloc( (strLength+1)*sizeof(wchar_t) );
   
   strPtr = objectName;
   mbsrtowcs( name, &strPtr, strLength, &ps );
#else
   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
#endif
   if ( lowerName == NULL ) 
   {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i )
      lowerName[i] = (vdsChar_T) vds_tolower( name[i] );
   
   /* strip the first char if a separator */
   if ( name[0] == VDS_SLASH || name[0] == VDS_BACKSLASH )
   {
      first = 1;
      --strLength;
      if ( strLength == 0 )
      {
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
   if ( vdseLock( &pFolder->memObject, pContext ) == 0 )
   {
      rc = vdseFolderDeleteObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength,
                                   pContext );

      if ( rc != 0 ) goto error_handler;
   }
   else
   {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
#if VDS_SUPPORT_i18n
   free( name );
#endif
   free( lowerName );
   
   return 0;

error_handler:

#if VDS_SUPPORT_i18n
   if ( name != NULL )
      free( name );
#endif
   if ( lowerName != NULL )
      free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTopFolderOpenObject( vdseFolder         * pFolder,
                             const char         * objectName,
                             size_t               nameLengthInBytes,
                             vdseFolderItem     * pFolderItem,
                             vdseSessionContext * pContext )
{
   vdsErrors errcode = VDS_OK;
   size_t strLength, i;
   int rc;
   size_t first = 0;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   wchar_t * name = NULL, * lowerName = NULL;
   const char * strPtr;
#else
   const char * name = objectName;
   char * lowerName = NULL;
#endif

   VDS_PRE_CONDITION( pFolder     != NULL );
   VDS_PRE_CONDITION( pFolderItem != NULL );
   VDS_PRE_CONDITION( objectName  != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );

#if VDS_SUPPORT_i18n
   memset( &ps, 0, sizeof(mbstate_t) );
   strPtr = objectName;
   strLength = mbsrtowcs( NULL, &strPtr, 0, &ps );
#else
   strLength = nameLengthInBytes;
#endif
   
   if ( strLength > VDS_MAX_FULL_NAME_LENGTH )
   {
      errcode = VDS_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) 
   {
      errcode = VDS_INVALID_OBJECT_NAME;
      goto error_handler;
   }

#if VDS_SUPPORT_i18n
   name = (wchar_t *) malloc( (strLength+1)*sizeof(wchar_t) );
   if ( name == NULL ) 
   {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }
   lowerName = (wchar_t *) malloc( (strLength+1)*sizeof(wchar_t) );
   
   strPtr = objectName;
   mbsrtowcs( name, &strPtr, strLength, &ps );
#else
   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
#endif
   if ( lowerName == NULL ) 
   {
      errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i )
      lowerName[i] = (vdsChar_T) vds_tolower( name[i] );
   
   /* strip the first char if a separator */
   if ( name[0] == VDS_SLASH || name[0] == VDS_BACKSLASH )
   {
      first = 1;
      --strLength;
      if ( strLength == 0 )
      {
         errcode = VDS_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no vdseUnlock here - the recursive nature of the 
    * function vdseFolderGetObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( vdseLock( &pFolder->memObject, pContext ) == 0 )
   {
      rc = vdseFolderGetObject( pFolder,
                                &(lowerName[first]), 
                                strLength, 
                                pFolderItem,
                                pContext );
      if ( rc != 0 ) goto error_handler;
   }
   else
   {
      errcode = VDS_ENGINE_BUSY;
      goto error_handler;
   }
   
#if VDS_SUPPORT_i18n
   free( name );
#endif
   free( lowerName );
   
   return 0;

error_handler:

#if VDS_SUPPORT_i18n
   if ( name != NULL )
      free( name );
#endif
   if ( lowerName != NULL )
      free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called vdscSetError. 
    */
   if ( errcode != VDS_OK )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseValidateString( const vdsChar_T * objectName,
                         size_t            strLength, 
                         size_t          * pPartialLength,
                         bool            * pLastIteration )
{
   size_t i;
   *pLastIteration = true;
   
   /* The first char is always special - it cannot be '/' */
   if ( ! vds_isalnum( (int) objectName[0] )  )
   {
      return false;
   }
   
   for ( i = 1; i < strLength; ++i )
   {
      if ( objectName[i] == VDS_SLASH || objectName[i] == VDS_BACKSLASH )
      {
         *pLastIteration = false;
         /* Strip the last character if it is a separator (in other words */
         /* we keep lastIteration to true - we have found the end of the */
         /* "path". */
         if ( i == (strLength-1) )
            *pLastIteration = true;
         break;
      }
      if ( !( vds_isalnum( (int) objectName[i] ) 
              || (objectName[i] == VDS_SPACE) 
              || (objectName[i] == VDS_UNDERSCORE) 
              || (objectName[i] == VDS_HYPHEN) ) ) 
      {
         return false;
      }
   }
   *pPartialLength = i;

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

