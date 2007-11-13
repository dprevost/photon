/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

   if ( g_pProcessInstance->pHeader != NULL )
      ret = false;
   
   vdscReleaseThreadLock( &g_ProcessMutex );

   return ret;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaProcessInit( vdsaProcess * process, const char * wdAddress )
{
   struct WDOutput answer;
   char path[PATH_MAX];
   int errcode = 0;
   vdseSessionContext context;
   vdseProcMgr* pCleanupManager;

   VDS_PRE_CONDITION( process   != NULL );
   VDS_PRE_CONDITION( wdAddress != NULL );
   
   if (vdseInitEngine() != 0 )
      return VDS_INTERNAL_ERROR;
   
   memset( &context, 0, sizeof context );
   context.pidLocker = getpid();
   vdscInitErrorHandler( &context.errorHandler );
   
   if ( g_protectionIsNeeded )
      vdscAcquireThreadLock( &g_ProcessMutex );
   
   if ( process->pHeader != NULL )
   {
      errcode = VDS_PROCESS_ALREADY_INITIALIZED;
      goto the_exit;
   }
   
   if ( wdAddress == NULL )
   {
      errcode = VDS_INVALID_WATCHDOG_ADDRESS;
      goto the_exit;
   }
   
   if ( memcmp( wdAddress, "12348", 5 ) == 0 )
   {
      strcpy( answer.pathname, "/tmp/vdsf_001" );
      answer.memorySizekb = 10000;
   }
   else
      errcode = vdsaConnect( &process->connector, 
                             wdAddress, 
                             &answer, 
                             &context.errorHandler );
   
   if ( errcode != VDS_OK )
      goto the_exit;
   
/*   fprintf( stderr, "w %s-%s-%s-\n", answer.pathname, VDS_DIR_SEPARATOR, */
/*            VDS_MEMFILE_NAME ); */

/*   printf( "w %p%p%p", answer.pathname, VDS_DIR_SEPARATOR, */
/*            VDS_MEMFILE_NAME ); */
/*   sleep( 1 ); */
/*   printf( "q %d%d%d", strlen(answer.pathname), strlen(VDS_DIR_SEPARATOR), */
/*            strlen(VDS_MEMFILE_NAME) ); */
   /* The watchdog already tested that there is no buffer overflow here. */

   sprintf( path, "%s%s%s", answer.pathname, VDS_DIR_SEPARATOR,
            VDS_MEMFILE_NAME );

/*   strcpy( path, "/tmp/vdsf_001/mem_file" ); */
   
   errcode = vdsaOpenVDS( process,
                          path,
                          answer.memorySizekb,
                          &context );
   if ( errcode != VDS_OK )
      goto the_exit;

   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   GET_PTR( pCleanupManager, process->pHeader->cleanupMgrOffset, vdseProcMgr );

   errcode = vdseProcMgrAddProcess( pCleanupManager,
                                    getpid(), 
                                    &process->pCleanup,
                                    &context );
   if ( errcode == 0 )
   {
      strcpy( process->logDirName, VDS_LOGDIR_NAME );
      g_pProcessInstance = process;
   }
   
 the_exit:

   if ( g_protectionIsNeeded )
      vdscReleaseThreadLock( &g_ProcessMutex );

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

   process = g_pProcessInstance;
   VDS_PRE_CONDITION( process != NULL );

   /* 
    * This can occurs if the process init (vdsaProcessInit()) 
    * was not called from the C API and somehow failed. 
    */
   if ( process->pHeader == NULL ) return;

   memset( &context, 0, sizeof context );
   context.pidLocker = getpid();
   GET_PTR( context.pAllocator, process->pHeader->allocatorOffset, void );
   vdscInitErrorHandler( &context.errorHandler );

   if ( g_protectionIsNeeded )
      vdscAcquireThreadLock( &g_ProcessMutex );

   GET_PTR( processManager, process->pHeader->cleanupMgrOffset, vdseProcMgr );

   vdseProcessNoMoreSessionAllowed( process->pCleanup,
                                    &context );

   errcode = vdseLock( &process->pCleanup->memObject, &context );
   if ( errcode == 0 )
   {
      errcode = vdseProcessGetFirstSession( process->pCleanup, 
                                            &pVdsSession, 
                                            &context );
      while ( errcode == 0 )
      {
         pApiSession = pVdsSession->pApiSession;
         if ( pApiSession != NULL ) 
         {
            errcode = vdsaCloseSession( pApiSession );
            /* \todo */
         }

         pCurrent = pVdsSession;
         errcode = vdseProcessGetNextSession( process->pCleanup,
                                              pCurrent,
                                              &pVdsSession,
                                              &context );
      }
      
      vdseUnlock( &process->pCleanup->memObject, &context );
   }
   else
   {
      errcode = VDS_INTERNAL_ERROR;
      goto error_handler;
   }
   
   /*
    * The lock is implicit. This call will also recursively destroy
    * all the vdseSession objects of the current process.
    */
   vdseProcMgrRemoveProcess( processManager, process->pCleanup, &context );
   process->pCleanup = NULL;
   
   /* close our access to the VDS */
   if ( process->pHeader != NULL )
   {
      vdsaCloseVDS( process, &context );
      process->pHeader = NULL;
   }
   
error_handler:

   if ( g_protectionIsNeeded )
      vdscReleaseThreadLock( &g_ProcessMutex );

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaOpenVDS( vdsaProcess        * process,
                 const char         * memoryFileName,
                 size_t               memorySizekb,
                 vdseSessionContext * pSession )
{
   int errcode = 0;
   vdscMemoryFileStatus fileStatus;
   void * ptr;
   
   VDS_PRE_CONDITION( process        != NULL );
   VDS_PRE_CONDITION( pSession       != NULL );
   VDS_PRE_CONDITION( memoryFileName != NULL );

   process->pHeader = NULL;
   
   vdscInitMemoryFile( &process->memoryFile, memorySizekb, memoryFileName );
   
   vdscBackStoreStatus( &process->memoryFile, &fileStatus );
   
   if ( ! fileStatus.fileExist )
   {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   errcode = vdscOpenMemFile( &process->memoryFile, 
                              &ptr, 
                              &pSession->errorHandler );   
   if ( errcode != 0 )
   {
      fprintf( stderr, "MMAP failure - %d %s\n", errno, memoryFileName );
      return VDS_ERROR_OPENING_VDS;
   }
   
   g_pBaseAddr = (unsigned char * ) ptr;
   process->pHeader = (vdseMemoryHeader*) g_pBaseAddr;

   if ( process->pHeader->version != MEMORY_VERSION )
   {
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

