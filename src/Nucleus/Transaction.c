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
#include "Nucleus/TransactionHash.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
bool psonAddLockTx( psonTx             * pTx,
                    psonMemObject      * pMemObj,
                    psonSessionContext * pContext )
{
   bool ok;
      
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pMemObj  != NULL );

   ok = txHashInsert( pTx, pMemObj, pContext );
   if ( !ok ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_NOT_ENOUGH_PSO_MEMORY );
   }

   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psonLockTx( psonTx             * pTx,
                 psonMemObject      * pMemObj,
                 psonSessionContext * pContext )
{
   bool ok;
   
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   ok = psocTestLockPidValue( &pMemObj->lock, pContext->pidLocker );
   PSO_POST_CONDITION( ok == true || ok == false );

   if ( ! ok ) {
      ok = psocTryAcquireProcessLock ( &pMemObj->lock,
                                       pContext->pidLocker,
                                       PSON_LOCK_TIMEOUT );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         ok = psonAddLockTx( pTx, pMemObj, pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static
void psonClearLocks( psonTx * pTx )
{
   psonMemObject * pMemObj = NULL;
   bool ok;
   size_t rowNumber = 0;
   
   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
   
   ok = txHashGetFirst( pTx, &rowNumber, &pMemObj );
   while ( ok ) {
      psocReleaseProcessLock( &pMemObj->lock );

      ok = txHashGetNext( pTx, rowNumber, &rowNumber, &pMemObj );
   }
   
   txHashEmpty( pTx );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxInit( psonTx             * pTx,
                 size_t               numberOfBlocks,
                 psonSessionContext * pContext )
{
   psoErrors errcode;
   bool ok;
   
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
   ok = txHashInit( pTx, 100, pContext );
   if ( !ok ) return false;
   
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

bool psonTxCommit( psonTx             * pTx,
                   psonSessionContext * pContext )
{
   psonTxOps     * pOps = NULL;
   psonLinkNode  * pLinkNode = NULL;
   psonFolder    * parentFolder,    * pChildFolder;
   psonMemObject * pChildMemObject, * parentMemObject;
   psonTreeNode  * pChildNode;
   psonTxStatus  * pChildStatus;
   psonHashMap   * pHashMap;
   psonQueue     * pQueue;
   psonHashTxItem  * pHashItem;
   psonObjectDescriptor * pDesc;
   bool ok, okLock, okDelete;
#ifdef USE_DBC
   int pOps_invalid_type = 0;
#endif

   PSO_PRE_CONDITION( pTx      != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pTx->signature == PSON_TX_SIGNATURE );
      
   /* Synch the shared memory  (commented out - too slow) */
#if 0
   int errcode = PSO_OK;

   MemoryManager::Instance()->Sync( &pContext->errorHandler );

   /* Write to the log file */
   if ( pContext->pLogFile != NULL ) {
      errcode = psonLogTransaction( pContext->pLogFile, 
                                    SET_OFFSET(pTx), 
                                    &pContext->errorHandler );
      if ( errcode != PSO_OK ) {
//         fprintf(stderr, "Transaction::Commit err1 \n" );
         return -1;
      }
   }
#endif

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to commit */
      return true;
   }
   
   /*
    * First pass - we lock all objects of interest. This is the only place 
    * were we can return false.
    */
   ok = psonLinkedListPeakFirst( &pTx->listOfOps, &pLinkNode );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {
      pChildMemObject = parentMemObject = NULL;
      
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));

      /* We always lock the parent */
      GET_PTR( parentMemObject, pOps->parentOffset, psonMemObject );
      okLock = psonLockTx( pTx, parentMemObject, pContext );
      PSO_POST_CONDITION( okLock == true || okLock == false );
      if ( ! okLock ) {
         psonClearLocks( pTx );
         return false;
      }
      /* We only lock the child for these two ops */
      if ( pOps->transType ==  PSON_TX_ADD_OBJECT || 
         pOps->transType == PSON_TX_REMOVE_OBJECT ) {

         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         
         okLock = psonLockTx( pTx, pChildMemObject, pContext );
         PSO_POST_CONDITION( okLock == true || okLock == false );
         if ( ! okLock ) {
            psonClearLocks( pTx );
            return false;
         }
      }

      ok = psonLinkedListPeakNext( &pTx->listOfOps, pLinkNode, &pLinkNode );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   
   /*
    * Second pass - we commit all operations.
    */
   ok = psonLinkedListPeakFirst( &pTx->listOfOps, &pLinkNode );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {

      parentFolder = pChildFolder = NULL;
      pChildMemObject = parentMemObject = NULL;
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
            parentMemObject = &pHashMap->memObject;

            psonHashMapCommitAdd( pHashMap, pOps->childOffset, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueCommitAdd( pQueue, pOps->childOffset );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      case PSON_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;
         
         psonTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         break;

      case PSON_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );

         pChildMemObject = NULL;
         psonFolderCommitEdit( parentFolder, pHashItem, pOps->childType, 
                               &pChildMemObject, pContext );
         /*
          * If the oldest version of the map is deleted from memory (no current
          * access), its pointer is returned in pChildMemObject so that we
          * can avoid trying to unlocking it, if we hold the lock.
          */
         if ( pChildMemObject ) {
            /*
             * Note: the old version might be locked by this transaction or
             * it might not. We clear it just in case and do not check the
             * return code (which would tell us if the lock was taken or not).
             */
            txHashDelete( pTx, pChildMemObject );
         }
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         break;

      case PSON_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, psonTreeNode );
         pChildStatus = &pHashItem->txStatus;

         if ( pChildStatus->usageCounter > 0 || 
            pChildNode->txCounter > 0  || pChildStatus->parentCounter > 0 ) {
            /* 
             * We can't remove it - someone is using it. But we flag it
             * as "Remove is committed" so that the last "user" do delete
             * it.
             */
            psonTxStatusCommitRemove( pChildStatus );
         }
         else {
            /* 
             * Remove it from the folder (from the hash list)
             * (this function also decrease the txCounter of the parentFolder 
             */
            psonFolderRemoveObject( parentFolder, 
                                    pHashItem,
                                    pContext );

            okDelete = txHashDelete( pTx, pChildMemObject );
         }

         break;

      case PSON_TX_REMOVE_DATA:
         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psonHashMap );
            parentMemObject = &pHashMap->memObject;

            psonHashMapCommitRemove( pHashMap,
                                     pOps->childOffset,
                                     pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueCommitRemove( pQueue,
                                   pOps->childOffset,
                                   pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      } /* end of switch on type of ops */

      ok = psonLinkedListPeakNext( &pTx->listOfOps, pLinkNode, &pLinkNode );
      PSO_POST_CONDITION( ok == true || ok == false );
                
   } /* end of while loop on the list of ops */

   /*
    * Third pass: we unlock everything and release the memory of the
    * transaction list.
    */
   psonClearLocks( pTx );

   while ( psonLinkedListGetFirst( &pTx->listOfOps, &pLinkNode ) ) {
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));
      psonFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psonTxOps), 
                pContext );
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTxRollback( psonTx             * pTx,
                     psonSessionContext * pContext )
{
   int errcode = PSO_OK;
   psonTxOps     * pOps = NULL;
   psonLinkNode  * pLinkNode = NULL;
   psonFolder    * parentFolder,    * pChildFolder;
   psonMemObject * pChildMemObject, * parentMemObject;
   psonTreeNode  * pChildNode;
   psonTxStatus  * pChildStatus;
   psonHashMap   * pHashMap;
   psonQueue     * pQueue;
   psonHashTxItem  * pHashItem;
   psonObjectDescriptor * pDesc;
   bool isRemoved, ok, okLock, okDelete;
#ifdef USE_DBC
   int pOps_invalid_type = 0;
#endif
   
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
//         fprintf(stderr, "Transaction::Rollback err1 \n" );
         return false;
      }
   }

   if ( pTx->listOfOps.currentSize == 0 ) {
      /* All is well - nothing to rollback */
      return true;
   }
   
   /*
    * First pass - we lock all objects of interest. This is the only place 
    * were we can return false.
    */
   ok = psonLinkedListPeakFirst( &pTx->listOfOps, &pLinkNode );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {
      pChildMemObject = parentMemObject = NULL;
      
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));

      /* We always lock the parent */
      GET_PTR( parentMemObject, pOps->parentOffset, psonMemObject );
      okLock = psonLockTx( pTx, parentMemObject, pContext );
      PSO_POST_CONDITION( okLock == true || okLock == false );
      if ( ! okLock ) {
         psonClearLocks( pTx );
         return false;
      }
      /* We only lock the child for these two ops */
      if ( pOps->transType ==  PSON_TX_ADD_OBJECT || 
         pOps->transType == PSON_TX_REMOVE_OBJECT ) {

         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         
         okLock = psonLockTx( pTx, pChildMemObject, pContext );
         PSO_POST_CONDITION( okLock == true || okLock == false );
         if ( ! okLock ) {
            psonClearLocks( pTx );
            return false;
         }
      }

      ok = psonLinkedListPeakNext( &pTx->listOfOps, pLinkNode, &pLinkNode );
      PSO_POST_CONDITION( ok == true || ok == false );
   }

   /*
    * Second pass - we rollback all operations.
    */
   ok = psonLinkedListPeakLast( &pTx->listOfOps, &pLinkNode );
   PSO_POST_CONDITION( ok == true || ok == false );
   while ( ok ) {
      parentFolder = pChildFolder = NULL;
      pChildMemObject = parentMemObject = NULL;
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
            parentMemObject = &pHashMap->memObject;

            psonHashMapRollbackAdd( pHashMap, 
                                    pOps->childOffset,
                                    pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueRollbackAdd( pQueue, 
                                  pOps->childOffset,
                                  pContext );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;
            
      case PSON_TX_ADD_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         GET_PTR( pChildNode, pDesc->nodeOffset, psonTreeNode );
         pChildStatus = &pHashItem->txStatus;

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
         }
         else {
            /* 
             * No one uses the object to remove and no one can access it
             * since we have a lock on its parent. We can safely unlock it.
             */

            /* Remove it from the folder (from the hash list) */
            psonFolderRemoveObject( parentFolder,
                                    pHashItem,
                                    pContext );
            okDelete = txHashDelete( pTx, pChildMemObject );
         }

         break;

      case PSON_TX_ADD_EDIT_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         
         psonFolderRollbackEdit( parentFolder, pHashItem, pOps->childType, 
                                 &isRemoved, pContext );
         
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );

         break;

      case PSON_TX_REMOVE_OBJECT:

         PSO_POST_CONDITION( pOps->parentType == PSON_IDENT_FOLDER );

         GET_PTR( parentFolder, pOps->parentOffset, psonFolder );
         GET_PTR( pHashItem, pOps->childOffset, psonHashTxItem );
         GET_PTR( pDesc, pHashItem->dataOffset, psonObjectDescriptor );
         GET_PTR( pChildMemObject, pDesc->memOffset, psonMemObject );
         pChildStatus = &pHashItem->txStatus;

         psonTxStatusClearTx( pChildStatus );
         parentFolder->nodeObject.txCounter--;

         /* If needed */
         psonFolderResize( parentFolder, pContext );
         
         break;

      case PSON_TX_REMOVE_DATA:

         if ( pOps->parentType == PSON_IDENT_HASH_MAP ) {
            GET_PTR( pHashMap, pOps->parentOffset, psonHashMap );
            parentMemObject = &pHashMap->memObject;

            psonHashMapRollbackRemove( pHashMap, 
                                       pOps->childOffset, pContext );
         }
         else if ( pOps->parentType == PSON_IDENT_QUEUE ) {
            GET_PTR( pQueue, pOps->parentOffset, psonQueue );
            parentMemObject = &pQueue->memObject;

            psonQueueRollbackRemove( pQueue, 
                                     pOps->childOffset );
         }
         /* We should not come here */
         else {
            PSO_POST_CONDITION( pOps_invalid_type );
         }
         
         break;

      } /* end of switch on type of ops */

      ok = psonLinkedListPeakPrevious( &pTx->listOfOps, pLinkNode, &pLinkNode );
      PSO_POST_CONDITION( ok == true || ok == false );
                
   } /* end of while loop on the list of ops */

   /*
    * Third pass: we unlock everything and release the memory of the
    * transaction list.
    */
   psonClearLocks( pTx );

   while ( psonLinkedListGetFirst( &pTx->listOfOps, &pLinkNode ) ) {
      pOps = (psonTxOps*) ((char*)pLinkNode - offsetof( psonTxOps, node ));
      psonFree( &pTx->memObject, (unsigned char*) pOps, sizeof(psonTxOps), 
                pContext );
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

