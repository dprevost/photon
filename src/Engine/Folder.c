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

/*
 * gotos... 
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Folder.h"
#include "Transaction.h"
#include "MemoryAllocator.h"

/* To make the code easier to read */
#define SET_ERROR_RETURN(RC) \
{\
   vdscSetError( &pContext->errorHandler, \
                 g_vdsErrorHandle, \
                 RC ); \
         return -1; \
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static bool ValidateString( const vdsChar_T* objectName,
                            size_t           strLength, 
                            size_t*          pPartialLength,
                            bool*            pLastIteration );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseFolderDeletable( vdseFolder*         pFolder,
                          vdseSessionContext* pContext )
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

int vdseFolderInit( vdseFolder*         pFolder,
                    ptrdiff_t           parentOffset,
                    size_t              numberOfPages,
                    size_t              expectedNumOfChilds,
                    vdseTxStatus*       pTxStatus,
                    size_t              origNameLength,
                    vdsChar_T*          origName,
                    vdseSessionContext* pContext )
{
   vdsErrors errcode;
   enum ListErrors listErr;
   
   VDS_PRE_CONDITION( pFolder   != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( pTxStatus != NULL );
   VDS_PRE_CONDITION( origName  != NULL );
   VDS_PRE_CONDITION( numberOfPages  > 0 );
   VDS_PRE_CONDITION( origNameLength > 0 );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   
   errcode = vdseMemObjectInit( &pFolder->memObject, 
                                VDSE_IDENT_FOLDER,
                                numberOfPages );
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
                     parentOffset );
   
   vdsePageGroupInit( &pFolder->pageGroup,
                      SET_OFFSET(pFolder), 
                      numberOfPages ); 

   listErr = vdseHashInit( &pFolder->hashObj, expectedNumOfChilds, pContext );
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

void vdseFolderFini( vdseFolder*         pFolder,
                     vdseSessionContext* pContext )
{
   VDS_PRE_CONDITION( pFolder != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   vdseHashFini(      &pFolder->hashObj, pContext );
   vdsePageGroupFini( &pFolder->pageGroup );
   vdseTreeNodeFini(  &pFolder->nodeObject );
   vdseMemObjectFini( &pFolder->memObject );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderGetObject( vdseFolder*            pFolder,
                         const vdsChar_T*       objectName,
                         size_t                 strLength, 
                         vdseObjectDescriptor** ppDescriptor,
                         vdseSessionContext*    pContext )
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
   
   VDS_PRE_CONDITION( pFolder != NULL );
   VDS_PRE_CONDITION( objectName != NULL )
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( ppDescriptor != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   if ( ! ValidateString( objectName, 
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

      txStatus->usageCounter++;
      *ppDescriptor = pDesc;
      
      vdseUnlock( &pFolder->memObject, pContext );
      return 0;
   }
   
   /* This is not the last node. This node must be a folder, otherwise... */
   if ( pDesc->type != VDS_FOLDER )
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
   rc = vdseLock( &pNextFolder->memObject, pDesc->offset, pContext );
   if ( rc != 0 )
   {
      errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      goto the_exit;
   }
   vdseUnlock( &pFolder->memObject, pContext );
     
   rc = vdseFolderGetObject( pNextFolder,
                                  &objectName[partialLength+1], 
                                  strLength - partialLength - 1, 
                                  ppDescriptor,
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

int vdseFolderInsertObject( vdseFolder*         pFolder,
                            const vdsChar_T*    objectName,
                            const vdsChar_T*    originalName,
                            size_t              strLength, 
                            enum vdsObjectType  objectType,
                            size_t              numPages,
                            size_t              expectedNumOfChilds,
                            vdseSessionContext* pContext )
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
   
   VDS_PRE_CONDITION( pFolder != NULL );
   VDS_PRE_CONDITION( objectName != NULL )
   VDS_PRE_CONDITION( strLength > 0 );
   VDS_PRE_CONDITION( pContext != NULL );
   
   if ( ! ValidateString( objectName, 
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
      
      ptr = (unsigned char*) vdseMallocPages( pContext->pAllocator,
                                     numPages,
                                     pContext );
      if ( ptr == NULL )
      {
         errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         goto the_exit;
      }
      descLength = offsetof(vdseObjectDescriptor, originalName) + 
          partialLength * sizeof(vdsChar_T);
      pDesc = (vdseObjectDescriptor *) malloc( descLength );
      if ( pDesc == NULL )
      {
         vdseFree( pContext->pAllocator, ptr, numPages, pContext );
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto the_exit;
      }
      pDesc->type = objectType;
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
         vdseFree( pContext->pAllocator, ptr, numPages, pContext );
         free( pDesc );
         if ( listErr == LIST_KEY_FOUND )
            errcode = VDS_OBJECT_ALREADY_PRESENT;
         else if ( listErr == LIST_NO_MEMORY )
            errcode = VDS_NOT_ENOUGH_VDS_MEMORY;
         else
            errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      rc = vdseTxAddOps( (vdseTx*)&pContext->pTransaction,
                         VDSE_TX_CREATE,
                         SET_OFFSET(pFolder),
                         VDS_FOLDER,
                         pDesc->offset,
                         pDesc->type,
                         pContext );
      free( pDesc ); 
      pDesc = NULL;
      if ( rc != 0 )
      {
         vdseHashDelete( &pFolder->hashObj, 
                         (unsigned char*)objectName, 
                         partialLength * sizeof(vdsChar_T), 
                         pContext );
         vdseFreePages( pContext->pAllocator, ptr, numPages, pContext );
         goto the_exit;
      }
      
      objTxStatus = &pHashItem->txStatus;
      pDesc = GET_PTR(pHashItem->dataOffset, vdseObjectDescriptor );
      switch ( objectType )
      {
      case VDS_QUEUE:
         break;
      case VDS_FOLDER:
         rc = vdseFolderInit( (vdseFolder*)ptr,
                              SET_OFFSET(pFolder),
                              numPages,
                              expectedNumOfChilds,
                              objTxStatus,
                              partialLength,
                              pDesc->originalName,
                              pContext );
         break;
      
      default:
         errcode = VDS_INTERNAL_ERROR;
         goto the_exit;
      }

      if ( rc != 0 )
      {
         vdseTxRemoveLastOps( (vdseTx*)&pContext->pTransaction, pContext );
         vdseHashDelete( &pFolder->hashObj, 
                         (unsigned char*)objectName, 
                         partialLength * sizeof(vdsChar_T), 
                         pContext );
         vdseFreePages( pContext->pAllocator, ptr, numPages, pContext );
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
   if ( pDesc->type != VDS_FOLDER )
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
   rc = vdseLock( &pNextFolder->memObject, pDesc->offset, pContext );
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
                                    numPages,
                                    expectedNumOfChilds,
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

int vdseFolderDeleteObject( vdseFolder*         pFolder,
                            const vdsChar_T*    objectName,
                            size_t              strLength, 
                            vdseSessionContext* pContext )
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
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pContext   != NULL );
   VDS_PRE_CONDITION( strLength > 0 );
   
   if ( ! ValidateString( objectName, 
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
      if ( pDesc->type == VDS_FOLDER )
      {
         pDeletedFolder = GET_PTR( pDesc->offset, vdseFolder );
         if ( ! vdseFolderDeletable( pFolder, pContext ) )
         {
            errcode = VDS_FOLDER_IS_NOT_EMPTY;
            goto the_exit;
         }
      }
      
      rc = vdseTxAddOps( (vdseTx*)&pContext->pTransaction,
                         VDSE_TX_DESTROY,
                         SET_OFFSET(pFolder),
                         VDS_FOLDER,
                         pDesc->offset,
                         pDesc->type,
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
   if ( pDesc->type != VDS_FOLDER )
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
   rc = vdseLock( &pNextFolder->memObject, pDesc->offset, pContext );
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

bool ValidateString( const vdsChar_T* objectName,
                     size_t           strLength, 
                     size_t*          pPartialLength,
                     bool*            pLastIteration )
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

#if 0

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseFolderRollbackCreate( vdseFolder*         pFolder,
                              ptrdiff_t           childOffset,
                              enum vdsObjectType  objType,
                              vdseSessionContext* pContext )
{
   VDS_PRE_CONDITION( pFolder  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( childOffset != NULL_OFFSET );
   
   BaseNode* pNode = GET_PTR( childOffset, BaseNode, pContext->pAllocator );

   if ( pNode->m_accessCounter > 0 || pNode->m_transactionCounter > 0 )
   {
      pNode->CommitRemove();

      return VDS_OK;
   }

   RemoveObject( pNode, objType, pContext );

/*     this whole function needs to be revisited - see RemoveObject() */

/*     if ( pNode->m_nAccessCounter > 0 ) */
/*     { */
/*        pNode->Commit();  */
      
/*        fprintf( stderr, " Error in Folder::RollbackCreate()\n" ); */
/*        return VDS_OBJECT_IN_USE;   */
/*     } */

       /* Should I lock the node ? */

/*     const ptrdiff_t selfOffset = pNode->GetSelf(); */
   
/*     RowDescriptor* pRow = GET_PTR( selfOffset, RowDescriptor ); */
   
/*  //     enum ListErrors listErr = LIST_OK; */
/*     HashList* pHashList = GET_PTR( m_hashListOffset, HashList ); */

/*     pHashList->DeleteByKey( (char*)pRow + sizeof(RowDescriptor), */
/*                             pRow->nKeyLength, */
/*                             pContext ); */
/*  //     if ( errCode == VDS_OK ) */
/*  //     { */
/*        char* ptr = GET_PTR( zChildObject, char ); */
/*        switch ( nType ) */
/*        { */
/*        case QUEUE: */
/*           ((Queue*)ptr)->Close( pContext ); */
/*           pContext->pAllocator->Free( ptr, pContext->lockValue ); */
/*           break; */
         
/*        case FOLDER: */
/*           ((Folder*)ptr)->Close( pContext ); */
/*           pContext->pAllocator->Free( ptr, pContext->lockValue ); */
/*           break; */
/*        } */
/*  //   } */
   return VDS_OK;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors Folder::RollbackDestroy( ptrdiff_t       childOffset,
                                   vdseSessionContext* pContext )
{
   BaseNode* pNode = GET_PTR( childOffset, BaseNode, pContext->pAllocator );

   int errCode = pNode->Lock( pContext->lockValue );
   if ( errCode == VDS_OK )
   {
      pNode->Commit();
      pNode->UnmarkAsDestroyed();
      pNode->Unlock();

      return VDS_OK;
   }

   return VDS_ENGINE_BUSY;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
#endif

/* 
 * lock on the folder is the responsability of the caller.
 */
void vdseFolderRemoveObject( vdseFolder*         pFolder,
                             vdseMemObject*      pMemObj,
                             const vdsChar_T*    objectName,
                             size_t              nameLength,
                             vdseSessionContext* pContext )
{
   enum ListErrors listErr;
   
   VDS_PRE_CONDITION( pFolder    != NULL );
   VDS_PRE_CONDITION( pMemObj    != NULL );
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


