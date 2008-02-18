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

#include "Engine/ProcessManager.h"
#include "Engine/Process.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcMgrInit( vdseProcMgr        * pManager,
                     vdseSessionContext * pContext )
{
   vdsErrors errcode;

   VDS_PRE_CONDITION( pManager != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   errcode = vdseMemObjectInit( &pManager->memObject, 
                                VDSE_IDENT_PROCESS_MGR,
                                &pManager->blockGroup,
                                1 ); /* A single block */
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler,
                    g_vdsErrorHandle,
                    errcode );
      return -1;
   }

   vdseLinkedListInit( &pManager->listOfProcesses );
      
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcMgrAddProcess( vdseProcMgr        * pManager,
                           pid_t                pid, 
                           vdseProcess       ** ppProcess,
                           vdseSessionContext * pContext )
{
   int errcode, rc = -1;
   vdseProcess* pCurrentBuffer;

   VDS_PRE_CONDITION( pManager  != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( ppProcess != NULL );
   VDS_PRE_CONDITION( pid > 0 );

   /* For recovery purposes, always lock before doing anything! */
   errcode = vdseLock( &pManager->memObject, pContext );
   if ( errcode == 0 )
   {
      pCurrentBuffer = (vdseProcess*)
         vdseMallocBlocks( pContext->pAllocator, VDSE_ALLOC_ANY, 1, pContext );
      if ( pCurrentBuffer != NULL )
      {
         errcode = vdseProcessInit( pCurrentBuffer, pid, pContext );
         if ( errcode == 0 )
         {
            vdseLinkNodeInit( &pCurrentBuffer->node );
            vdseLinkedListPutLast( &pManager->listOfProcesses, 
                                   &pCurrentBuffer->node );
            *ppProcess = pCurrentBuffer;
            rc = 0;
         }
         else
         {
            vdseFreeBlocks( pContext->pAllocator, 
                            VDSE_ALLOC_ANY,
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
      vdseUnlock( &pManager->memObject, pContext );
   }
   else
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    VDS_ENGINE_BUSY );

   return rc;
}


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcMgrFindProcess( vdseProcMgr        * pManager,
                            pid_t                pid,
                            vdseProcess       ** ppProcess,
                            vdseSessionContext * pContext )
{
   enum ListErrors err = LIST_OK;
   vdseProcess *pCurrent, *pNext;
   vdseLinkNode * pNodeCurrent = NULL, * pNodeNext = NULL;
   vdsErrors errcode = VDS_OK;

   VDS_PRE_CONDITION( pManager  != NULL );
   VDS_PRE_CONDITION( pContext  != NULL );
   VDS_PRE_CONDITION( ppProcess != NULL );
   VDS_PRE_CONDITION( pid > 0 );
   
   *ppProcess = NULL;
   
   /* For recovery purposes, always lock before doing anything! */
   if ( vdseLock( &pManager->memObject, pContext ) == 0 )
   {
      err = vdseLinkedListPeakFirst( &pManager->listOfProcesses, 
                                     &pNodeCurrent );
      if ( err == LIST_OK )
      {
         pCurrent = (vdseProcess*)
            ((char*)pNodeCurrent - offsetof( vdseProcess, node ));
         if ( pCurrent->pid == pid )
            *ppProcess = pCurrent;
      
         while ( (*ppProcess == NULL) &&
                 (vdseLinkedListPeakNext( &pManager->listOfProcesses, 
                                          pNodeCurrent, 
                                          &pNodeNext ) == LIST_OK) )
         {
            pNext = (vdseProcess*)
               ((char*)pNodeNext - offsetof( vdseProcess, node ));
            if ( pNext->pid == pid )
               *ppProcess = pNext;
            pNodeCurrent = pNodeNext;
         }
      }
      else
         errcode = VDS_INTERNAL_ERROR;
      
      vdseUnlock( &pManager->memObject, pContext );
   }
   else
      errcode = VDS_ENGINE_BUSY;

   /* Is this possible ? */
   if ( *ppProcess == NULL )
      errcode = VDS_INTERNAL_ERROR;

   if ( errcode !=VDS_OK )
   {
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    errcode );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcMgrRemoveProcess( vdseProcMgr        * pManager,
                              vdseProcess        * pProcess,
                              vdseSessionContext * pContext )
{
   int errcode = 0;

   VDS_PRE_CONDITION( pManager != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pProcess != NULL );
   
   /* For recovery purposes, always lock before doing anything! */
   errcode = vdseLock( &pManager->memObject, pContext );
   if ( errcode == 0 )
   {
      vdseLinkedListRemoveItem( &pManager->listOfProcesses, 
                                &pProcess->node );
   
      vdseProcessFini( pProcess, pContext );
                      
      vdseUnlock( &pManager->memObject, pContext );
   }
   else
      vdscSetError( &pContext->errorHandler, 
                    g_vdsErrorHandle, 
                    VDS_ENGINE_BUSY );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

