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

#include "API/Process.h"
#include "API/QuasarCommon.h"
#include "Nucleus/MemoryHeader.h"
#include "Nucleus/ProcessManager.h"
#include "Common/MemoryFile.h"
#include "API/Session.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

psoaProcess     *g_pProcessInstance = NULL;
psocThreadLock   g_ProcessMutex;
   
int psoaStandalone( const char       * address,
                    char             * path,
                    size_t           * memorySizekb,
                    psocErrorHandler * errorHandler );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool AreWeTerminated()
{
   bool ret = true;
   
   psocAcquireThreadLock( &g_ProcessMutex );

   if ( g_pProcessInstance->pHeader != NULL ) ret = false;
   
   psocReleaseThreadLock( &g_ProcessMutex );

   return ret;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaProcessInit( psoaProcess * process, 
                     const char  * qsrAddress,
                     const char  * processName )
{
   struct qsrOutput answer;
   char path[PATH_MAX];
   int errcode = PSO_OK;
   psonSessionContext context;
   psonProcMgr* pCleanupManager;
   bool ok;
   size_t memorySizekb = 0;
   
   PSO_PRE_CONDITION( process   != NULL );
   PSO_PRE_CONDITION( qsrAddress != NULL );
   
   if ( ! psonInitEngine() ) return PSO_INTERNAL_ERROR;
   
   memset( &context, 0, sizeof(psonSessionContext) );
   context.pidLocker = getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   psocAcquireThreadLock( &g_ProcessMutex );
   
   if ( process->pHeader != NULL ) {
      errcode = PSO_PROCESS_ALREADY_INITIALIZED;
      goto the_exit;
   }
   
   if ( qsrAddress == NULL ) {
      errcode = PSO_INVALID_QUASAR_ADDRESS;
      goto the_exit;
   }
   
   process->standalone = false;
   if ( (strlen(qsrAddress) > strlen("file:")) && 
      (strncmp(qsrAddress, "file:", strlen("file:")) == 0) ) {
      process->standalone = true;
      errcode = psoaStandalone( qsrAddress,
                                path,
                                &memorySizekb,
                                &context.errorHandler );
   }
   else {
      errcode = psoaConnect( &process->connector, 
                             qsrAddress, 
                             &answer, 
                             &context.errorHandler );
   }
   if ( errcode != PSO_OK ) goto the_exit;
   
   if ( ! process->standalone ) {
      /* The server already tested that there is no buffer overflow here. */
      sprintf( path, "%s%s%s", answer.pathname, PSO_DIR_SEPARATOR,
               PSO_MEMFILE_NAME );
      memorySizekb = answer.memorySizekb;
   }
   
   errcode = psoaOpenMemory( process,
                             path,
                             memorySizekb,
                             &context );
   if ( errcode != PSO_OK ) goto the_exit;

   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   GET_PTR( pCleanupManager, process->pHeader->processMgrOffset, psonProcMgr );

   ok = psonProcMgrAddProcess( pCleanupManager,
                               getpid(), 
                               &process->pCleanup,
                               &context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ok ) {
      strcpy( process->logDirName, PSO_LOGDIR_NAME );
      g_pProcessInstance = process;
   }
   else {
      errcode = psocGetLastError( &context.errorHandler );
   }
   
the_exit:
   
   psocReleaseThreadLock( &g_ProcessMutex );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaProcessFini()
{
   psonProcMgr * processManager = NULL;
   psonSessionContext context;
   psoaSession * pApiSession = NULL;
   psonSession * pMemSession = NULL, * pCurrent;
   int errcode = 0;
   psoaProcess * process;
   bool ok;
   
   process = g_pProcessInstance;
   PSO_PRE_CONDITION( process != NULL );

   /* 
    * This can occurs if the process init (psoaProcessInit()) 
    * was not called from the C API and somehow failed.
    */
   if ( process->pHeader == NULL ) {
      if ( ! process->standalone ) {
         psoaDisconnect( &process->connector, &context.errorHandler );
      }
      return;
   }
   
   memset( &context, 0, sizeof(psonSessionContext) );
   context.pidLocker = getpid();
   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   psocInitErrorHandler( &context.errorHandler );

   psocAcquireThreadLock( &g_ProcessMutex );
   
   GET_PTR( processManager, process->pHeader->processMgrOffset, psonProcMgr );

   psonProcessNoMoreSessionAllowed( process->pCleanup,
                                    &context );

   if ( psonLock(&process->pCleanup->memObject, &context) ) {
      ok = psonProcessGetFirstSession( process->pCleanup, 
                                       &pMemSession, 
                                       &context );
      PSO_POST_CONDITION( ok == true || ok == false );
      while ( ok ) {
         pApiSession = pMemSession->pApiSession;
         if ( pApiSession != NULL ) {
            errcode = psoaCloseSession( pApiSession );
            /** \todo handle error here */
         }

         pCurrent = pMemSession;
         ok = psonProcessGetNextSession( process->pCleanup,
                                         pCurrent,
                                         &pMemSession,
                                         &context );
      }
      
      psonUnlock( &process->pCleanup->memObject, &context );
   }
   else {
      errcode = PSO_INTERNAL_ERROR;
      goto error_handler;
   }
   
   /*
    * The lock is implicit. This call will also recursively destroy
    * all the psonSession objects of the current process.
    */
   ok = psonProcMgrRemoveProcess( processManager, process->pCleanup, &context );
   PSO_POST_CONDITION( ok == true || ok == false );
   process->pCleanup = NULL;
   
   /* close our access to the shared memory */
   if ( process->pHeader != NULL ) {
      psoaCloseMemory( process, &context );
      process->pHeader = NULL;
   }
   
error_handler:

   if ( ! process->standalone ) {
      psoaDisconnect( &process->connector, &context.errorHandler );
   }
   
   psocReleaseThreadLock( &g_ProcessMutex );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaOpenMemory( psoaProcess        * process,
                    const char         * memoryFileName,
                    size_t               memorySizekb,
                    psonSessionContext * pContext )
{
   int errcode = 0;
   bool ok;
   psocMemoryFileStatus fileStatus;
   void * ptr;
   
   PSO_PRE_CONDITION( process        != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );
   PSO_PRE_CONDITION( memoryFileName != NULL );

   process->pHeader = NULL;
   
   psocInitMemoryFile( &process->memoryFile, memorySizekb, memoryFileName );
   
   psocBackStoreStatus( &process->memoryFile, &fileStatus );
   
   if ( ! fileStatus.fileExist ) {
      return PSO_BACKSTORE_FILE_MISSING;
   }
   
   ok = psocOpenMemFile( &process->memoryFile, 
                         &ptr, 
                         &pContext->errorHandler );   
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
      return PSO_ERROR_OPENING_MEMORY;
   }
   
   g_pBaseAddr = (unsigned char * ) ptr;
   process->pHeader = (psonMemoryHeader*) g_pBaseAddr;

   if ( process->pHeader->version != PSON_MEMORY_VERSION ) {
      process->pHeader = NULL;
      return PSO_INCOMPATIBLE_VERSIONS;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaCloseMemory( psoaProcess        * process,
                      psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( process  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   psocCloseMemFile( &process->memoryFile, &pContext->errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

