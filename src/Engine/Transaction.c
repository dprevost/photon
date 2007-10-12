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

#include "Transaction.h"
#include "LogFile.h"
#include "Folder.h"
#include "HashMap.h"
#include "Queue.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTxInit( vdseTx *            pTx,
                size_t              numberOfBlocks,
                vdseSessionContext* pContext )
{
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pTx != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( numberOfBlocks  > 0 );
   
   errcode = vdseMemObjectInit( &pTx->memObject, 
                                VDSE_IDENT_TRANSACTION,
                                &pTx->blockGroup,
                                numberOfBlocks );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }

   vdseLinkedListInit( &pTx->listOfOps );

   pTx->signature = VDSE_TX_SIGNATURE;
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxFini( vdseTx*             pTx, 
                 vdseSessionContext* pContext )
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
   if ( pContext->pLogFile != NULL )
      vdseCloseLogFile( pContext->pLogFile, &pContext->errorHandler );
   
   vdseLinkedListFini( &pTx->listOfOps );
   pTx->signature = 0;
   
   /* This will remove the blocks of allocated memory */
   vdseMemObjectFini(  &pTx->memObject, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTxAddOps( vdseTx*             pTx,
                  vdseTxType          txType,
                  ptrdiff_t           parentOffset, 
                  enum vdsObjectType  parentType, 
                  ptrdiff_t           childOffset,
                  enum vdsObjectType  childType, 
                  vdseSessionContext* pContext )
{
   vdseTxOps* pOps;
   
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( parentOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( childOffset  != NULL_OFFSET );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );

   pOps = (vdseTxOps *) vdseMalloc( &pTx->memObject,
                                    sizeof(vdseTxOps), 
                                    pContext );
   if ( pOps != NULL )
   {
      pOps->transType    = txType;
      pOps->parentOffset = parentOffset;
      pOps->parentType   = parentType;
      pOps->childOffset  = childOffset;
      pOps->childType    = childType;

      vdseLinkNodeInit(  &pOps->node );
      vdseLinkedListPutLast( &pTx->listOfOps, &pOps->node );

      return 0;
   }

   vdscSetError( &pContext->errorHandler, 
                 g_vdsErrorHandle, 
                 VDS_NOT_ENOUGH_VDS_MEMORY );

   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxRemoveLastOps( vdseTx* pTx, vdseSessionContext* pContext )
{
   enum ListErrors listErr;
   vdseLinkNode* pDummy = NULL;
   vdseTxOps* pOps;

   VDS_PRE_CONDITION( pTx != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );
   
   listErr = vdseLinkedListGetLast( &pTx->listOfOps, &pDummy );

   VDS_POST_CONDITION( listErr == LIST_OK );
   
   pOps = (vdseTxOps*)((char*)pDummy - offsetof( vdseTxOps, node ));
   
   vdseFree( &pTx->memObject,
             (unsigned char*) pOps,
             sizeof(vdseTxOps), 
             pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTxCommit( vdseTx*             pTx,
                  vdseSessionContext* pContext )
{
   int errcode = VDS_OK;
   vdseTxOps     * pOps = NULL;
   vdseLinkNode  * pLinkNode = NULL;
   vdseFolder    * parentFolder,    *pChildFolder;
   vdseMemObject * pChildMemObject, *pParentMemObject;
   vdseTreeNode  * pChildNode;
   vdseTxStatus  * pChildStatus;
   vdseHashMap   * pHashMap;
   vdseQueue     * pQueue;
   
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );
      
   /* Synch the VDS */
#if 0
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Write to the log file */
   if ( pContext->pLogFile != NULL )
   {
      errcode = vdseLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != VDS_OK )
      {
         fprintf(stderr, "Transaction::Commit err1 \n" );
         return -1;
      }
   }

   if ( pTx->listOfOps.currentSize == 0 )
      /* All is well - nothing to commit */
      return 0;
   
   while ( vdseLinkedListGetFirst( &pTx->listOfOps, 
                                   &pLinkNode ) == LIST_OK )
   {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode = NULL;
      pChildStatus = NULL;
      pHashMap = NULL;
      pQueue   = NULL;

      pOps = (vdseTxOps*)
         ((char*)pLinkNode - offsetof( vdseTxOps, node ));
      
      switch( pOps->transType )
      {         
      case VDSE_TX_ADD_DATA:

         if ( pOps->parentType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapCommitAdd( pHashMap, pOps->childOffset );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else if ( pOps->parentType == VDS_QUEUE )
         {
            pQueue = GET_PTR( pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueCommitAdd( pQueue, pOps->childOffset );
            vdseUnlock( pParentMemObject, pContext );
         }
         else
            VDS_POST_CONDITION( 0 == 1 );

         break;

      case VDSE_TX_ADD_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDS_FOLDER );

         parentFolder = GET_PTR( pOps->parentOffset, vdseFolder );

         if ( pOps->childType == VDS_FOLDER )
         {
            pChildFolder = GET_PTR( pOps->childOffset, vdseFolder );
            pChildMemObject = &pChildFolder->memObject;
            pChildNode      = &pChildFolder->nodeObject;
         }
         else if ( pOps->childType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->childOffset, vdseHashMap );
            pChildMemObject = &pHashMap->memObject;
            pChildNode      = &pHashMap->nodeObject;
         }
         else
         {
            pQueue          = GET_PTR( pOps->childOffset, vdseQueue );
            pChildMemObject = &pQueue->memObject;
            pChildNode      = &pQueue->nodeObject;
         }
         pChildStatus = GET_PTR( pChildNode->txStatusOffset, vdseTxStatus );
         
         vdseLockNoFailure( pChildMemObject, pContext );
         vdseTxStatusClearTx( pChildStatus );
         vdseUnlock( pChildMemObject, pContext );
         
         break;

      case VDSE_TX_REMOVE_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDS_FOLDER );

         parentFolder = GET_PTR( pOps->parentOffset, vdseFolder );

         if ( pOps->childType == VDS_FOLDER )
         {
            pChildFolder = GET_PTR( pOps->childOffset, vdseFolder );
            pChildMemObject = &pChildFolder->memObject;
            pChildNode      = &pChildFolder->nodeObject;
         }
         else if ( pOps->childType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->childOffset, vdseHashMap );
            pChildMemObject = &pHashMap->memObject;
            pChildNode      = &pHashMap->nodeObject;
         }
         else
         {
            pQueue          = GET_PTR( pOps->childOffset, vdseQueue );
            pChildMemObject = &pQueue->memObject;
            pChildNode      = &pQueue->nodeObject;
         }
         pChildStatus = GET_PTR( pChildNode->txStatusOffset, vdseTxStatus );
      
         vdseLockNoFailure( &parentFolder->memObject, pContext );

         vdseLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || pChildNode->txCounter > 0 )
         {
            /* 
             * We can't remove it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            vdseTxStatusCommitRemove( pChildStatus );
            vdseUnlock( pChildMemObject, pContext );
         }
         else
         {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            vdseUnlock( pChildMemObject, pContext );

            /* Remove it from the folder (from the hash list) */
            vdseFolderRemoveObject( parentFolder, 
                                    GET_PTR(pChildNode->myKeyOffset, vdsChar_T),
                                    pChildNode->myNameLength,
                                    pContext );
            /*
             * Since the object is now remove from the hash, all we need
             * to do is reclaim the memory (which is done in the destructor
             * of the memory object).
             */
            if ( pOps->childType == VDS_FOLDER )
            {
               vdseFolderFini( pChildFolder, pContext );
            }
            else if ( pOps->childType == VDS_HASH_MAP )
            {
               vdseHashMapFini( pHashMap, pContext );
            }
            else if ( pOps->childType == VDS_QUEUE )
            {
               vdseQueueFini( pQueue, pContext );
            }
            else
               VDS_POST_CONDITION( 0 == 1 );               
         }
         vdseUnlock( &parentFolder->memObject, pContext );

         break;

      case VDSE_TX_REMOVE_DATA:
         if ( pOps->parentType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapCommitRemove( pHashMap,
                                     pOps->childOffset,
                                     pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDS_QUEUE )
         {
            pQueue = GET_PTR( pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueCommitRemove( pQueue,
                                   pOps->childOffset,
                                   pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else
            VDS_POST_CONDITION( 0 == 1 );

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
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseTxRollback( vdseTx*             pTx,
                     vdseSessionContext* pContext )
{
   int errcode = VDS_OK;
   vdseTxOps     * pOps = NULL;
   vdseLinkNode  * pLinkNode = NULL;
   vdseFolder    * parentFolder,    *pChildFolder;
   vdseMemObject * pChildMemObject, *pParentMemObject;
   vdseTreeNode  * pChildNode;
   vdseTxStatus  * pChildStatus;
   vdseHashMap   * pHashMap;
   vdseQueue     * pQueue;

   VDS_PRE_CONDITION( pTx != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );

#if 0
   /* Synch the VDS */
   MemoryManager::Instance()->Sync( &pContext->errorHandler );
#endif

   /* Write to the log file */
   if ( pContext->pLogFile != NULL )
   {
      errcode = vdseLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != VDS_OK )
      {
         fprintf(stderr, "Transaction::Rollback err1 \n" );
         return;
      }
   }

   if ( pTx->listOfOps.currentSize == 0 )
      /* All is well - nothing to rollback */
      return;
   
   while ( vdseLinkedListGetLast( &pTx->listOfOps, 
                                  &pLinkNode ) == LIST_OK )
   {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode   = NULL;
      pChildStatus = NULL;
      pHashMap     = NULL;
      pQueue       = NULL;
      
      pOps = (vdseTxOps*)
         ((char*)pLinkNode - offsetof( vdseTxOps, node ));

      if ( pOps->parentType == VDS_FOLDER )
      {
         parentFolder = GET_PTR( pOps->parentOffset, vdseFolder );

         if ( pOps->childType == VDS_FOLDER )
         {
            pChildFolder = GET_PTR( pOps->childOffset, vdseFolder );
            pChildMemObject = &pChildFolder->memObject;
            pChildNode      = &pChildFolder->nodeObject;
            pChildStatus = GET_PTR( pChildNode->txStatusOffset, vdseTxStatus );
         }
      }
      
      switch( pOps->transType )
      {            
      case VDSE_TX_ADD_DATA:
         if ( pOps->parentType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapRollbackAdd( pHashMap, 
                                    pOps->childOffset,
                                    pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDS_QUEUE )
         {
            pQueue = GET_PTR( pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueRollbackAdd( pQueue, 
                                  pOps->childOffset,
                                  pContext );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else
            VDS_POST_CONDITION( 0 == 1 );

         break;
            
      case VDSE_TX_ADD_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDS_FOLDER );

         parentFolder = GET_PTR( pOps->parentOffset, vdseFolder );

         if ( pOps->childType == VDS_FOLDER )
         {
            pChildFolder = GET_PTR( pOps->childOffset, vdseFolder );
            pChildMemObject = &pChildFolder->memObject;
            pChildNode      = &pChildFolder->nodeObject;
         }
         else if ( pOps->childType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->childOffset, vdseHashMap );
            pChildMemObject = &pHashMap->memObject;
            pChildNode      = &pHashMap->nodeObject;
         }
         else
         {
            pQueue          = GET_PTR( pOps->childOffset, vdseQueue );
            pChildMemObject = &pQueue->memObject;
            pChildNode      = &pQueue->nodeObject;
         }
         pChildStatus = GET_PTR( pChildNode->txStatusOffset, vdseTxStatus );

         vdseLockNoFailure( &parentFolder->memObject, pContext );

         vdseLockNoFailure( pChildMemObject, pContext );

         if ( pChildStatus->usageCounter > 0 || pChildNode->txCounter > 0 )
         {
            /*
             * Can this really happen? No other session is supposed to
             * be able to open the object until CREATE is committed but
             * the current session might still have it open I guess.
             */
            /** \todo Revisit this */
            /* 
             * We can't "uncreate" it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            vdseTxStatusCommitRemove( pChildStatus );
            vdseUnlock( pChildMemObject, pContext );
         }
         else
         {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */
            vdseUnlock( pChildMemObject, pContext );

            /* Remove it from the folder (from the hash list) */
            vdseFolderRemoveObject( parentFolder, 
                                    GET_PTR(pChildNode->myKeyOffset, vdsChar_T),
                                    pChildNode->myNameLength,
                                    pContext );
            /*
             * Since the object is now remove from the hash, all we need
             * to do is reclaim the memory (which is done in the destructor
             * of the memory object).
             */
            if ( pOps->childType == VDS_FOLDER )
            {
               vdseFolderFini( pChildFolder, pContext );
            }
         }
         vdseUnlock( &parentFolder->memObject, pContext );

         break;

      case VDSE_TX_REMOVE_OBJECT:

         VDS_POST_CONDITION( pOps->parentType == VDS_FOLDER );

         parentFolder = GET_PTR( pOps->parentOffset, vdseFolder );

         if ( pOps->childType == VDS_FOLDER )
         {
            pChildFolder = GET_PTR( pOps->childOffset, vdseFolder );
            pChildMemObject = &pChildFolder->memObject;
            pChildNode      = &pChildFolder->nodeObject;
         }
         else if ( pOps->childType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->childOffset, vdseHashMap );
            pChildMemObject = &pHashMap->memObject;
            pChildNode      = &pHashMap->nodeObject;
         }
         else
         {
            pQueue          = GET_PTR( pOps->childOffset, vdseQueue );
            pChildMemObject = &pQueue->memObject;
            pChildNode      = &pQueue->nodeObject;
         }
         pChildStatus = GET_PTR( pChildNode->txStatusOffset, vdseTxStatus );

         /*
          * Tricky: the status of a child is only use in the Folder object,
          * not in the object itself. So no need to lock the child...
          */
         vdseLockNoFailure( pParentMemObject, pContext );
         vdseTxStatusClearTx( pChildStatus );
         vdseUnlock( pParentMemObject, pContext );

         break;

      case VDSE_TX_REMOVE_DATA:

         if ( pOps->parentType == VDS_HASH_MAP )
         {
            pHashMap = GET_PTR( pOps->parentOffset, vdseHashMap );
            pParentMemObject = &pHashMap->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseHashMapRollbackRemove( pHashMap, 
                                       pOps->childOffset );
            vdseUnlock( pParentMemObject, pContext );
         }
         else if ( pOps->parentType == VDS_QUEUE )
         {
            pQueue = GET_PTR( pOps->parentOffset, vdseQueue );
            pParentMemObject = &pQueue->memObject;

            vdseLockNoFailure( pParentMemObject, pContext );
            vdseQueueRollbackRemove( pQueue, 
                                     pOps->childOffset );
            vdseUnlock( pParentMemObject, pContext );
         }
         /* We should not come here */
         else
            VDS_POST_CONDITION( 0 == 1 );

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
