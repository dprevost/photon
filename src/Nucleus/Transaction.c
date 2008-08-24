/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

bool psnTxInit( psnTx             * pTx,
                 size_t               numberOfBlocks,
                 psnSessionContext * pContext )
{
   psoErrors errcode;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( numberOfBlocks  > 0 );
   
   errcode = psnMemObjectInit( &pTx->memObject, 
                                PSN_IDENT_TRANSACTION,
                                &pTx->blockGroup,
                                numberOfBlocks );
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      return false;
   }

   psnLinkedListInit( &pTx->listOfOps );

   pTx->signature = PSN_TX_SIGNATURE;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnTxFini( psnTx             * pTx, 
                 psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->listOfOps.currentSize == 0 );
   PSO_PRE_CONDITION( pTx->signature == PSN_TX_SIGNATURE );
   
   /* Synch the VDS */
#if 0
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Close and unlink the log file */
   if ( pContext->pLogFile != NULL ) {
      psnCloseLogFile( pContext->pLogFile, &pContext->errorHandler );
   }
   
   psnLinkedListFini( &pTx->listOfOps );
   pTx->signature = 0;
   
   /* This will remove the blocks of allocated memory */
   psnMemObjectFini(  &pTx->memObject, PSN_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnTxAddOps( psnTx             * pTx,
                   psnTxType           txType,
                   ptrdiff_t            parentOffset, 
                   psnMemObjIdent      parentType, 
                   ptrdiff_t            childOffset,
                   psnMemObjIdent      childType, 
                   psnSessionContext * pContext )
{
   psnTxOps * pOps;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( parentOffset != PSN_NULL_OFFSET );
   PSO_PRE_CONDITION( childOffset  != PSN_NULL_OFFSET );
   PSO_PRE_CONDITION( pTx->signature == PSN_TX_SIGNATURE );

   pOps = (psnTxOps *) psnMalloc( &pTx->memObject,
                                    sizeof(psnTxOps), 
                                    pContext );
   if ( pOps != NULL ) {
      pOps->transType    = txType;
      pOps->parentOffset = parentOffset;
      pOps->parentType   = parentType;
      pOps->childOffset  = childOffset;
      pOps->childType    = childType;

      psnLinkNodeInit(  &pOps->node );
      psnLinkedListPutLast( &pTx->listOfOps, &pOps->node );

      return true;
   }

   pscSetError( &pContext->errorHandler, 
                 g_psoErrorHandle, 
                 PSO_NOT_ENOUGH_PSO_MEMORY );

   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnTxRemoveLastOps( psnTx             * pTx, 
                          psnSessionContext * pContext )
{
   bool ok;
   psnLinkNode * pDummy = NULL;
   psnTxOps * pOps;

   PSO_PRE_CONDITION( pTx != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSN_TX_SIGNATURE );
   
   ok = psnLinkedListGetLast( &pTx->listOfOps, &pDummy );

   PSO_POST_CONDITION( ok );
   
   pOps = (psnTxOps *)((char *)pDummy - offsetof( psnTxOps, node ));
   
   psnFree( &pTx->memObject,
             (unsigned char *) pOps,
             sizeof(psnTxOps), 
             pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnTxCommit( psnTx             * pTx,
                   psnSessionContext * pContext )
{
   psnTxOps     * pOps = NULL;
   psnLinkNode  * pLinkNode = NULL;
   psnFolder    * parentFolder,    * pChildFolder;
   psnMemObject * pChildMemObject, * pParentMemObject;
   psnTreeNode  * pChildNode;
   psnTxStatus  * pChildStatus;
   psnHashMap   * pHashMap;
   psnQueue     * pQueue;
   int pOps_invalid_type = 0;
   psnHashItem  * pHashItem;
   psnObjectDescriptor * pDesc;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSN_TX_SIGNATURE );
      
   /* Synch the VDS */
#if 0
   int errcode = PSO_OK;

   MemoryManager::Instance()->Sync( &pContext->errorHandler );

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = psnLogTransaction( pContext->pLogFile, 
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
   
   while ( psnLinkedListGetFirst( &pTx->listOfOps, &pLinkNode ) ) {

      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode = NULL;
      pChildStatus = NULL;
      pHashMap = NULL;
      pQueue   = NULL;
      
      pOps = (psnTxOps*)
         ((char*)pLinkNode - offsetof( psnTxOps, node ));
      
      switch( pOps->transType ) {

      case PSN_TX_ADD_DATA:

         if ( pOps->parentType == PSN_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psnHashMap );
            pParentMemObject = &pHashMap->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnHashMapCommitAdd( pHashMap, pOps->childOffset, pContext );
            psnUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSN_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psnQueue );
            pParentMemObject = &pQueue->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnQueueCommitAdd( pQueue, pOps->childOffset );
            psnUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      case PSN_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSN_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psnFolder );
         GET_PTR( pHashItem, pOps->childOffset, psnHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psnMemObject );
         pChildStatus = &pHashItem->txStatus;
         
         psnLockNoFailure( &parentFolder->memObject, pContext );
         psnLockNoFailure( pChildMemObject, pContext );

         psnTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psnFolderResize( parentFolder, pContext );

         psnUnlock( pChildMemObject, pContext );
         psnUnlock( &parentFolder->memObject, pContext );
         
         break;

      case PSN_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSN_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psnFolder );
         GET_PTR( pHashItem, pOps->childOffset, psnHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
         
         psnLockNoFailure( &parentFolder->memObject, pContext );

         psnFolderCommitEdit( parentFolder, 
                               pHashItem, pOps->childType, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psnFolderResize( parentFolder, pContext );

         psnUnlock( &parentFolder->memObject, pContext );
         
         break;

      case PSN_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSN_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psnFolder );
         GET_PTR( pHashItem, pOps->childOffset, psnHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psnMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, psnTreeNode );
         pChildStatus = &pHashItem->txStatus;

         psnLockNoFailure( &parentFolder->memObject, pContext );

         psnLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /* 
             * We can't remove it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            psnTxStatusCommitRemove( pChildStatus );
            psnUnlock( pChildMemObject, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            psnUnlock( pChildMemObject, pContext );

            /* 
             * Remove it from the folder (from the hash list)
             * (this function also decrease the txCounter of the parentFolder 
             */
            psnFolderRemoveObject( parentFolder, 
                                    pHashItem,
                                    pContext );
         }
         psnUnlock( &parentFolder->memObject, pContext );

         break;

      case PSN_TX_REMOVE_DATA:
         if ( pOps->parentType == PSN_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psnHashMap );
            pParentMemObject = &pHashMap->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnHashMapCommitRemove( pHashMap,
                                     pOps->childOffset,
                                     pContext );
            psnUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSN_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psnQueue );
            pParentMemObject = &pQueue->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnQueueCommitRemove( pQueue,
                                   pOps->childOffset,
                                   pContext );
            psnUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
#if 0
      case PSN_TX_SELECT:
         /* Not yet! */
         break;
         
      case PSN_TX_UPDATE:
         /* Not yet! */
         break;
#endif
      } /* end of switch on type of ops */

      psnFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psnTxOps), 
                pContext );
                
   } /* end of while loop on the list of ops */
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnTxRollback( psnTx             * pTx,
                     psnSessionContext * pContext )
{
   int errcode = PSO_OK;
   psnTxOps     * pOps = NULL;
   psnLinkNode  * pLinkNode = NULL;
   psnFolder    * parentFolder,    * pChildFolder;
   psnMemObject * pChildMemObject, * pParentMemObject;
   psnTreeNode  * pChildNode;
   psnTxStatus  * pChildStatus;
   psnHashMap   * pHashMap;
   psnQueue     * pQueue;
   psnHashItem  * pHashItem;
   psnObjectDescriptor * pDesc;
   int pOps_invalid_type = 0;

   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSN_TX_SIGNATURE );

#if 0
   /* Synch the VDS */
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = psnLogTransaction( pContext->pLogFile, 
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
   
   while ( psnLinkedListGetLast( &pTx->listOfOps, 
                                  &pLinkNode ) ) {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode   = NULL;
      pChildStatus = NULL;
      pHashMap     = NULL;
      pQueue       = NULL;
      
      pOps = (psnTxOps*)
         ((char*)pLinkNode - offsetof( psnTxOps, node ));

      switch( pOps->transType ) {

      case PSN_TX_ADD_DATA:
         if ( pOps->parentType == PSN_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psnHashMap );
            pParentMemObject = &pHashMap->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnHashMapRollbackAdd( pHashMap, 
                                    pOps->childOffset,
                                    pContext );
            psnUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSN_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psnQueue );
            pParentMemObject = &pQueue->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnQueueRollbackAdd( pQueue, 
                                  pOps->childOffset,
                                  pContext );
            psnUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
            
      case PSN_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSN_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psnFolder );
         GET_PTR( pHashItem, pOps->childOffset, psnHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psnMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, psnTreeNode );
         pChildStatus = &pHashItem->txStatus;

         psnLockNoFailure( &parentFolder->memObject, pContext );

         psnLockNoFailure( pChildMemObject, pContext );

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
            psnTxStatusCommitRemove( pChildStatus );
            psnUnlock( pChildMemObject, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            psnUnlock( pChildMemObject, pContext );

            /* Remove it from the folder (from the hash list) */
            psnFolderRemoveObject( parentFolder,
                                    pHashItem,
                                    pContext );
         }
         psnUnlock( &parentFolder->memObject, pContext );

         break;

      case PSN_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSN_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psnFolder );
         GET_PTR( pHashItem, pOps->childOffset, psnHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
         
         psnLockNoFailure( &parentFolder->memObject, pContext );

         psnFolderRollbackEdit( parentFolder, 
                                 pHashItem, pOps->childType, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psnFolderResize( parentFolder, pContext );

         psnUnlock( &parentFolder->memObject, pContext );
         
         break;

      case PSN_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSN_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psnFolder );
         GET_PTR( pHashItem, pOps->childOffset, psnHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psnObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psnMemObject );
         pChildStatus = &pHashItem->txStatus;

         psnLockNoFailure( &parentFolder->memObject, pContext );
         psnLockNoFailure( pChildMemObject, pContext );

         psnTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psnFolderResize( parentFolder, pContext );
         
         psnUnlock( pChildMemObject, pContext );
         psnUnlock( &parentFolder->memObject, pContext );

         break;

      case PSN_TX_REMOVE_DATA:

         if ( pOps->parentType == PSN_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psnHashMap );
            pParentMemObject = &pHashMap->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnHashMapRollbackRemove( pHashMap, 
                                       pOps->childOffset, pContext );
            psnUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == PSN_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psnQueue );
            pParentMemObject = &pQueue->memObject;

            psnLockNoFailure( pParentMemObject, pContext );
            psnQueueRollbackRemove( pQueue, 
                                     pOps->childOffset );
            psnUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

#if 0
      case PSN_TX_SELECT:
         /* Not yet! */
         break;
         
      case PSN_TX_UPDATE:
         /* Not yet! */
         break;
#endif         
      }

      psnFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psnTxOps), 
                pContext );
   }
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

