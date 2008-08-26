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
   int errcode = PSO_OK;
   psnSessionContext context;
   psnProcMgr* pCleanupManager;
   bool ok;
   
   PSO_PRE_CONDITION( process   != NULL );
   PSO_PRE_CONDITION( wdAddress != NULL );
   
   if ( ! psnInitEngine() ) return PSO_INTERNAL_ERROR;
   
   memset( &context, 0, sizeof(psnSessionContext) );
   context.pidLocker = getpid();
   pscInitErrorHandler( &context.errorHandler );
   
   if ( g_protectionIsNeeded ) {
      pscAcquireThreadLock( &g_ProcessMutex );
   }
   
   if ( process->pHeader != NULL ) {
      errcode = PSO_PROCESS_ALREADY_INITIALIZED;
      goto the_exit;
   }
   
   if ( wdAddress == NULL ) {
      errcode = PSO_INVALID_WATCHDOG_ADDRESS;
      goto the_exit;
   }
   
   errcode = psaConnect( &process->connector, 
                          wdAddress, 
                          &answer, 
                          &context.errorHandler );
   if ( errcode != PSO_OK ) goto the_exit;
   
   /* The watchdog already tested that there is no buffer overflow here. */
   sprintf( path, "%s%s%s", answer.pathname, PSO_DIR_SEPARATOR,
            PSO_MEMFILE_NAME );

   errcode = psoaOpenMemory( process,
                             path,
                             answer.memorySizekb,
                             &context );
   if ( errcode != PSO_OK ) goto the_exit;

   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   GET_PTR( pCleanupManager, process->pHeader->processMgrOffset, psnProcMgr );

   ok = psnProcMgrAddProcess( pCleanupManager,
                               getpid(), 
                               &process->pCleanup,
                               &context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ok ) {
      strcpy( process->logDirName, PSO_LOGDIR_NAME );
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
   psnSession * pMemSession = NULL, * pCurrent;
   int errcode = 0;
   psaProcess * process;
   bool ok;
   
   process = g_pProcessInstance;
   PSO_PRE_CONDITION( process != NULL );

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
                                       &pMemSession, 
                                       &context );
      PSO_POST_CONDITION( ok == true || ok == false );
      while ( ok ) {
         pApiSession = pMemSession->pApiSession;
         if ( pApiSession != NULL ) {
            errcode = psaCloseSession( pApiSession );
            /** \todo handle error here */
         }

         pCurrent = pMemSession;
         ok = psnProcessGetNextSession( process->pCleanup,
                                         pCurrent,
                                         &pMemSession,
                                         &context );
      }
      
      psnUnlock( &process->pCleanup->memObject, &context );
   }
   else {
      errcode = PSO_INTERNAL_ERROR;
      goto error_handler;
   }
   
   /*
    * The lock is implicit. This call will also recursively destroy
    * all the psnSession objects of the current process.
    */
   ok = psnProcMgrRemoveProcess( processManager, process->pCleanup, &context );
   PSO_POST_CONDITION( ok == true || ok == false );
   process->pCleanup = NULL;
   
   /* close our access to the shared memory */
   if ( process->pHeader != NULL ) {
      psoaCloseMemory( process, &context );
      process->pHeader = NULL;
   }
   
error_handler:

   psaDisconnect( &process->connector, &context.errorHandler );

   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &g_ProcessMutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaOpenMemory( psaProcess        * process,
                    const char        * memoryFileName,
                    size_t              memorySizekb,
                    psnSessionContext * pContext )
{
   int errcode = 0;
   bool ok;
   pscMemoryFileStatus fileStatus;
   void * ptr;
   
   PSO_PRE_CONDITION( process        != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_PRE_CONDITION( memoryFileName != NULL );

   process->pHeader = NULL;
   
   pscInitMemoryFile( &process->memoryFile, memorySizekb, memoryFileName );
   
   pscBackStoreStatus( &process->memoryFile, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      return PSO_BACKSTORE_FILE_MISSING;
   }
   
   ok = pscOpenMemFile( &process->memoryFile, 
                         &ptr, 
                         &pContext->errorHandler );   
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
      return PSO_ERROR_OPENING_MEMORY;
   }
   
   g_pBaseAddr = (unsigned char * ) ptr;
   process->pHeader = (psnMemoryHeader*) g_pBaseAddr;

   if ( process->pHeader->version != PSN_MEMORY_VERSION ) {
      process->pHeader = NULL;
      return PSO_INCOMPATIBLE_VERSIONS;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaCloseMemory( psaProcess        * process,
                      psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( process  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   pscCloseMemFile( &process->memoryFile, &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

