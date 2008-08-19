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

#include "API/Process.h"
#include "API/WatchdogCommon.h"
#include "Nucleus/MemoryHeader.h"
#include "Nucleus/ProcessManager.h"
#include "Common/MemoryFile.h"
#include "API/Session.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

psaProcess     *g_pProcessInstance = NULL;
pscThreadLock   g_ProcessMutex;
   
/** 
 * Set to true if the program is multithreaded.
 */
bool             g_protectionIsNeeded = false;
                          
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool AreWeTerminated()
{
   bool ret = true;
   
   pscAcquireThreadLock( &g_ProcessMutex );

   if ( g_pProcessInstance->pHeader != NULL ) ret = false;
   
   pscReleaseThreadLock( &g_ProcessMutex );

   return ret;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaProcessInit( psaProcess * process, const char * wdAddress )
{
   struct WDOutput answer;
   char path[PATH_MAX];
   int errcode = VDS_OK;
   psnSessionContext context;
   psnProcMgr* pCleanupManager;
   bool ok;
   
   VDS_PRE_CONDITION( process   != NULL );
   VDS_PRE_CONDITION( wdAddress != NULL );
   
   if ( ! psnInitEngine() ) return VDS_INTERNAL_ERROR;
   
   memset( &context, 0, sizeof(psnSessionContext) );
   context.pidLocker = getpid();
   pscInitErrorHandler( &context.errorHandler );
   
   if ( g_protectionIsNeeded ) {
      pscAcquireThreadLock( &g_ProcessMutex );
   }
   
   if ( process->pHeader != NULL ) {
      errcode = VDS_PROCESS_ALREADY_INITIALIZED;
      goto the_exit;
   }
   
   if ( wdAddress == NULL ) {
      errcode = VDS_INVALID_WATCHDOG_ADDRESS;
      goto the_exit;
   }
   
   errcode = psaConnect( &process->connector, 
                          wdAddress, 
                          &answer, 
                          &context.errorHandler );
   if ( errcode != VDS_OK ) goto the_exit;
   
   /* The watchdog already tested that there is no buffer overflow here. */
   sprintf( path, "%s%s%s", answer.pathname, VDS_DIR_SEPARATOR,
            VDS_MEMFILE_NAME );

   errcode = psaOpenVDS( process,
                          path,
                          answer.memorySizekb,
                          &context );
   if ( errcode != VDS_OK ) goto the_exit;

   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   GET_PTR( pCleanupManager, process->pHeader->processMgrOffset, psnProcMgr );

   ok = psnProcMgrAddProcess( pCleanupManager,
                               getpid(), 
                               &process->pCleanup,
                               &context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ok ) {
      strcpy( process->logDirName, VDS_LOGDIR_NAME );
      g_pProcessInstance = process;
   }
   else {
      errcode = pscGetLastError( &context.errorHandler );
   }
   
the_exit:
   
   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &g_ProcessMutex );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psaProcessFini()
{
   psnProcMgr * processManager = NULL;
   psnSessionContext context;
   psaSession * pApiSession = NULL;
   psnSession * pVdsSession = NULL, * pCurrent;
   int errcode = 0;
   psaProcess * process;
   bool ok;
   
   process = g_pProcessInstance;
   VDS_PRE_CONDITION( process != NULL );

   /* 
    * This can occurs if the process init (psaProcessInit()) 
    * was not called from the C API and somehow failed.
    */
   if ( process->pHeader == NULL ) {
      psaDisconnect( &process->connector, &context.errorHandler );
      return;
   }
   
   memset( &context, 0, sizeof(psnSessionContext) );
   context.pidLocker = getpid();
   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   pscInitErrorHandler( &context.errorHandler );

   if ( g_protectionIsNeeded ) {
      pscAcquireThreadLock( &g_ProcessMutex );
   }
   
   GET_PTR( processManager, process->pHeader->processMgrOffset, psnProcMgr );

   psnProcessNoMoreSessionAllowed( process->pCleanup,
                                    &context );

   if ( psnLock(&process->pCleanup->memObject, &context) ) {
      ok = psnProcessGetFirstSession( process->pCleanup, 
                                       &pVdsSession, 
                                       &context );
      VDS_POST_CONDITION( ok == true || ok == false );
      while ( ok ) {
         pApiSession = pVdsSession->pApiSession;
         if ( pApiSession != NULL ) {
            errcode = psaCloseSession( pApiSession );
            /** \todo handle error here */
         }

         pCurrent = pVdsSession;
         ok = psnProcessGetNextSession( process->pCleanup,
                                         pCurrent,
                                         &pVdsSession,
                                         &context );
      }
      
      psnUnlock( &process->pCleanup->memObject, &context );
   }
   else {
      errcode = VDS_INTERNAL_ERROR;
      goto error_handler;
   }
   
   /*
    * The lock is implicit. This call will also recursively destroy
    * all the psnSession objects of the current process.
    */
   ok = psnProcMgrRemoveProcess( processManager, process->pCleanup, &context );
   VDS_POST_CONDITION( ok == true || ok == false );
   process->pCleanup = NULL;
   
   /* close our access to the VDS */
   if ( process->pHeader != NULL ) {
      psaCloseVDS( process, &context );
      process->pHeader = NULL;
   }
   
error_handler:

   psaDisconnect( &process->connector, &context.errorHandler );

   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &g_ProcessMutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaOpenVDS( psaProcess        * process,
                const char        * memoryFileName,
                size_t              memorySizekb,
                psnSessionContext * pContext )
{
   int errcode = 0;
   bool ok;
   pscMemoryFileStatus fileStatus;
   void * ptr;
   
   VDS_PRE_CONDITION( process        != NULL );
   VDS_PRE_CONDITION( pContext       != NULL );
   VDS_PRE_CONDITION( memoryFileName != NULL );

   process->pHeader = NULL;
   
   pscInitMemoryFile( &process->memoryFile, memorySizekb, memoryFileName );
   
   pscBackStoreStatus( &process->memoryFile, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   ok = pscOpenMemFile( &process->memoryFile, 
                         &ptr, 
                         &pContext->errorHandler );   
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
      return VDS_ERROR_OPENING_VDS;
   }
   
   g_pBaseAddr = (unsigned char * ) ptr;
   process->pHeader = (psnMemoryHeader*) g_pBaseAddr;

   if ( process->pHeader->version != PSN_MEMORY_VERSION ) {
      process->pHeader = NULL;
      return VDS_INCOMPATIBLE_VERSIONS;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psaCloseVDS( psaProcess        * process,
                  psnSessionContext * pContext )
{
   VDS_PRE_CONDITION( process  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   pscCloseMemFile( &process->memoryFile, &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

