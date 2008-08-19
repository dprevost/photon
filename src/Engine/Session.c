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

#include "Engine/Session.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/SessionContext.h"
#include "Engine/Transaction.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnSessionInit( psnSession        * pSession,
                      void               * pApiSession,
                      psnSessionContext * pContext )
{
   vdsErrors errcode;
   psnTx * pTx;
   bool ok, rc = false;
   
   VDS_PRE_CONDITION( pSession    != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( pApiSession != NULL );
   
   errcode = psnMemObjectInit( &pSession->memObject, 
                                PSN_IDENT_SESSION,
                                &pSession->blockGroup,
                                1 ); /* A single block */
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
   }
   else {
      psnLinkedListInit( &pSession->listOfObjects );

      pTx = (psnTx*) psnMallocBlocks( pContext->pAllocator, PSN_ALLOC_ANY, 1, pContext );
      if ( pTx != NULL ) {
         ok = psnTxInit( pTx, 1, pContext );
         VDS_PRE_CONDITION( ok == true || ok == false );
         if ( ok ) {
            pSession->numLocks = 0;
            pSession->pTransaction = pTx;
            pContext->pTransaction = (void *) pTx;            
            pContext->lockOffsets = pSession->lockOffsets;
            pContext->numLocks = &pSession->numLocks;
            pSession->pApiSession = pApiSession;
            
            rc = true;
         }
         else {
            psnFreeBlocks( pContext->pAllocator, 
                            PSN_ALLOC_ANY,
                            (unsigned char *)pTx, 
                            1, pContext );
         }
      }
      else {
         pscSetError( &pContext->errorHandler, 
                       g_vdsErrorHandle, 
                       VDS_NOT_ENOUGH_VDS_MEMORY );
      }
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnSessionFini( psnSession        * pSession,
                      psnSessionContext * pContext )
{
   psnObjectContext * pObject = NULL;
   psnLinkNode * pNode = NULL;

   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pSession->memObject.objType == PSN_IDENT_SESSION );
   
   /*
    * Eliminate all objects in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( psnLinkedListPeakFirst( &pSession->listOfObjects, &pNode ) ) {
      pObject = (psnObjectContext*)
         ((char*)pNode - offsetof( psnObjectContext, node ));
      psnSessionRemoveObj( pSession, pObject, pContext );
   }
   
   /* Terminates our associated transaction objects. We presume that
    * either commit or rollback was called to clear it.
    */
   psnTxFini( pSession->pTransaction, pContext );

   /* This will remove the blocks of allocated memory */
   psnMemObjectFini(  &pSession->memObject, PSN_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnSessionAddObj( psnSession        * pSession,
                        ptrdiff_t            objOffset, 
                        enum vdsObjectType   objType, 
                        void               * pCommonObject,
                        psnObjectContext ** ppObject,
                        psnSessionContext * pContext )
{
   bool ok = false;
   psnObjectContext * pCurrentBuffer;

   VDS_PRE_CONDITION( pSession      != NULL );
   VDS_PRE_CONDITION( pCommonObject != NULL );
   VDS_PRE_CONDITION( ppObject      != NULL );
   VDS_PRE_CONDITION( pContext      != NULL );
   VDS_PRE_CONDITION( objOffset     != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( objType > 0 && objType < VDS_LAST_OBJECT_TYPE );
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pSession->memObject, pContext ) ) {
      pCurrentBuffer = (psnObjectContext *)
         psnMalloc( &pSession->memObject, sizeof(psnObjectContext), pContext );
      if ( pCurrentBuffer != NULL ) {
         pCurrentBuffer->offset    = objOffset;
         pCurrentBuffer->type      = objType;
         pCurrentBuffer->pCommonObject = pCommonObject;

         psnLinkNodeInit( &pCurrentBuffer->node );
         psnLinkedListPutLast( &pSession->listOfObjects, 
                                &pCurrentBuffer->node );
         *ppObject = pCurrentBuffer;
         ok = true;
      }
      else {
         pscSetError( &pContext->errorHandler,
                       g_vdsErrorHandle,
                       VDS_NOT_ENOUGH_VDS_MEMORY );
      }
      
      psnUnlock( &pSession->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    VDS_ENGINE_BUSY );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnSessionRemoveObj( psnSession        * pSession,
                          psnObjectContext  * pObject,
                          psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pObject  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pSession->memObject, pContext ) ) {
      psnLinkedListRemoveItem( &pSession->listOfObjects, 
                                &pObject->node );
      psnFree( &pSession->memObject, 
                (unsigned char *)pObject, 
                sizeof(psnObjectContext),
                pContext );

      psnUnlock( &pSession->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    VDS_ENGINE_BUSY );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Lock and Unlock must be used before calling this function */
bool psnSessionRemoveFirst( psnSession        * pSession,
                             psnSessionContext * pContext )
{
   psnLinkNode * pNode = NULL;
   psnObjectContext * pObject;
   
   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   if ( psnLinkedListGetFirst(&pSession->listOfObjects, &pNode) ) {
      pObject = (psnObjectContext*)
         ((char*)pNode - offsetof( psnObjectContext, node ));

      psnFree( &pSession->memObject, 
                (unsigned char *)pObject, 
                sizeof(psnObjectContext),
                pContext );
      return true;
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Lock and Unlock must be used before calling this function */
bool psnSessionGetFirst( psnSession        * pSession,
                          psnObjectContext ** ppObject,
                          psnSessionContext * pContext )
{
   psnLinkNode * pNode = NULL;
   bool ok;

   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( ppObject != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   
   ok = psnLinkedListPeakFirst( &pSession->listOfObjects, 
                                 &pNode );
   if ( ok ) {
      *ppObject = (psnObjectContext*)
         ((char*)pNode - offsetof( psnObjectContext, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

