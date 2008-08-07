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

#include "Transaction.h"
#include "LogFile.h"
#include "Folder.h"
#include "HashMap.h"
#include "Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseTxInit( vdseTx             * pTx,
                 size_t               numberOfBlocks,
                 vdseSessionContext * pContext )
{
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   
   errcode = vdseMemObjectInit( &pTx->memObject, 
                                VDSE_IDENT_TRANSACTION,
                                &pTx->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return false;
   }

   vdseLinkedListInit( &pTx->listOfOps );

   pTx->signature = VDSE_TX_SIGNATURE;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxFini( vdseTx             * pTx, 
                 vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->listOfOps.currentSize == 0 );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );
   
   /* Synch the VDS */
#if 0
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Close and unlink the log file */
   if ( pContext->pLogFile != NULL ) {
      vdseCloseLogFile( pContext->pLogFile, &pContext->errorHandler );
   }
   
   vdseLinkedListFini( &pTx->listOfOps );
   pTx->signature = 0;
   
   /* This will remove the blocks of allocated memory */
   vdseMemObjectFini(  &pTx->memObject, VDSE_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseTxAddOps( vdseTx             * pTx,
                        vdseTxType           txType,
                        ptrdiff_t            parentOffset, 
                        vdseMemObjIdent      parentType, 
                        ptrdiff_t            childOffset,
                        vdseMemObjIdent      childType, 
                        vdseSessionContext * pContext )
{
   vdseTxOps * pOps;
   
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( parentOffset != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( childOffset  != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );

   pOps = (vdseTxOps *) vdseMalloc( &pTx->memObject,
                                    sizeof(vdseTxOps), 
                                    pContext );
   if ( pOps != NULL ) {
      pOps->transType    = txType;
      pOps->parentOffset = parentOffset;
      pOps->parentType   = parentType;
      pOps->childOffset  = childOffset;
      pOps->childType    = childType;

      vdseLinkNodeInit(  &pOps->node );
      vdseLinkedListPutLast( &pTx->listOfOps, &pOps->node );

      return VDS_OK;
   }

   vdscSetError( &pContext->errorHandler, 
                 g_vdsErrorHandle, 
                 VDS_NOT_ENOUGH_VDS_MEMORY );

   return VDS_NOT_ENOUGH_VDS_MEMORY;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxRemoveLastOps( vdseTx             * pTx, 
                          vdseSessionContext * pContext )
{
   bool ok;
   vdseLinkNode * pDummy = NULL;
   vdseTxOps * pOps;

   VDS_PRE_CONDITION( pTx != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );
   
   ok = vdseLinkedListGetLast( &pTx->listOfOps, &pDummy );

   VDS_POST_CONDITION( ok );
   
   pOps = (vdseTxOps *)((char *)pDummy - offsetof( vdseTxOps, node ));
   
   vdseFree( &pTx->memObject,
             (unsigned char *) pOps,
             sizeof(vdseTxOps), 
             pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxCommit( vdseTx             * pTx,
                   vdseSessionContext * pContext )
{
   int errcode = VDS_OK;
   vdseTxOps     * pOps = NULL;
   vdseLinkNode  * pLinkNode = NULL;
   vdseFolder    * parentFolder,    * pChildFolder;
   vdseMemObject * pChildMemObject, * pParentMemObject;
   vdseTreeNode  * pChildNode;
   vdseTxStatus  * pChildStatus;
   vdseHashMap   * pHashMap;
   vdseQueue     * pQueue;
   int pOps_invalid_type = 0;
   vdseHashItem  * pHashItem;
   vdseObjectDescriptor * pDesc;
   
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );
      
   /* Synch the VDS */
#if 0
   MemoryManager::Instance()->Sync( &pContext->errorHandler );

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = vdseLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != VDS_OK ) {
         fprintf(stderr, "Transaction::Commit err1 \n" );
         return -1;
      }
   }
#endif

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to commit */
      return;
   }
   
   while ( vdseLinkedListGetFirst( &pTx->listOfOps, &pLinkNode ) ) {

      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode = NULL;
      pChildStatus = NULL;
      pHashMap = NULL;
      pQueue   = NULL;
      
      pOps = (vdseTxOps*)
         ((char*)pLinkNode - offsetof( vdseTxOps, node ));
      
      switch( pOps->transType ) {

      case VDSE_TX_ADD_DATA:

         if ( pOps->parentType == VDSE_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapCommitAdd( pHashMap, pOps->childOffset, pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDSE_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueCommitAdd( pQueue, pOps->childOffset );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            VDS_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      case VDSE_TX_ADD_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDSE_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, vdseFolder );
         GET_PTR( pHashItem, pOps->childOffset, vdseHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, vdseMemObject );
         pChildStatus = &pHashItem->txStatus;
         
         vdseLockNoFailure( &parentFolder->memObject, pContext );
         vdseLockNoFailure( pChildMemObject, pContext );

         vdseTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         vdseFolderResize( parentFolder, pContext );

         vdseUnlock( pChildMemObject, pContext );
         vdseUnlock( &parentFolder->memObject, pContext );
         
         break;

      case VDSE_TX_ADD_EDIT_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDSE_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, vdseFolder );
         GET_PTR( pHashItem, pOps->childOffset, vdseHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
         
         vdseLockNoFailure( &parentFolder->memObject, pContext );

         vdseFolderCommitEdit( parentFolder, 
                               pHashItem, pOps->childType, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         vdseFolderResize( parentFolder, pContext );

         vdseUnlock( &parentFolder->memObject, pContext );
         
         break;

      case VDSE_TX_REMOVE_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDSE_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, vdseFolder );
         GET_PTR( pHashItem, pOps->childOffset, vdseHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, vdseMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, vdseTreeNode );
         pChildStatus = &pHashItem->txStatus;

         vdseLockNoFailure( &parentFolder->memObject, pContext );

         vdseLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /* 
             * We can't remove it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            vdseTxStatusCommitRemove( pChildStatus );
            vdseUnlock( pChildMemObject, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            vdseUnlock( pChildMemObject, pContext );

            /* 
             * Remove it from the folder (from the hash list)
             * (this function also decrease the txCounter of the parentFolder 
             */
            vdseFolderRemoveObject( parentFolder, 
                                    pHashItem,
                                    pContext );
         }
         vdseUnlock( &parentFolder->memObject, pContext );

         break;

      case VDSE_TX_REMOVE_DATA:
         if ( pOps->parentType == VDSE_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapCommitRemove( pHashMap,
                                     pOps->childOffset,
                                     pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDSE_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueCommitRemove( pQueue,
                                   pOps->childOffset,
                                   pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            VDS_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
#if 0
      case VDSE_TX_SELECT:
         /* Not yet! */
         break;
         
      case VDSE_TX_UPDATE:
         /* Not yet! */
         break;
#endif
      } /* end of switch on type of ops */

      vdseFree( &pTx->memObject, (unsigned char*) pOps, sizeof(vdseTxOps), 
                pContext );
                
   } /* end of while loop on the list of ops */
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxRollback( vdseTx             * pTx,
                     vdseSessionContext * pContext )
{
   int errcode = VDS_OK;
   vdseTxOps     * pOps = NULL;
   vdseLinkNode  * pLinkNode = NULL;
   vdseFolder    * parentFolder,    * pChildFolder;
   vdseMemObject * pChildMemObject, * pParentMemObject;
   vdseTreeNode  * pChildNode;
   vdseTxStatus  * pChildStatus;
   vdseHashMap   * pHashMap;
   vdseQueue     * pQueue;
   vdseHashItem  * pHashItem;
   vdseObjectDescriptor * pDesc;
   int pOps_invalid_type = 0;

   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );

#if 0
   /* Synch the VDS */
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = vdseLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != VDS_OK ) {
         fprintf(stderr, "Transaction::Rollback err1 \n" );
         return;
      }
   }

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to rollback */
      return;
   }
   
   while ( vdseLinkedListGetLast( &pTx->listOfOps, 
                                  &pLinkNode ) ) {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode   = NULL;
      pChildStatus = NULL;
      pHashMap     = NULL;
      pQueue       = NULL;
      
      pOps = (vdseTxOps*)
         ((char*)pLinkNode - offsetof( vdseTxOps, node ));

      switch( pOps->transType ) {

      case VDSE_TX_ADD_DATA:
         if ( pOps->parentType == VDSE_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapRollbackAdd( pHashMap, 
                                    pOps->childOffset,
                                    pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDSE_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueRollbackAdd( pQueue, 
                                  pOps->childOffset,
                                  pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            VDS_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
            
      case VDSE_TX_ADD_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDSE_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, vdseFolder );
         GET_PTR( pHashItem, pOps->childOffset, vdseHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, vdseMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, vdseTreeNode );
         pChildStatus = &pHashItem->txStatus;

         vdseLockNoFailure( &parentFolder->memObject, pContext );

         vdseLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /*
             * Can this really happen? Yes! No other session is supposed to
             * be able to open the object until CREATE is committed but
             * the current session might have open it to insert data and... 
             * (the rollback might be the result of an abnormal error, for
             * example or a call to vdsExit() or...).
             */
            /** \todo Revisit this. Maybe rolling back a create object should
             * also automatically close the object if open. */
            /* 
             * We can't "uncreate" it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            vdseTxStatusCommitRemove( pChildStatus );
            vdseUnlock( pChildMemObject, pContext );
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            vdseUnlock( pChildMemObject, pContext );

            /* Remove it from the folder (from the hash list) */
            vdseFolderRemoveObject( parentFolder,
                                    pHashItem,
                                    pContext );
         }
         vdseUnlock( &parentFolder->memObject, pContext );

         break;

      case VDSE_TX_ADD_EDIT_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDSE_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, vdseFolder );
         GET_PTR( pHashItem, pOps->childOffset, vdseHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
         
         vdseLockNoFailure( &parentFolder->memObject, pContext );

         vdseFolderRollbackEdit( parentFolder, 
                                 pHashItem, pOps->childType, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         vdseFolderResize( parentFolder, pContext );

         vdseUnlock( &parentFolder->memObject, pContext );
         
         break;

      case VDSE_TX_REMOVE_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDSE_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, vdseFolder );
         GET_PTR( pHashItem, pOps->childOffset, vdseHashItem );
         GET_PTR( pDesc, pHashItem->dataOffset, vdseObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, vdseMemObject );
         pChildStatus = &pHashItem->txStatus;

         vdseLockNoFailure( &parentFolder->memObject, pContext );
         vdseLockNoFailure( pChildMemObject, pContext );

         vdseTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         vdseFolderResize( parentFolder, pContext );
         
         vdseUnlock( pChildMemObject, pContext );
         vdseUnlock( &parentFolder->memObject, pContext );

         break;

      case VDSE_TX_REMOVE_DATA:

         if ( pOps->parentType == VDSE_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapRollbackRemove( pHashMap, 
                                       pOps->childOffset, pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDSE_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueRollbackRemove( pQueue, 
                                     pOps->childOffset );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else {
            VDS_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

#if 0
      case VDSE_TX_SELECT:
         /* Not yet! */
         break;
         
      case VDSE_TX_UPDATE:
         /* Not yet! */
         break;
#endif         
      }

      vdseFree( &pTx->memObject, (unsigned char*) pOps, sizeof(vdseTxOps), 
                pContext );
   }
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

