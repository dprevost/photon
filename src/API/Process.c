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

#include "API/Process.h"
#include "API/WatchdogCommon.h"
#include "Engine/MemoryHeader.h"
#include "Engine/ProcessManager.h"
#include "Common/MemoryFile.h"
#include "API/Session.h"
#include "Engine/InitEngine.h"
#include "Engine/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsaProcess     *g_pProcessInstance = NULL;
vdscThreadLock   g_ProcessMutex;
   
/** 
 * Set to true if the program is multithreaded.
 */
bool             g_protectionIsNeeded = false;
                          
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool AreWeTerminated()
{
   bool ret = true;
   
   vdscAcquireThreadLock( &g_ProcessMutex );

   if ( g_pProcessInstance->pHeader != NULL ) ret = false;
   
   vdscReleaseThreadLock( &g_ProcessMutex );

   return ret;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaProcessInit( vdsaProcess * process, const char * wdAddress )
{
   struct WDOutput answer;
   char path[PATH_MAX];
   int errcode = VDS_OK;
   vdseSessionContext context;
   vdseProcMgr* pCleanupManager;
   bool ok;
   
   VDS_PRE_CONDITION( process   != NULL );
   VDS_PRE_CONDITION( wdAddress != NULL );
   
   if ( ! vdseInitEngine() ) return VDS_INTERNAL_ERROR;
   
   memset( &context, 0, sizeof(vdseSessionContext) );
   context.pidLocker = getpid();
   vdscInitErrorHandler( &context.errorHandler );
   
   if ( g_protectionIsNeeded ) {
      vdscAcquireThreadLock( &g_ProcessMutex );
   }
   
   if ( process->pHeader != NULL ) {
      errcode = VDS_PROCESS_ALREADY_INITIALIZED;
      goto the_exit;
   }
   
   if ( wdAddress == NULL ) {
      errcode = VDS_INVALID_WATCHDOG_ADDRESS;
      goto the_exit;
   }
   
   errcode = vdsaConnect( &process->connector, 
                          wdAddress, 
                          &answer, 
                          &context.errorHandler );
   if ( errcode != VDS_OK ) goto the_exit;
   
   /* The watchdog already tested that there is no buffer overflow here. */
   sprintf( path, "%s%s%s", answer.pathname, VDS_DIR_SEPARATOR,
            VDS_MEMFILE_NAME );

   errcode = vdsaOpenVDS( process,
                          path,
                          answer.memorySizekb,
                          &context );
   if ( errcode != VDS_OK ) goto the_exit;

   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   GET_PTR( pCleanupManager, process->pHeader->processMgrOffset, vdseProcMgr );

   ok = vdseProcMgrAddProcess( pCleanupManager,
                               getpid(), 
                               &process->pCleanup,
                               &context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ok ) {
      strcpy( process->logDirName, VDS_LOGDIR_NAME );
      g_pProcessInstance = process;
   }
   else {
      errcode = vdscGetLastError( &context.errorHandler );
   }
   
the_exit:
   
   if ( g_protectionIsNeeded ) {
      vdscReleaseThreadLock( &g_ProcessMutex );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsaProcessFini()
{
   vdseProcMgr * processManager = NULL;
   vdseSessionContext context;
   vdsaSession * pApiSession = NULL;
   vdseSession * pVdsSession = NULL, * pCurrent;
   int errcode = 0;
   vdsaProcess * process;
   bool ok;
   
   process = g_pProcessInstance;
   VDS_PRE_CONDITION( process != NULL );

   /* 
    * This can occurs if the process init (vdsaProcessInit()) 
    * was not called from the C API and somehow failed.
    */
   if ( process->pHeader == NULL ) {
      vdsaDisconnect( &process->connector, &context.errorHandler );
      return;
   }
   
   memset( &context, 0, sizeof(vdseSessionContext) );
   context.pidLocker = getpid();
   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   vdscInitErrorHandler( &context.errorHandler );

   if ( g_protectionIsNeeded ) {
      vdscAcquireThreadLock( &g_ProcessMutex );
   }
   
   GET_PTR( processManager, process->pHeader->processMgrOffset, vdseProcMgr );

   vdseProcessNoMoreSessionAllowed( process->pCleanup,
                                    &context );

   if ( vdseLock(&process->pCleanup->memObject, &context) ) {
      ok = vdseProcessGetFirstSession( process->pCleanup, 
                                       &pVdsSession, 
                                       &context );
      VDS_POST_CONDITION( ok == true || ok == false );
      while ( ok ) {
         pApiSession = pVdsSession->pApiSession;
         if ( pApiSession != NULL ) {
            errcode = vdsaCloseSession( pApiSession );
            /** \todo handle error here */
         }

         pCurrent = pVdsSession;
         ok = vdseProcessGetNextSession( process->pCleanup,
                                         pCurrent,
                                         &pVdsSession,
                                         &context );
      }
      
      vdseUnlock( &process->pCleanup->memObject, &context );
   }
   else {
      errcode = VDS_INTERNAL_ERROR;
      goto error_handler;
   }
   
   /*
    * The lock is implicit. This call will also recursively destroy
    * all the vdseSession objects of the current process.
    */
   ok = vdseProcMgrRemoveProcess( processManager, process->pCleanup, &context );
   VDS_POST_CONDITION( ok == true || ok == false );
   process->pCleanup = NULL;
   
   /* close our access to the VDS */
   if ( process->pHeader != NULL ) {
      vdsaCloseVDS( process, &context );
      process->pHeader = NULL;
   }
   
error_handler:

   vdsaDisconnect( &process->connector, &context.errorHandler );

   if ( g_protectionIsNeeded ) {
      vdscReleaseThreadLock( &g_ProcessMutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaOpenVDS( vdsaProcess        * process,
                 const char         * memoryFileName,
                 size_t               memorySizekb,
                 vdseSessionContext * pContext )
{
   int errcode = 0;
   bool ok;
   vdscMemoryFileStatus fileStatus;
   void * ptr;
   
   VDS_PRE_CONDITION( process        != NULL );
   VDS_PRE_CONDITION( pContext       != NULL );
   VDS_PRE_CONDITION( memoryFileName != NULL );

   process->pHeader = NULL;
   
   vdscInitMemoryFile( &process->memoryFile, memorySizekb, memoryFileName );
   
   vdscBackStoreStatus( &process->memoryFile, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   ok = vdscOpenMemFile( &process->memoryFile, 
                         &ptr, 
                         &pContext->errorHandler );   
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
      return VDS_ERROR_OPENING_VDS;
   }
   
   g_pBaseAddr = (unsigned char * ) ptr;
   process->pHeader = (vdseMemoryHeader*) g_pBaseAddr;

   if ( process->pHeader->version != VDSE_MEMORY_VERSION ) {
      process->pHeader = NULL;
      return VDS_INCOMPATIBLE_VERSIONS;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsaCloseVDS( vdsaProcess        * process,
                   vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( process  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   vdscCloseMemFile( &process->memoryFile, &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

