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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseTxInit( vdseTx *            pTx,
                size_t              numberOfPages,
                vdseSessionContext* pContext )
{
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pTx != NULL );
   VDS_PRE_CONDITION( numberOfPages  > 0 );
   
   errcode = vdseMemObjectInit( &pTx->memObject, 
                                VDSE_IDENT_FOLDER,
                                numberOfPages );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }

   vdsePageGroupInit( &pTx->pageGroup,
                      SET_OFFSET(pTx), 
                      numberOfPages );
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

   pOps = (vdseTxOps*)vdseMalloc( &pTx->memObject,
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

   VDS_POST_CONDITION( listErr != LIST_OK );
   
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
   vdseTxOps* pOps = NULL;
   vdseLinkNode * pLinkNode = NULL;
   vdseFolder    *parentFolder,    *pChildFolder;
   vdseMemObject *pChildMemObject, *pParentMemObject;
   vdseTreeNode  *pChildNode;
   vdseTxStatus  *pChildStatus;
   
   VDS_PRE_CONDITION( pTx      != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );
   
   if ( pTx->listOfOps.currentSize == 0 )
      /* All is well - nothing to commit */
      return 0;
   
   /* Synch the VDS */
//   MemoryManager::Instance()->Sync( &pContext->errorHandler );
   
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
   
   while ( vdseLinkedListGetFirst( &pTx->listOfOps, 
                                   &pLinkNode ) == LIST_OK )
   {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = pParentMemObject = NULL;
      pChildNode = NULL;
      pChildStatus = NULL;
      
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
      case VDSE_TX_ADD:
         /* We only have one type of objects doing this, currently */
#if 0
         pQueue = GET_PTR( pOps->parentOffset, vdseQueue );
         errcode = pQueue->Lock( pContext->lockValue );
         if ( errcode == VDS_OK )
         {
            pQueue->RollbackRemove( pOps->childOffset, pContext );
            pQueue->Unlock();
         }
         break;
#endif         
      case VDSE_TX_CREATE:
         
         vdseLockNoFailure( pChildMemObject, pOps->childOffset, pContext );
         vdseTxStatusCommit( pChildStatus );
         vdseUnlock( pChildMemObject, pContext );
         
         break;

      case VDSE_TX_DESTROY:
      
         vdseLockNoFailure( &parentFolder->memObject, pOps->childOffset, pContext );

         vdseLockNoFailure( pChildMemObject, pOps->childOffset, pContext );

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
            vdseUnlock( pChildMemObject, pContext );

            /* Remove it from the folder and free the memory */
            vdseFolderRemoveObject( parentFolder, 
                                    pChildMemObject,
                                    GET_PTR(pChildNode->myKeyOffset, vdsChar_T),
                                    pChildNode->myNameLength,
                                    pContext );
            /*
             * Since the object is now remove from the hash, all we need
             * to do is reclaim the memory.
             */

         }
         vdseUnlock( &parentFolder->memObject, pContext );

         break;
#if 0
      case VDSE_TX_REMOVE:
         /* We only have one type of objects doing this, currently */
         pQueue =  GET_PTR( pOps->parentOffset, Queue, pContext->pAllocator );
         errcode = pQueue->Lock( pContext->lockValue );
         if ( errcode == VDS_OK )
         {
            pQueue->RollbackInsert( pOps->childOffset, pContext );
            pQueue->Unlock();
         }
         break;

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

#if 0

void vdseTxRollback( vdseTx*             pTx,
                     vdseSessionContext* pContext )
{
   int errcode = VDS_OK;
   vdseTxOps* pOps = NULL;
   vdseLinkNode * pLinkNode = NULL;

   VDS_PRE_CONDITION( pTx != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pTx->signature == VDSE_TX_SIGNATURE );

   if ( pTx->listOfOps.currentSize == 0 )
      /* All is well - nothing to rollback */
      return;

   /* Synch the VDS ??? */   
   /* Write to the log file ??? */

   
   while ( vdseLinkedListGetLast( &pTx->listOfOps, 
                                  &pLinkNode, 
                                  pContext->pAllocator ) == LIST_OK )
   {
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
      case VDSE_TX_ADD:
#if 0
         /* We only have one type of objects doing this, currently */
         pQueue =  GET_PTR( pOps->parentOffset, Queue, pContext->pAllocator );
         errcode = pQueue->Lock( pContext->lockValue );
         if ( errcode == VDS_OK )
         {
            pQueue->RollbackInsert( pOps->childOffset, pContext );
            pQueue->Unlock();
         }
#endif
         break;
            
      case VDSE_TX_CREATE:

         pFolder =  
            GET_PTR( pOps->parentOffset, Folder, pContext->pAllocator );

         errcode = pFolder->Lock( pContext->lockValue );
         if ( errcode == VDS_OK )
         {
            pFolder->RollbackCreate( pOps->childOffset, 
                                     pOps->childType,
                                     pContext );
            pFolder->Unlock();
         }
         break;

      case VDSE_TX_DESTROY:
         /* Is locking the folder necessary? Not sure... */
         pFolder = GET_PTR( pOps->parentOffset, Folder, pContext->pAllocator );
         errcode = pFolder->Lock( pContext->lockValue );
         if ( errcode == VDS_OK )
         {
            pFolder->RollbackDestroy( pOps->childOffset, 
                                      pContext );
            pFolder->Unlock();
         }
         break;

      case VDSE_TX_REMOVE:
#if 0
         /* We only have one type of objects doing this, currently */
         pQueue =  GET_PTR( pOps->parentOffset, Queue, pContext->pAllocator );
         errcode = pQueue->Lock( pContext->lockValue );
         if ( errcode == VDS_OK )
         {
            pQueue->RollbackRemove( pOps->childOffset, pContext );
            pQueue->Unlock();
         }
#endif
         break;

      case VDSE_TX_SELECT:
         /* Not yet! */
         break;
         
      case VDSE_TX_UPDATE:
         /* Not yet! */
         break;
         
      }

      vdseFree( &pTx->memObject, (unsigned char*) pOps, sizeof(vdseTxOps), 
                pContext );
   }
   
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */