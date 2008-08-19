/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "Nucleus/Process.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcessInit( psnProcess        * pProcess,
                      pid_t                pid,
                      psnSessionContext * pContext )
{
   psoErrors errcode;

   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pid > 0 );

   errcode = psnMemObjectInit( &pProcess->memObject, 
                                PSN_IDENT_PROCESS,
                                &pProcess->blockGroup,
                                1 ); /* A single block */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   pProcess->pid = pid;
   pProcess->processIsTerminating = false;

   psnLinkedListInit( &pProcess->listOfSessions );

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnProcessFini( psnProcess        * pProcess,
                      psnSessionContext * pContext )
{
   psnSession  * pSession = NULL;
   psnLinkNode * pNode    = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pProcess->memObject.objType == PSN_IDENT_PROCESS );

   /*
    * Eliminate all sessions in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( psnLinkedListPeakFirst( &pProcess->listOfSessions, 
                                    &pNode ) ) {
      pSession = (psnSession*)
         ((char*)pNode - offsetof( psnSession, node ));

      ok = psnProcessRemoveSession( pProcess, pSession, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
   }

   /* 
    * This will remove the blocks of allocated memory.
    * It must be the last operation on that piece of memory.
    */
   psnMemObjectFini(  &pProcess->memObject, PSN_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcessAddSession( psnProcess        * pProcess,
                            void               * pApiSession,
                            psnSession       ** ppSession,
                            psnSessionContext * pContext )
{
   psnSession * pCurrentBuffer;
   bool ok = false;
   
   PSO_PRE_CONDITION( pProcess    != NULL );
   PSO_PRE_CONDITION( pApiSession != NULL );
   PSO_PRE_CONDITION( ppSession   != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );

   *ppSession = NULL;
   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pProcess->memObject, pContext ) ) {
      pCurrentBuffer = (psnSession*) 
          psnMallocBlocks( pContext->pAllocator, PSN_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = psnSessionInit( pCurrentBuffer, pApiSession, pContext );
         PSO_PRE_CONDITION( ok == true || ok == false );
         if ( ok ) {
            psnLinkNodeInit( &pCurrentBuffer->node );
            psnLinkedListPutLast( &pProcess->listOfSessions, 
                                   &pCurrentBuffer->node );
            *ppSession = pCurrentBuffer;
         }
         else {
            psnFreeBlocks( pContext->pAllocator, 
                            PSN_ALLOC_ANY,
                            (unsigned char *)pCurrentBuffer, 
                            1, pContext );
         }
      }
      else {
         pscSetError( &pContext->errorHandler, 
                       g_psoErrorHandle, 
                       PSO_NOT_ENOUGH_PSO_MEMORY );
      }

      /* 
       * If the init was a success,  this is now initialized. We must
       * add the previouslock otherwise... the unlock will fail (segv).
       */
      if ( pContext->lockOffsets != NULL ) {
         pContext->lockOffsets[*pContext->numLocks] = SET_OFFSET(&pProcess->memObject);
         (*pContext->numLocks)++;
      }
      psnUnlock( &pProcess->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcessRemoveSession( psnProcess        * pProcess,
                               psnSession        * pSession,
                               psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pProcess->memObject, pContext ) ) {
      psnLinkedListRemoveItem( &pProcess->listOfSessions, 
                                &pSession->node );
      
      psnSessionFini( pSession, pContext );

      psnUnlock( &pProcess->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcessGetFirstSession( psnProcess        * pProcess,
                                 psnSession       ** ppSession,
                                 psnSessionContext * pContext )
{
   psnLinkNode * pNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pProcess  != NULL );
   PSO_PRE_CONDITION( ppSession != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );

   ok = psnLinkedListPeakFirst( &pProcess->listOfSessions, &pNode );
   if ( ok ) {
      *ppSession = (psnSession *)
         ((char*)pNode - offsetof( psnSession, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcessGetNextSession( psnProcess        * pProcess,
                                psnSession        * pCurrent,
                                psnSession       ** ppNext,
                                psnSessionContext * pContext )
{
   psnLinkNode * pNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pCurrent != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( ppNext   != NULL );

   ok = psnLinkedListPeakNext( &pProcess->listOfSessions,
                                &pCurrent->node,
                                &pNode );
   if ( ok ) {
      *ppNext = (psnSession*)
         ((char*)pNode - offsetof( psnSession, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

