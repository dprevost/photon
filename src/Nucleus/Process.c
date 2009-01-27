/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Process.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessInit( psonProcess        * pProcess,
                      pid_t                pid,
                      psonSessionContext * pContext )
{
   psoErrors errcode;

   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pid > 0 );

   errcode = psonMemObjectInit( &pProcess->memObject, 
                                PSON_IDENT_PROCESS,
                                &pProcess->blockGroup,
                                1 ); /* A single block */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   pProcess->pid = pid;
   pProcess->processIsTerminating = false;

   psonLinkedListInit( &pProcess->listOfSessions );

   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonProcessFini( psonProcess        * pProcess,
                      psonSessionContext * pContext )
{
   psonSession  * pSession = NULL;
   psonLinkNode * pNode    = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pProcess->memObject.objType == PSON_IDENT_PROCESS );

   /*
    * Eliminate all sessions in the list. This is probably not needed
    * as we will release the blocks of memory to the allocator as the
    * last step. This might be reviewed eventually.
    */

   while ( psonLinkedListPeakFirst( &pProcess->listOfSessions, 
                                    &pNode ) ) {
      pSession = (psonSession*)
         ((char*)pNode - offsetof( psonSession, node ));

      ok = psonProcessRemoveSession( pProcess, pSession, pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
   }

   /* 
    * This will remove the blocks of allocated memory.
    * It must be the last operation on that piece of memory.
    */
   psonMemObjectFini(  &pProcess->memObject, PSON_ALLOC_ANY, pContext );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessAddSession( psonProcess        * pProcess,
                            void               * pApiSession,
                            psonSession       ** ppSession,
                            psonSessionContext * pContext )
{
   psonSession * pCurrentBuffer;
   bool ok = false;
   
   PSO_PRE_CONDITION( pProcess    != NULL );
   PSO_PRE_CONDITION( pApiSession != NULL );
   PSO_PRE_CONDITION( ppSession   != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );

   *ppSession = NULL;
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pProcess->memObject, pContext ) ) {
      pCurrentBuffer = (psonSession*) 
          psonMallocBlocks( pContext->pAllocator, PSON_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = psonSessionInit( pCurrentBuffer, pApiSession, pContext );
         PSO_PRE_CONDITION( ok == true || ok == false );
         if ( ok ) {
            psonLinkNodeInit( &pCurrentBuffer->node );
            psonLinkedListPutLast( &pProcess->listOfSessions, 
                                   &pCurrentBuffer->node );
            *ppSession = pCurrentBuffer;
         }
         else {
            psonFreeBlocks( pContext->pAllocator, 
                            PSON_ALLOC_ANY,
                            (unsigned char *)pCurrentBuffer, 
                            1, pContext );
         }
      }
      else {
         psocSetError( &pContext->errorHandler, 
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
      psonUnlock( &pProcess->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessRemoveSession( psonProcess        * pProcess,
                               psonSession        * pSession,
                               psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pSession != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pProcess->memObject, pContext ) ) {
      psonLinkedListRemoveItem( &pProcess->listOfSessions, 
                                &pSession->node );
      
      psonSessionFini( pSession, pContext );

      psonUnlock( &pProcess->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
      return false;
   }
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessGetFirstSession( psonProcess        * pProcess,
                                 psonSession       ** ppSession,
                                 psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pProcess  != NULL );
   PSO_PRE_CONDITION( ppSession != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );

   ok = psonLinkedListPeakFirst( &pProcess->listOfSessions, &pNode );
   if ( ok ) {
      *ppSession = (psonSession *)
         ((char*)pNode - offsetof( psonSession, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcessGetNextSession( psonProcess        * pProcess,
                                psonSession        * pCurrent,
                                psonSession       ** ppNext,
                                psonSessionContext * pContext )
{
   psonLinkNode * pNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pProcess != NULL );
   PSO_PRE_CONDITION( pCurrent != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( ppNext   != NULL );

   ok = psonLinkedListPeakNext( &pProcess->listOfSessions,
                                &pCurrent->node,
                                &pNode );
   if ( ok ) {
      *ppNext = (psonSession*)
         ((char*)pNode - offsetof( psonSession, node ));
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

