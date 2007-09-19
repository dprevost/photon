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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseSessionInit( vdseSession        * pSession,
                     void               * pApiSession,
                     vdseSessionContext * pContext )
{
   vdsErrors errcode;
   
   VDS_PRE_CONDITION( pSession  != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   
   errcode = vdseMemObjectInit( &pSession->memObject, 
                                VDSE_IDENT_CLEAN_SESSION,
                                &pSession->blockGroup,
                                1 ); /* A single block */
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }
   pContext->pCurrentMemObject = &pSession->memObject;

   pSession->pApiSession = pApiSession;
   
   vdseLinkedListInit( &pSession->listOfObjects );

   return 0;
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
                            vdseObjectContext ** ppObject,
                            vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   int rc;
   
   rc = vdseLinkedListGetFirst( &pSession->listOfObjects, 
                                &pNode );
   if ( rc == 0 )
      *ppObject = (vdseObjectContext*)
         ((char*)pNode - offsetof( vdseObjectContext, node ));
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

