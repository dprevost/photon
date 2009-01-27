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

#include "Nucleus/ProcessManager.h"
#include "Nucleus/Process.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrInit( psonProcMgr        * pManager,
                      psonSessionContext * pContext )
{
   psoErrors errcode;

   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   errcode = psonMemObjectInit( &pManager->memObject, 
                                PSON_IDENT_PROCESS_MGR,
                                &pManager->blockGroup,
                                1 ); /* A single block */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler,
                    g_psoErrorHandle,
                    errcode );
      return false;
   }

   psonLinkedListInit( &pManager->listOfProcesses );
      
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrAddProcess( psonProcMgr        * pManager,
                            pid_t                pid, 
                            psonProcess       ** ppProcess,
                            psonSessionContext * pContext )
{
   psonProcess* pCurrentBuffer;
   bool ok = false;
   
   PSO_PRE_CONDITION( pManager  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( ppProcess != NULL );
   PSO_PRE_CONDITION( pid > 0 );

   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pManager->memObject, pContext ) ) {
      pCurrentBuffer = (psonProcess*)
         psonMallocBlocks( pContext->pAllocator, PSON_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL ) {
         ok = psonProcessInit( pCurrentBuffer, pid, pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            psonLinkNodeInit( &pCurrentBuffer->node );
            psonLinkedListPutLast( &pManager->listOfProcesses, 
                                   &pCurrentBuffer->node );
            *ppProcess = pCurrentBuffer;
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
      psonUnlock( &pManager->memObject, pContext );
   }
   else {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    PSO_ENGINE_BUSY );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrFindProcess( psonProcMgr        * pManager,
                             pid_t                pid,
                             psonProcess       ** ppProcess,
                             psonSessionContext * pContext )
{
   psonProcess *pCurrent, *pNext;
   psonLinkNode * pNodeCurrent = NULL, * pNodeNext = NULL;
   psoErrors errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pManager  != NULL );
   PSO_PRE_CONDITION( pContext  != NULL );
   PSO_PRE_CONDITION( ppProcess != NULL );
   PSO_PRE_CONDITION( pid > 0 );
   
   *ppProcess = NULL;
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pManager->memObject, pContext ) ) {
      ok = psonLinkedListPeakFirst( &pManager->listOfProcesses, 
                                    &pNodeCurrent );
      if ( ok ) {
         pCurrent = (psonProcess*)
            ((char*)pNodeCurrent - offsetof( psonProcess, node ));
         if ( pCurrent->pid == pid ) *ppProcess = pCurrent;
      
         while ( (*ppProcess == NULL) &&
                 psonLinkedListPeakNext( &pManager->listOfProcesses, 
                                         pNodeCurrent, 
                                         &pNodeNext ) ) {
            pNext = (psonProcess*)
               ((char*)pNodeNext - offsetof( psonProcess, node ));
            if ( pNext->pid == pid ) *ppProcess = pNext;
            pNodeCurrent = pNodeNext;
         }
      }
      else {
         errcode = PSO_INTERNAL_ERROR;
      }
      
      psonUnlock( &pManager->memObject, pContext );
   }
   else {
      errcode = PSO_ENGINE_BUSY;
   }
   
   /* Is this possible ? */
   if ( *ppProcess == NULL ) errcode = PSO_INTERNAL_ERROR;

   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, 
                    g_psoErrorHandle, 
                    errcode );
      return false;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonProcMgrRemoveProcess( psonProcMgr        * pManager,
                               psonProcess        * pProcess,
                               psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pManager != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pProcess != NULL );
   
   /* For recovery purposes, always lock before doing anything! */
   if ( psonLock( &pManager->memObject, pContext ) ) {
      psonLinkedListRemoveItem( &pManager->listOfProcesses, 
                                &pProcess->node );
   
      psonProcessFini( pProcess, pContext );
                      
      psonUnlock( &pManager->memObject, pContext );
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

