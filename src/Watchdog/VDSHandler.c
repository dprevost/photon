/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Watchdog/VDSHandler.h"
#include "Watchdog/MemoryManager.h"
#include "Watchdog/Watchdog.h"
#include "Watchdog/VerifyVDS.h"
#include "API/WatchdogCommon.h"
#include "Engine/SessionContext.h"
#include "Engine/InitEngine.h"

extern vdscErrMsgHandle g_wdErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdswHandlerInit( vdswHandler         * pHandler,
                      struct ConfigParams * pConfig,
                      vdseMemoryHeader   ** ppMemoryAddress,
                      bool                  verifyVDSOnly )
{
   bool rc;
   int errcode = 0;
   char path[PATH_MAX];
   char logFile[PATH_MAX];
   int path_len;
   vdscMemoryFileStatus fileStatus;
   vdscMemoryFile memFile;
   FILE * fp = NULL;
   char timeBuf[30];
   time_t t;
   struct tm formattedTime;
   size_t numObjectsOK = 0;
   size_t numObjectsRepaired = 0;
   size_t numObjectsDeleted = 0;
   size_t numObjectsError = 0;

   VDS_PRE_CONDITION( pHandler        != NULL );
   VDS_PRE_CONDITION( pConfig         != NULL );
   VDS_PRE_CONDITION( ppMemoryAddress != NULL );

   pHandler->pMemHeader = NULL;

   memset( &pHandler->context, 0, sizeof(vdseSessionContext) );
   pHandler->context.pidLocker = getpid();
   
   errcode = vdseInitEngine();
   if ( errcode != 0 ) {
      fprintf( stderr, "Abnormal error at line %d in VdsHandler.cpp\n", __LINE__ );
      exit(1);
   }
   vdscInitErrorHandler( &pHandler->context.errorHandler );

   pHandler->pConfig = pConfig;

   pHandler->pMemManager = (vdswMemoryManager *)calloc( 
      sizeof(vdswMemoryManager), 1 );
   if ( pHandler->pMemManager == NULL ) {
      vdscSetError( &pHandler->context.errorHandler,
                    g_wdErrorHandle,
                    VDSW_NOT_ENOUGH_HEAP_MEMORY );
      return false;
   }
   vdswMemoryManagerInit( pHandler->pMemManager );
   
   path_len = strlen( pConfig->wdLocation ) + strlen( VDS_DIR_SEPARATOR ) +
      strlen( VDS_MEMFILE_NAME )  + strlen( ".bak" );
   if ( path_len >= PATH_MAX ) {
      vdscSetError( &pHandler->context.errorHandler,
                    g_wdErrorHandle,
                    VDSW_CFG_BCK_LOCATION_TOO_LONG );
      return false;
   }
      
   sprintf( path, "%s%s%s", pConfig->wdLocation, VDS_DIR_SEPARATOR,
            VDS_MEMFILE_NAME );
   
   vdscInitMemoryFile ( &memFile, pConfig->memorySizekb, path );
   vdscBackStoreStatus( &memFile, &fileStatus );

   if ( ! fileStatus.fileExist ) {
      if ( verifyVDSOnly ) {
         vdscSetError( &pHandler->context.errorHandler,
                       g_wdErrorHandle,
                       VDSW_NO_VERIFICATION_POSSIBLE );
         return false;
      }
      rc = vdswCreateVDS( pHandler->pMemManager,
                               path,
                               pConfig->memorySizekb,
                               pConfig->filePerms,
                               ppMemoryAddress,
                               &pHandler->context );
      VDS_POST_CONDITION( rc == true || rc == false );
      if ( ! rc ) return false;
      
      (*ppMemoryAddress)->logON = false;
      if ( pConfig->logOn ) {
         
         sprintf( path, "%s%s%s", pConfig->wdLocation, VDS_DIR_SEPARATOR,
                  VDS_LOGDIR_NAME );
         errcode = mkdir( path, pConfig->dirPerms );
<<<<<<< HEAD:src/Watchdog/VDSHandler.c
         if ( errcode != 0 ) {
            vdscSetError( &pHandler->context.errorHandler,
                          g_wdErrorHandle,
                          VDSW_MKDIR_FAILURE );
            return false;
         }
=======
         if ( errcode != 0 ) return VDSW_MKDIR_FAILURE;
>>>>>>> Work in progress - better error codes for the watchdog.:src/Watchdog/VDSHandler.c
         (*ppMemoryAddress)->logON = true;
      }
   }
   else {
      if ( ! fileStatus.fileReadable || ! fileStatus.fileWritable || 
         ! fileStatus.lenghtOK ) {
<<<<<<< HEAD:src/Watchdog/VDSHandler.c
         vdscSetError( &pHandler->context.errorHandler,
                       g_wdErrorHandle,
                       VDSW_FILE_NOT_ACCESSIBLE );
         return false;
=======
         return VDSW_FILE_NOT_ACCESSIBLE;
>>>>>>> Work in progress - better error codes for the watchdog.:src/Watchdog/VDSHandler.c
      }

      memset( logFile, '\0', PATH_MAX );
      memset( timeBuf, '\0', 30 );

      t = time(NULL);
      localtime_r( &t, &formattedTime );
      strftime( timeBuf, 30, "%Y_%m_%d_%H_%M_%S", &formattedTime );

      path_len = strlen( pConfig->wdLocation ) + strlen( VDS_DIR_SEPARATOR ) +
      strlen("Logs") + strlen( VDS_DIR_SEPARATOR ) +
      strlen("Verify_") + strlen(timeBuf) + strlen(".log");
      if ( path_len < PATH_MAX ) {
        sprintf( logFile, "%s%s%s", 
            pConfig->wdLocation, 
            VDS_DIR_SEPARATOR,
            "Logs" );
      
         errcode = mkdir( logFile, pConfig->dirPerms );
         if ( errcode != 0 ) {
            if ( errno != EEXIST ) {
               vdscSetError( &pHandler->context.errorHandler,
                             VDSC_ERRNO_HANDLE,
                             errno );
               vdscChainError( &pHandler->context.errorHandler,
                               g_wdErrorHandle,
                               VDSW_MKDIR_FAILURE );
               return false;
            }
         }
         sprintf( logFile, "%s%s%s%s%s%s%s", 
            pConfig->wdLocation, 
            VDS_DIR_SEPARATOR,
            "Logs",
            VDS_DIR_SEPARATOR,
            "Verify_",
            timeBuf,
            ".log" );
      
         fp = fopen( logFile, "w" );
      }
      if ( fp == NULL ) fp = stderr;

      if ( ! verifyVDSOnly ) {
         errcode = vdscCopyBackstore( &pHandler->pMemManager->memory,
                                      pConfig->filePerms,
                                      &pHandler->context.errorHandler );
         if ( errcode != 0 ) {
            vdscChainError( &pHandler->context.errorHandler,
                            g_wdErrorHandle,
                            VDSW_COPY_BCK_FAILURE );
            return false;
         }
      }
      
      rc = vdswOpenVDS( pHandler->pMemManager,
                        path,
                        pConfig->memorySizekb,
                        ppMemoryAddress,
                        &pHandler->context );
      VDS_POST_CONDITION( rc == true || rc == false );
      if ( ! rc ) return false;
      
      fprintf( stderr, "Starting the recovery of the VDS, please be patient\n" );
      if ( verifyVDSOnly ) {
         vdswVerify( *ppMemoryAddress, 
                     &numObjectsOK,
                     &numObjectsRepaired,
                     &numObjectsDeleted,
                     &numObjectsError,
                     fp );
         if ( fp != stderr ) fclose( fp );
         return (numObjectsError == 0);
      }
      vdswRepair( *ppMemoryAddress, 
                  &numObjectsOK,
                  &numObjectsRepaired,
                  &numObjectsDeleted,
                  &numObjectsError,
                  fp );
      if ( fp != stderr ) fclose( fp );

      fprintf( stderr, "Number of objects with no defects: "VDSF_SIZE_T_FORMAT"\n", numObjectsOK );
      fprintf( stderr, "Number of objects with small defects: "VDSF_SIZE_T_FORMAT"\n", numObjectsRepaired );
      fprintf( stderr, "Number of deleted objects (added but not committed, etc.): "VDSF_SIZE_T_FORMAT"\n", numObjectsDeleted );
      fprintf( stderr, "Number of objects with errors: "VDSF_SIZE_T_FORMAT"\n", numObjectsError );
      if ( numObjectsError != 0 ) {
         fprintf( stderr, "Failure repairing the vds (see log for details)\n" );
         fprintf( stderr, "Please save the backup (%s%s) \n%s\n",
            path, ".bak",
            "before running this program a second time." );
         return false;
      }

#if 0
      /*
       * We validate the VDS first since the config file can set/unset the  
       * logging of transactions on an existing VDS.
       */
      if ( pConfig->logOn ) {
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
         if ( fp == NULL ) {
            errcode = -1;
         }
         else {
            fclose( fp );
         }
#endif
         if ( errcode == -1 ) {
            errcode = mkdir( path, pConfig->dirPerms );
            if ( errcode != 0 )
               return false;
         }
         (*ppMemoryAddress)->logON = true;
      }
      else
#endif
         (*ppMemoryAddress)->logON = false;
      
   }

   pHandler->pMemHeader = *ppMemoryAddress;
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswHandlerFini( vdswHandler * pHandler )
{
   VDS_PRE_CONDITION( pHandler != NULL );

   pHandler->pConfig = NULL;
   pHandler->pMemManager = NULL;
   pHandler->pMemHeader = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

//void CleanSession( vdseSession * pSession )
//{
//   fprintf( stderr, "Session %p\n", pSession );

/*
- need to loop on all objects to see if we hold a lock somewhere
 */
//}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswHandleCrash( vdswHandler * pHandler, pid_t pid )
{
//   int errcode;
   VDS_PRE_CONDITION( pHandler != NULL );

#if 0   
   vdseProcess* pProcess = NULL;
   vdseSessionContext context;
   vdseSession* pFirstSession = NULL;
   vdseSession* pCurrSession  = NULL;
   vdseSession* pNextSession  = NULL;

   memset( &context, 0, sizeof context );
   context.lockValue = getpid();
   context.pAllocator = &pHandler->pMemHeader->allocator;
   
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

   if ( errcode != 0 ) {
      // log it
      return;
   }

   /*
    * Loop on all sessions to clean them up. It is indeed possible to 
    * have no current session - a non-null errcode is not an error.
    */
   errcode = pProcess->GetFirstSession( &pCurrSession, &context );
   fprintf( stderr, "Errcode %d %p \n", errcode, pCurrSession );
   if ( errcode == 0 ) {
      CleanSession( pCurrSession );

      errcode = pProcess->GetNextSession( pCurrSession, 
                                          &pNextSession,
                                          &context );
      while ( errcode == 0 ) {
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
   if ( errcode != 0 ) {
      // log it
   }
#endif // #if 0
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

