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

#include "Engine/Process.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseProcessInit( vdseProcess        * pProcess,
                      pid_t                pid,
                      vdseSessionContext * pContext )
{
   vdsErrors errcode;

   VDS_PRE_CONDITION( pProcess != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pid > 0 );

   errcode = vdseMemObjectInit( &pProcess->memObject, 
                                VDSE_IDENT_PROCESS,
                                &pProcess->blockGroup,
                                1 ); /* A single block */
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return false;
   }

   pProcess->pid = pid;
   pProcess->processIsTerminating = false;

   vdseLinkedListInit( &pProcess->listOfSessions );

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseProcessFini( vdseProcess        * pProcess,
                      vdseSessionContext * pContext )
{
   vdseSession  * pSession = NULL;
   vdseLinkNode * pNode    = NULL;
   bool ok;
   
   VDS_PRE_CONDITION( pProcess != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pProcess->memObject.objType == VDSE_IDENT_PROCESS );

   /*
    * Eliminate all sessions in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( vdseLinkedListPeakFirst( &pProcess->listOfSessions, 
                                    &pNode ) ) {
      pSession = (vdseSession*)
         ((char*)pNode - offsetof( vdseSession, node ));

      ok = vdseProcessRemoveSession( pProcess, pSession, pContext );
      VDS_POST_CONDITION( ok == true || ok == false );
   }

   /* 
    * This will remove the blocks of allocated memory.
    * It must be the last operation on that piece of memory.
    */
   vdseMemObjectFini(  &pProcess->memObject, VDSE_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseProcessAddSession( vdseProcess        * pProcess,
                            void               * pApiSession,
                            vdseSession       ** ppSession,
                            vdseSessionContext * pContext )
{
   vdseSession * pCurrentBuffer;
   bool ok = false;
   
   VDS_PRE_CONDITION( pProcess    != NULL );
   VDS_PRE_CONDITION( pApiSession != NULL );
   VDS_PRE_CONDITION( ppSession   != NULL );
   VDS_PRE_CONDITION( pContext    != NULL );

   *ppSession = NULL;
   /* For recovery purposes, always lock before doing anything! */
   if ( vdseLock( &pProcess->memObject, pContext ) ) {
      pCurrentBuffer = (vdseSession*) 
          vdseMallocBlocks( pContext->pAllocator, VDSE_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = vdseSessionInit( pCurrentBuffer, pApiSession, pContext );
         VDS_PRE_CONDITION( ok == true || ok == false );
         if ( ok ) {
            vdseLinkNodeInit( &pCurrentBuffer->node );
            vdseLinkedListPutLast( &pProcess->listOfSessions, 
                                   &pCurrentBuffer->node );
            *ppSession = pCurrentBuffer;
         }
         else {
            vdseFreeBlocks( pContext->pAllocator, 
                            VDSE_ALLOC_ANY,
                            (unsigned char *)pCurrentBuffer, 
                            1, pContext );
         }
      }
      else {
         vdscSetError( &pContext->errorHandler, 
                       g_vdsErrorHandle, 
                       VDS_NOT_ENOUGH_VDS_MEMORY );
      }

      /* 
       * If the init was a success,  this is now initialized. We must
       * add the previouslock otherwise... the unlock will fail (segv).
       */
      if ( pContext->lockOffsets != NULL ) {
         pContext->lockOffsets[*pContext->numLocks] = SET_OFFSET(&pProcess->memObject);
         (*pContext->numLocks)++;
      }
      vdseUnlock( &pProcess->memObject, pContext );
   }
   else {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    VDS_ENGINE_BUSY );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseProcessRemoveSession( vdseProcess        * pProcess,
                               vdseSession        * pSession,
                               vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pProcess != NULL );
   VDS_PRE_CONDITION( pSession != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   /* For recovery purposes, always lock before doing anything! */
   if ( vdseLock( &pProcess->memObject, pContext ) ) {
      vdseLinkedListRemoveItem( &pProcess->listOfSessions, 
                                &pSession->node );
      
      vdseSessionFini( pSession, pContext );

      vdseUnlock( &pProcess->memObject, pContext );
   }
   else {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    VDS_ENGINE_BUSY );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseProcessGetFirstSession( vdseProcess        * pProcess,
                                 vdseSession       ** ppSession,
                                 vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   bool ok;
   
   VDS_PRE_CONDITION( pProcess  != NULL );
   VDS_PRE_CONDITION( ppSession != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );

   ok = vdseLinkedListPeakFirst( &pProcess->listOfSessions, &pNode );
   if ( ok ) {
      *ppSession = (vdseSession *)
         ((char*)pNode - offsetof( vdseSession, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseProcessGetNextSession( vdseProcess        * pProcess,
                                vdseSession        * pCurrent,
                                vdseSession       ** ppNext,
                                vdseSessionContext * pContext )
{
   vdseLinkNode * pNode = NULL;
   bool ok;
   
   VDS_PRE_CONDITION( pProcess != NULL );
   VDS_PRE_CONDITION( pCurrent != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( ppNext   != NULL );

   ok = vdseLinkedListPeakNext( &pProcess->listOfSessions,
                                &pCurrent->node,
                                &pNode );
   if ( ok ) {
      *ppNext = (vdseSession*)
         ((char*)pNode - offsetof( vdseSession, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

