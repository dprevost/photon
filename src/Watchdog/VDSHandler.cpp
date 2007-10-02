/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include "VDSHandler.h"
#include "MemoryManager.h"
#include "Watchdog.h"
#include "API/WatchdogCommon.h"
#include "Engine/ProcessManager.h"
#include "Engine/SessionContext.h"
#include "Engine/InitEngine.h"
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswHandler::vdswHandler()
   : m_pConfig ( NULL )
{
   memset( &m_context, 0, sizeof(vdseSessionContext) );
   m_context.lockValue = getpid();
   
   int errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      fprintf( stderr, "Abnormal error at line %d in VdsHandler.cpp\n", __LINE__ );
         exit(1);
   }
   vdscInitErrorHandler( &m_context.errorHandler );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswHandler::~vdswHandler()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void
vdswHandler::CleanSession( vdseSession* pSession )
{
   fprintf( stderr, "Session %p\n", pSession );

/*
- need to loop on all objects to see if we hold a lock somewhere
 */
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void
vdswHandler::HandleCrash( pid_t pid )
{
//   int errcode;
#if 0   
   vdseProcess* pProcess = NULL;
   vdseSessionContext context;
   vdseSession* pFirstSession = NULL;
   vdseSession* pCurrSession  = NULL;
   vdseSession* pNextSession  = NULL;

   memset( &context, 0, sizeof context );
   context.lockValue = getpid();
   context.pAllocator = &m_pMemHeader->allocator;
   
//      GET_PTR( m_pMemHeader->allocatorOffset, MemoryAllocator );

   vdseProcessManager* pCleanupManager = 
      GET_PTR( m_pMemHeader->cleanupMgrOffset, vdseProcessManager );

   // Start by checking if we are not holding the lock to the 
   // cleanup manager, the memory allocator, etc...

   //////////////////////////////// ADD CODE HERE

   // Find the linked list of all "recovery information" for our crash
   // process - more specifically there are:
   //  - one or more linked list, one per session, for all objects 
   //    associated with that session
   //  - and a process wide linked list of all these session-related 
   //    linked lists
   // (to be complete: the cleanup manager is itself a linked list made 
   // of all the process wide linked lists.)
   // 
   errcode = pCleanupManager->FindProcess( pid, &pProcess, &context );
   
   fprintf( stderr, "Abnormal %d %d %p %p\n", pid, errcode, pCleanupManager, pProcess );

   if ( errcode != 0 )
   {
      // log it
      return;
   }

   /*
    * Loop on all sessions to clean them up. It is indeed possible to 
    * have no current session - a non-null errcode is not an error.
    */
   errcode = pProcess->GetFirstSession( &pCurrSession, &context );
   fprintf( stderr, "Errcode %d %p \n", errcode, pCurrSession );
   if ( errcode == 0 )
   {
      CleanSession( pCurrSession );

      errcode = pProcess->GetNextSession( pCurrSession, 
                                          &pNextSession,
                                          &context );
      while ( errcode == 0 )
      {
         CleanSession( pNextSession );
         pCurrSession = pNextSession;
         errcode = pProcess->GetNextSession( pCurrSession, 
                                             &pNextSession,
                                             &context );
      }
   }
   
   /*
    * Remove the CleanupProcess object (it will iteratively remove all 
    * its "children").
    */
   errcode = pCleanupManager->RemoveProcess( pProcess, &context );
   if ( errcode != 0 )
   {
      // log it
   }
#endif // #if 0
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int 
vdswHandler::Init( struct ConfigParams * pConfig,
                  vdseMemoryHeader   ** ppMemoryAddress )
{
   int errcode = 0;
   char path[PATH_MAX];
   int path_len;
   vdscMemoryFileStatus fileStatus;
   vdscMemoryFile memFile;

   if ( pConfig == NULL )
      return -1;
   m_pConfig = pConfig;

   m_pMemManager = new vdswMemoryManager();
   
   path_len = strlen( pConfig->wdLocation ) + strlen( VDS_DIR_SEPARATOR ) +
      strlen( VDS_MEMFILE_NAME )  + strlen( ".bak" );
   if ( path_len >= PATH_MAX )
      return -1;
   sprintf( path, "%s%s%s", pConfig->wdLocation, VDS_DIR_SEPARATOR,
            VDS_MEMFILE_NAME );
   
   vdscInitMemoryFile ( &memFile, pConfig->memorySizekb, path );
   vdscBackStoreStatus( &memFile, &fileStatus );

   if ( ! fileStatus.fileExist )
   {
      errcode = m_pMemManager->CreateVDS( path,
                                          pConfig->memorySizekb,
                                          pConfig->filePerms,
                                          ppMemoryAddress,
                                          &m_context );
      if ( errcode != 0 )
      {
         return -1;
      }
      
      (*ppMemoryAddress)->logON = false;
      if ( pConfig->logOn )
      {
         sprintf( path, "%s%s%s", pConfig->wdLocation, VDS_DIR_SEPARATOR,
                  VDS_LOGDIR_NAME );
         errcode = mkdir( path, pConfig->dirPerms );
         if ( errcode != 0 )
            return -1;
         (*ppMemoryAddress)->logON = true;
      }
   }
   else
   {
      if ( fileStatus.fileReadable && fileStatus.fileWritable && 
           fileStatus.lenghtOK )
         return -1;
      
      errcode = m_pMemManager->OpenVDS( path,
                                        pConfig->memorySizekb,
                                        ppMemoryAddress );
      
      errcode = ValidateVDS();
      if ( errcode != 0 )
         return -1;
      
      /*
       * We validate the VDS first since the config file can set/unset the  
       * logging of transactions on an existing VDS.
       */
      if ( pConfig->logOn )
      {
         /*
          * It is possible that logging was not on the last time the
          * program was run but is on now. Or that the directory was
          * removed for what ever reason. We recreate it if needed.
          */
         sprintf( path, "%s%s%s", pConfig->wdLocation, VDS_DIR_SEPARATOR,
                  VDS_LOGDIR_NAME );
#if HAVE_ACCESS
         errcode = access( path, F_OK );
#else
         FILE* fp = fopen( path, "r" );
         if ( fp == NULL )
            errcode = -1;
         else
            fclose( fp );        
#endif
         if ( errcode == -1 )
         {
            errcode = mkdir( path, pConfig->dirPerms );
            if ( errcode != 0 )
               return -1;
         }
         (*ppMemoryAddress)->logON = true;
      }
      else
         (*ppMemoryAddress)->logON = false;
      
   }

   m_pMemHeader = *ppMemoryAddress;
   
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int
vdswHandler::ValidateVDS()
{
   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

