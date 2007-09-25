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

#include "Engine/Session.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/SessionContext.h"
#include "Engine/Transaction.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseSessionInit( vdseSession        * pSession,
                     void               * pApiSession,
                     vdseSessionContext * pContext )
{
   vdsErrors errcode;
   int rc = -1;
   vdseTx *pTx;
   
   VDS_PRE_CONDITION( pSession    != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );
   VDS_PRE_CONDITION( pApiSession != NULL );
   
   errcode = vdseMemObjectInit( &pSession->memObject, 
                                VDSE_IDENT_CLEAN_SESSION,
                                &pSession->blockGroup,
                                1 ); /* A single block */
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
   }
   else
   {
      pContext->pCurrentMemObject = &pSession->memObject;
   
      vdseLinkedListInit( &pSession->listOfObjects );

      pTx = (vdseTx*) vdseMallocBlocks( pContext->pAllocator, 1, pContext );
      if ( pTx != NULL )
      {
         errcode = vdseTxInit( pTx, 1, pContext );
         if ( errcode == 0 )
         {
            pSession->pTransaction = pTx;
            pContext->pTransaction = (void *) pTx;            
            pContext->lockOffsets = pSession->lockOffsets;
            pContext->numLocks = &pSession->numLocks;
            pSession->pApiSession = pApiSession;
            
            rc = 0;
         }
         else
         {
            vdseFreeBlocks( pContext->pAllocator, 
                            (unsigned char *)pTx, 
                            1, pContext );
         }
      }
      else
      {
         vdscSetError( &pContext->errorHandler, 
                       g_vdsErrorHandle, 
                       VDS_NOT_ENOUGH_VDS_MEMORY );
      }
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseSessionFini( vdseSession        * pSession,
                      vdseSessionContext * pContext )
{
   vdseObjectContext* pObject = NULL;
   vdseLinkNode * pNode = NULL;

   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pSession->memObject.objType == VDSE_IDENT_CLEAN_SESSION );
   
   pContext->pCurrentMemObject = &pSession->memObject;

   /*
    * Eliminate all objects in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( vdseLinkedListPeakFirst( &pSession->listOfObjects, 
                                    &pNode ) == LIST_OK )
   {
      pObject = (vdseObjectContext*)
         ((char*)pNode - offsetof( vdseObjectContext, node ));
      vdseSessionRemoveObj( pSession, pObject, pContext );
   }
   
   /* Terminates our associated transaction objects. We presume that
    * either commit or rollback was called to clear it.
    */
   vdseTxFini( pSession->pTransaction, pContext );

   /* This will remove the blocks of allocated memory */
   vdseMemObjectFini(  &pSession->memObject, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseSessionAddObj( vdseSession        * pSession,
                       ptrdiff_t            objOffset, 
                       enum vdsObjectType   objType, 
                       void *               pProxyObject,
                       vdseObjectContext ** ppObject,
                       vdseSessionContext * pContext )
{
   int errcode = 0, rc = -1;
   vdseObjectContext* pCurrentBuffer;

   VDS_PRE_CONDITION( pSession     != NULL );
   VDS_PRE_CONDITION( pProxyObject != NULL );
   VDS_PRE_CONDITION( ppObject     != NULL );
   VDS_PRE_CONDITION( pContext     != NULL );
   VDS_PRE_CONDITION( objOffset    != NULL_OFFSET );
   VDS_PRE_CONDITION( objType > 0 && objType < VDS_LAST_OBJECT_TYPE );
   
   /* For recovery purposes, always lock before doing anything! */
   errcode = vdseLock( &pSession->memObject, pContext );
   if ( errcode == 0 )
   {
      pCurrentBuffer = (vdseObjectContext *)
         vdseMalloc( &pSession->memObject, sizeof(vdseObjectContext), pContext );
      if ( pCurrentBuffer != NULL )
      {
         pCurrentBuffer->offset    = objOffset;
         pCurrentBuffer->type      = objType;
         pCurrentBuffer->pProxyObject = pProxyObject;

         vdseLinkedListPutLast( &pSession->listOfObjects, 
                                &pCurrentBuffer->node );
         *ppObject = pCurrentBuffer;
         rc = 0;
      }
      else
      {
         vdscSetError( &pContext->errorHandler,
                       g_vdsErrorHandle,
                       VDS_NOT_ENOUGH_VDS_MEMORY );
      }
      
      vdseUnlock( &pSession->memObject, pContext );
   }
   else
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    VDS_ENGINE_BUSY );

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseSessionRemoveObj( vdseSession        * pSession,
                          vdseObjectContext  * pObject,
                          vdseSessionContext * pContext )
{
   int errcode = VDS_OK;

   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pObject  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   /* For recovery purposes, always lock before doing anything! */
   errcode = vdseLock( &pSession->memObject, pContext );
   if ( errcode == 0 )
   {
      vdseLinkedListRemoveItem( &pSession->listOfObjects, 
                                &pObject->node );
      vdseFree( &pSession->memObject, 
                (unsigned char *)pObject, 
                sizeof(vdseObjectContext),
                pContext );

      vdseUnlock( &pSession->memObject, pContext );
   }
   else
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    VDS_ENGINE_BUSY );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Lock and Unlock must be used before calling this function */
int vdseSessionRemoveFirst( vdseSession        * pSession,
                            vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   int rc;
   vdseObjectContext * pObject;
   
   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   rc = vdseLinkedListGetFirst( &pSession->listOfObjects, 
                                &pNode );
   if ( rc == 0 )
   {
      pObject = (vdseObjectContext*)
         ((char*)pNode - offsetof( vdseObjectContext, node ));

      vdseFree( &pSession->memObject, 
                (unsigned char *)pObject, 
                sizeof(vdseObjectContext),
                pContext );
      return 0;
   }
   
   return -1;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Lock and Unlock must be used before calling this function */
int vdseSessionGetFirst( vdseSession        * pSession,
                         vdseObjectContext ** ppObject,
                         vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   int rc;

   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( ppObject != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   
   rc = vdseLinkedListPeakFirst( &pSession->listOfObjects, 
                                 &pNode );
   if ( rc == 0 )
      *ppObject = (vdseObjectContext*)
         ((char*)pNode - offsetof( vdseObjectContext, node ));
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

