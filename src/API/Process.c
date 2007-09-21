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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsaProcess     *g_pProcessInstance = NULL;
vdscThreadLock   g_ProcessMutex;
   
/** 
 * Set to true if the program is multithreaded.
 */
bool             g_protectionIsNeeded = false;

static int vdsaOpenVDS( vdsaProcess * process,
                        const char  * memoryFileName,
                        size_t        memorySizekb );

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

int vdsaProcessInit( vdsaProcess *process, const char  *wdAddress )
{
   struct WDOutput answer;
   char path[PATH_MAX];
   int errcode = VDS_PROCESS_ALREADY_INITIALIZED;
   vdseSessionContext context;
   vdseProcMgr* pCleanupManager;

   memset( &context, 0, sizeof context );
   context.lockValue = getpid();
   vdscInitErrorHandler( &context.errorHandler );
   
   if ( g_protectionIsNeeded )
      vdscAcquireThreadLock( &g_ProcessMutex );
   
   if ( process->pHeader != NULL )
      goto the_exit;
   
   if ( wdAddress == NULL )
   {
      errcode = VDS_INVALID_WATCHDOG_ADDRESS;
      goto the_exit;
   }
   
   if ( memcmp( wdAddress, "12348", 5 ) == 0 )
   {
      errcode = 0;
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
   fprintf( stderr, "path -%s-\n", path );

/*   strcpy( path, "/tmp/vdsf_001/mem_file" ); */
   
   errcode = vdsaOpenVDS( process,
                          path,
                          answer.memorySizekb );
   if ( errcode != VDS_OK )
      goto the_exit;

   context.pAllocator = GET_PTR( &process->pHeader->allocatorOffset, void );
   
   pCleanupManager = GET_PTR( process->pHeader->cleanupMgrOffset, 
                              vdseProcMgr );
   errcode = vdseProcMgrAddProcess( pCleanupManager,
                                    getpid(), 
                                    &process->pCleanup,
                                    &context );
   if ( errcode == 0 )
   {
      strcpy( process->logDirName, VDS_LOGDIR_NAME );
   }
   
 the_exit:

   if ( g_protectionIsNeeded )
      vdscReleaseThreadLock( &g_ProcessMutex );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsaProcessFini( vdsaProcess *process )
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaOpenVDS( vdsaProcess * process,
                 const char  * memoryFileName,
                 size_t        memorySizekb )
{
   int errcode = 0;
   vdscMemoryFileStatus fileStatus;
   vdscErrorHandler errorHandler;
   void * ptr;
   
   process->pHeader = NULL;
   
   vdscInitErrorHandler( &errorHandler );
   
   vdscInitMemoryFile( &process->memoryFile, memorySizekb, memoryFileName );
   
   vdscBackStoreStatus( &process->memoryFile, &fileStatus );
   
   if ( ! fileStatus.fileExist )
   {
      return VDS_BACKSTORE_FILE_MISSING;
   }
   
   errcode = vdscOpenMemFile( &process->memoryFile, &ptr, &errorHandler );   
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

void vdsaCloseVDS( vdsaProcess      * process,
                   vdscErrorHandler * pError )
{
   vdscCloseMemFile( &process->memoryFile, pError );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

