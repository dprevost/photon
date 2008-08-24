/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Nucleus/ProcessManager.h"
#include "Nucleus/Process.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcMgrInit( psnProcMgr        * pManager,
                      psnSessionContext * pContext )
{
   psoErrors errcode;

   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   errcode = psnMemObjectInit( &pManager->memObject, 
                                PSN_IDENT_PROCESS_MGR,
                                &pManager->blockGroup,
                                1 ); /* A single block */
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psnLinkedListInit( &pManager->listOfProcesses );
      
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcMgrAddProcess( psnProcMgr        * pManager,
                            pid_t                pid, 
                            psnProcess       ** ppProcess,
                            psnSessionContext * pContext )
{
   psnProcess* pCurrentBuffer;
   bool ok = false;
   
   PSO_PRE_CONDITION( pManager  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( ppProcess != NULL );
   PSO_PRE_CONDITION( pid > 0 );

   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pManager->memObject, pContext ) ) {
      pCurrentBuffer = (psnProcess*)
         psnMallocBlocks( pContext->pAllocator, PSN_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = psnProcessInit( pCurrentBuffer, pid, pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            psnLinkNodeInit( &pCurrentBuffer->node );
            psnLinkedListPutLast( &pManager->listOfProcesses, 
                                   &pCurrentBuffer->node );
            *ppProcess = pCurrentBuffer;
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
      psnUnlock( &pManager->memObject, pContext );
   }
   else {
      pscSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcMgrFindProcess( psnProcMgr        * pManager,
                             pid_t                pid,
                             psnProcess       ** ppProcess,
                             psnSessionContext * pContext )
{
   psnProcess *pCurrent, *pNext;
   psnLinkNode * pNodeCurrent = NULL, * pNodeNext = NULL;
   psoErrors errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pManager  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( ppProcess != NULL );
   PSO_PRE_CONDITION( pid > 0 );
   
   *ppProcess = NULL;
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pManager->memObject, pContext ) ) {
      ok = psnLinkedListPeakFirst( &pManager->listOfProcesses, 
                                    &pNodeCurrent );
      if ( ok ) {
         pCurrent = (psnProcess*)
            ((char*)pNodeCurrent - offsetof( psnProcess, node ));
         if ( pCurrent->pid == pid ) *ppProcess = pCurrent;
      
         while ( (*ppProcess == NULL) &&
                 psnLinkedListPeakNext( &pManager->listOfProcesses, 
                                         pNodeCurrent, 
                                         &pNodeNext ) ) {
            pNext = (psnProcess*)
               ((char*)pNodeNext - offsetof( psnProcess, node ));
            if ( pNext->pid == pid ) *ppProcess = pNext;
            pNodeCurrent = pNodeNext;
         }
      }
      else {
         errcode = PSO_INTERNAL_ERROR;
      }
      
      psnUnlock( &pManager->memObject, pContext );
   }
   else {
      errcode = PSO_ENGINE_BUSY;
   }
   
   /* Is this possible ? */
   if ( *ppProcess == NULL ) errcode = PSO_INTERNAL_ERROR;

   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnProcMgrRemoveProcess( psnProcMgr        * pManager,
                               psnProcess        * pProcess,
                               psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pProcess != NULL );
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psnLock( &pManager->memObject, pContext ) ) {
      psnLinkedListRemoveItem( &pManager->listOfProcesses, 
                                &pProcess->node );
   
      psnProcessFini( pProcess, pContext );
                      
      psnUnlock( &pManager->memObject, pContext );
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

