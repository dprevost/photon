/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Transaction.h"
#include "LogFile.h"
#include "Folder.h"
#include "HashMap.h"
#include "Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxInit( psonTx             * pTx,
                 size_t               numberOfBlocks,
                 psonSessionContext * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( numberOfBlocks  > 0 );
   
   errcode = psonMemObjectInit( &pTx->memObject, 
                                PSON_IDENT_TRANSACTION,
                                &pTx->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }

   psonLinkedListInit( &pTx->listOfOps );

   pTx->signature = PSON_TX_SIGNATURE;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonTxFini( psonTx             * pTx, 
                 psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->listOfOps.currentSize == 0 );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   
   /* Synch the shared memory */
#if 0
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Close and unlink the log file */
   if ( pContext->pLogFile != NULL ) {
      psonCloseLogFile( pContext->pLogFile, &pContext->errorHandler );
   }
   
   psonLinkedListFini( &pTx->listOfOps );
   pTx->signature = 0;
   
   /* This will remove the blocks of allocated memory */
   psonMemObjectFini(  &pTx->memObject, PSON_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxAddOps( psonTx             * pTx,
                   psonTxType           txType,
                   ptrdiff_t            parentOffset, 
                   psonMemObjIdent      parentType, 
                   ptrdiff_t            childOffset,
                   psonMemObjIdent      childType, 
                   psonSessionContext * pContext )
{
   psonTxOps * pOps;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( parentOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( childOffset  != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );

   pOps = (psonTxOps *) psonMalloc( &pTx->memObject,
                                    sizeof(psonTxOps), 
                                    pContext );
   if ( pOps != NULL ) {
      pOps->transType    = txType;
      pOps->parentOffset = parentOffset;
      pOps->parentType   = parentType;
      pOps->childOffset  = childOffset;
      pOps->childType    = childType;

      psonLinkNodeInit(  &pOps->node );
      psonLinkedListPutLast( &pTx->listOfOps, &pOps->node );

      return true;
   }

   psocSetError( &pContext->errorHandler, 
                 g_psoErrorHandle, 
                 PSO_NOT_ENOUGH_PSO_MEMORY );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonTxRemoveLastOps( psonTx             * pTx, 
                          psonSessionContext * pContext )
{
   bool ok;
   psonLinkNode * pDummy = NULL;
   psonTxOps * pOps;

   PSO_PRE_CONDITION( pTx != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   
   ok = psonLinkedListGetLast( &pTx->listOfOps, &pDummy );

   PSO_POST_CONDITION( ok );
   
   pOps = (psonTxOps *)((char *)pDummy - offsetof( psonTxOps, node ));
   
   psonFree( &pTx->memObject,
             (unsigned char *) pOps,
             sizeof(psonTxOps), 
             pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonTxCommit( psonTx             * pTx,
                   psonSessionContext * pContext )
{
   psonTxOps     * pOps = NULL;
   psonLinkNode  * pLinkNode = NULL;
   psonFolder    * parentFolder,    * pChildFolder;
   psonMemObject * pChildMemObject, * pParentMemObject;
   psonTreeNode  * pChildNode;
   psonTxStatus  * pChildStatus;
   psonHashMap   * pHashMap;
   psonQueue     * pQueue;
   int pOps_invalid_type = 0;
   psonHashItem  * pHashItem;
   psonObjectDescriptor * pDesc;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
      
   /* Synch the shared memory */
#if 0
   int errcode = PSO_OK;

   MemoryManager::Instance()->Sync( &pContext->errorHandler );

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = psonLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != PSO_OK ) {
         fprintf(stderr, "Transaction::Commit err1 \n" );
         return -1;
      }
   }
#endif

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to commit */
      return;
   }
   
   while ( psonLinkedListGetFirst( &pTx->listOfOps, &pLinkNode ) ) {

      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode = NULL;
      pChildStatus = NULL;
      pHashMap = NULL;
      pQueue   = NULL;
      
      pOps = (psonTxOps*)
         ((char*)pLinkNode - offsetof( psonTxOps, node ));
      
      switch( pOps->transType ) {

      case PSON_TX_ADD_DATA:

         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psonHashMap );
            pParentMemObject = &pHashMap->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonHashMapCommitAdd( pHashMap, pOps->childOffset, pContext );
            psonUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            pParentMemObject = &pQueue->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonQueueCommitAdd( pQueue, pOps->childOffset );
            psonUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      case PSON_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;
         
         psonLockNoFailure( &parentFolder->memObject, pContext );
         psonLockNoFailure( pChildMemObject, pContext );

         psonTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         psonUnlock( pChildMemObject, pContext );
         psonUnlock( &parentFolder->memObject, pContext );
         
         break;

      case PSON_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         
         psonLockNoFailure( &parentFolder->memObject, pContext );

         psonFolderCommitEdit( parentFolder, 
                               pHashItem, pOps->childType, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         psonUnlock( &parentFolder->memObject, pContext );
         
         break;

      case PSON_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, psonTreeNode );
         pChildStatus = &pHashItem->txStatus;

         psonLockNoFailure( &parentFolder->memObject, pContext );

         psonLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /* 
             * We can't remove it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            psonTxStatusCommitRemove( pChildStatus );
            psonUnlock( pChildMemObject, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            psonUnlock( pChildMemObject, pContext );

            /* 
             * Remove it from the folder (from the hash list)
             * (this function also decrease the txCounter of the parentFolder 
             */
            psonFolderRemoveObject( parentFolder, 
                                    pHashItem,
                                    pContext );
         }
         psonUnlock( &parentFolder->memObject, pContext );

         break;

      case PSON_TX_REMOVE_DATA:
         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psonHashMap );
            pParentMemObject = &pHashMap->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonHashMapCommitRemove( pHashMap,
                                     pOps->childOffset,
                                     pContext );
            psonUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            pParentMemObject = &pQueue->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonQueueCommitRemove( pQueue,
                                   pOps->childOffset,
                                   pContext );
            psonUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
#if 0
      case PSON_TX_SELECT:
         /* Not yet! */
         break;
         
      case PSON_TX_UPDATE:
         /* Not yet! */
         break;
#endif
      } /* end of switch on type of ops */

      psonFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psonTxOps), 
                pContext );
                
   } /* end of while loop on the list of ops */
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonTxRollback( psonTx             * pTx,
                     psonSessionContext * pContext )
{
   int errcode = PSO_OK;
   psonTxOps     * pOps = NULL;
   psonLinkNode  * pLinkNode = NULL;
   psonFolder    * parentFolder,    * pChildFolder;
   psonMemObject * pChildMemObject, * pParentMemObject;
   psonTreeNode  * pChildNode;
   psonTxStatus  * pChildStatus;
   psonHashMap   * pHashMap;
   psonQueue     * pQueue;
   psonHashItem  * pHashItem;
   psonObjectDescriptor * pDesc;
   int pOps_invalid_type = 0;

   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );

#if 0
   /* Synch the shared memory */
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = psonLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != PSO_OK ) {
         fprintf(stderr, "Transaction::Rollback err1 \n" );
         return;
      }
   }

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to rollback */
      return;
   }
   
   while ( psonLinkedListGetLast( &pTx->listOfOps, 
                                  &pLinkNode ) ) {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode   = NULL;
      pChildStatus = NULL;
      pHashMap     = NULL;
      pQueue       = NULL;
      
      pOps = (psonTxOps*)
         ((char*)pLinkNode - offsetof( psonTxOps, node ));

      switch( pOps->transType ) {

      case PSON_TX_ADD_DATA:
         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psonHashMap );
            pParentMemObject = &pHashMap->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonHashMapRollbackAdd( pHashMap, 
                                    pOps->childOffset,
                                    pContext );
            psonUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            pParentMemObject = &pQueue->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonQueueRollbackAdd( pQueue, 
                                  pOps->childOffset,
                                  pContext );
            psonUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
            
      case PSON_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, psonTreeNode );
         pChildStatus = &pHashItem->txStatus;

         psonLockNoFailure( &parentFolder->memObject, pContext );

         psonLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /*
             * Can this really happen? Yes! No other session is supposed to
             * be able to open the object until CREATE is committed but
             * the current session might have open it to insert data and... 
             * (the rollback might be the result of an abnormal error, for
             * example or a call to psoExit() or...).
             */
            /** \todo Revisit this. Maybe rolling back a create object should
             * also automatically close the object if open. */
            /* 
             * We can't "uncreate" it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            psonTxStatusCommitRemove( pChildStatus );
            psonUnlock( pChildMemObject, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            psonUnlock( pChildMemObject, pContext );

            /* Remove it from the folder (from the hash list) */
            psonFolderRemoveObject( parentFolder,
                                    pHashItem,
                                    pContext );
         }
         psonUnlock( &parentFolder->memObject, pContext );

         break;

      case PSON_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         
         psonLockNoFailure( &parentFolder->memObject, pContext );

         psonFolderRollbackEdit( parentFolder, 
                                 pHashItem, pOps->childType, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         psonUnlock( &parentFolder->memObject, pContext );
         
         break;

      case PSON_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;

         psonLockNoFailure( &parentFolder->memObject, pContext );
         psonLockNoFailure( pChildMemObject, pContext );

         psonTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );
         
         psonUnlock( pChildMemObject, pContext );
         psonUnlock( &parentFolder->memObject, pContext );

         break;

      case PSON_TX_REMOVE_DATA:

         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psonHashMap );
            pParentMemObject = &pHashMap->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonHashMapRollbackRemove( pHashMap, 
                                       pOps->childOffset, pContext );
            psonUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            pParentMemObject = &pQueue->memObject;

            psonLockNoFailure( pParentMemObject, pContext );
            psonQueueRollbackRemove( pQueue, 
                                     pOps->childOffset );
            psonUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

#if 0
      case PSON_TX_SELECT:
         /* Not yet! */
         break;
         
      case PSON_TX_UPDATE:
         /* Not yet! */
         break;
#endif         
      }

      psonFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psonTxOps), 
                pContext );
   }
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

