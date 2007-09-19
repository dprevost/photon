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

#include "Engine/Process.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcessInit( vdseProcess *        pProcess,
                     pid_t                pid,
                     vdseSessionContext * pContext )
{
   vdsErrors errcode;

   VDS_PRE_CONDITION( pProcess != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   errcode = vdseMemObjectInit( &pProcess->memObject, 
                                VDSE_IDENT_CLEAN_PROCESS,
                                &pProcess->blockGroup,
                                1 ); /* A single block */
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }
   pContext->pCurrentMemObject = &pProcess->memObject;

   pProcess->pid = pid;
   pProcess->processIsTerminating = false;

   vdseLinkedListInit( &pProcess->listOfSessions );

   return 0;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseProcessFini( vdseProcess        * pProcess,
                      vdseSessionContext * pContext )
{
   vdseSession  * pSession = NULL;
   vdseLinkNode * pNode    = NULL;
   
   VDS_PRE_CONDITION( pProcess != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pProcess->memObject.objType == VDSE_IDENT_CLEAN_PROCESS );

   /*
    * Eliminate all sessions in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( vdseLinkedListPeakFirst( &pProcess->listOfSessions, 
                                    &pNode ) == LIST_OK )
   {
      pSession = (vdseSession*)
         ((char*)pNode - offsetof( vdseSession, node ));

      vdseProcessRemoveSession( pProcess, pSession, pContext );
   }

   /* 
    * This will remove the blocks of allocated memory.
    * It must be the last operation on that piece of memory.
    */
   vdseMemObjectFini(  &pProcess->memObject, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseProcessAddSession( vdseProcess        * pProcess,
                                 void *               pApiSession,
                                 vdseSession       ** ppSession,
                                 vdseSessionContext * pContext )
{
   int errcode, rc = -1;
   vdseSession* pCurrentBuffer;

   *ppSession = NULL;
   /* For recovery purposes, always lock before doing anything! */
   errcode = vdseLock( &pProcess->memObject, pContext );
   if ( errcode == 0 )
   {
      pCurrentBuffer = (vdseSession*) 
          vdseMallocBlocks( pContext->pAllocator, 1, pContext );
      if ( pCurrentBuffer != NULL )
      {
         errcode = vdseSessionInit( pCurrentBuffer, pApiSession, pContext );
         if ( errcode == 0 )
         {
            vdseLinkedListPutLast( &pProcess->listOfSessions, 
                                   &pCurrentBuffer->node );
            *ppSession = pCurrentBuffer;
            rc = 0;
         }
         else
         {
            vdseFreeBlocks( pContext->pAllocator, 
                            (unsigned char *)pCurrentBuffer, 
                            1, pContext );
         }
      }
      else
      {
         vdscSetError( &pContext->errorHandler, 
                       g_vdsErrorHandle, 
                       VDS_NOT_ENOUGH_VDS_MEMORY );
      }
      vdseUnlock( &pProcess->memObject, pContext );
   }
   else
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    VDS_ENGINE_BUSY );

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseProcessRemoveSession( vdseProcess        * pProcess,
                                    vdseSession        * pSession,
                                    vdseSessionContext * pContext )
{
   int errcode = 0;

   /* For recovery purposes, always lock before doing anything! */
   errcode = vdseLock( &pProcess->memObject, pContext );
   if ( errcode == 0 )
   {
      vdseLinkedListRemoveItem( &pProcess->listOfSessions, 
                                &pSession->node );
      
      vdseSessionFini( pSession, pContext );

      vdseUnlock( &pProcess->memObject, pContext );
   }
   else
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    VDS_ENGINE_BUSY );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcessGetFirstSession( vdseProcess        * pProcess,
                                vdseSession       ** ppSession,
                                vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   enum ListErrors err;
   
   err = vdseLinkedListPeakFirst( &pProcess->listOfSessions, &pNode );
   if ( err != LIST_OK )
      return -1;

   *ppSession = (vdseSession *)
      ((char*)pNode - offsetof( vdseSession, node ));

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcessGetNextSession( vdseProcess        * pProcess,
                               vdseSession        * pCurrent,
                               vdseSession       ** ppNext,
                               vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   enum ListErrors err;
   
   err = vdseLinkedListPeakNext( &pProcess->listOfSessions,
                                 &pCurrent->node,
                                 &pNode );
   if ( err != LIST_OK )
      return -1;

   *ppNext = (vdseSession*)
      ((char*)pNode - offsetof( vdseSession, node ));

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

