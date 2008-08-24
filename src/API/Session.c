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

#include "Common/Common.h"
#include <photon/psoSession.h>
#include "API/Session.h"
#include "API/Process.h"
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "Nucleus/MemoryHeader.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Process.h"
#include "Nucleus/Session.h"
#include "Nucleus/LogFile.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/TreeNode.h"
#include "Nucleus/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psaMapResetReader( void * map );

static inline
void psaResetReaders( psaSession * pSession )
{
   psaReader * reader = NULL;
   
   if ( psaListReadersPeakFirst( &pSession->listReaders, &reader ) ) {
      do {
         if ( reader->type == PSA_MAP ) {
            psaMapResetReader( reader->address );
         }
      } while (psaListReadersPeakNext(&pSession->listReaders, reader, &reader) );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoCommit( PSO_HANDLE sessionHandle )
{
   int errcode = PSO_OK;
   psaSession* pSession;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( pSession->numberOfEdits > 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, 
         PSO_NOT_ALL_EDIT_ARE_CLOSED );
      return PSO_NOT_ALL_EDIT_ARE_CLOSED;
   }
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         psnTxCommit( (psnTx*)pSession->context.pTransaction, 
                            &pSession->context );
         psaResetReaders( pSession );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
int psoCreateObject( PSO_HANDLE            sessionHandle,
                     const char          * objectName,
                     size_t                nameLengthInBytes,
                     psoObjectDefinition * pDefinition )
{
   psaSession* pSession;
   int errcode = PSO_OK;
   psnFolder * pTree;
   bool ok = true;
   
   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   errcode = psaValidateDefinition( pDefinition );
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
      return errcode;
   }
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderCreateObject( pTree,
                                         objectName,
                                         nameLengthInBytes,
                                         pDefinition,
                                         &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoDestroyObject( PSO_HANDLE   sessionHandle,
                      const char * objectName,
                      size_t       nameLengthInBytes )
{
   psaSession* pSession;
   int errcode = PSO_OK;
   psnFolder * pTree;
   bool ok = true;
   
   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderDestroyObject( pTree,
                                          objectName,
                                          nameLengthInBytes,
                                          &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoErrorMsg( PSO_HANDLE sessionHandle,
                 char *     message,
                 size_t     msgLengthInBytes )
{
   psaSession* pSession;
   int rc = PSO_OK;
   size_t len;
   
   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( message == NULL ) return PSO_NULL_POINTER;
   
   if ( msgLengthInBytes < 32 ) return PSO_INVALID_LENGTH;
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         if ( pscGetLastError( &pSession->context.errorHandler ) != 0 ) {
            len = pscGetErrorMsg( &pSession->context.errorHandler,
                                   message, 
                                   msgLengthInBytes );
         }
         else {
            strcpy( message, "No Error!" );
         }
      }
      else {
         rc = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      rc = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoExitSession( PSO_HANDLE sessionHandle )
{
   psaSession * pSession;
   psnSession * pCleanup;
   bool ok;
   int errcode = 0;
   
   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( psaProcessLock() ) {
      /*
       * if the exit process was called at the "same time" the session
       * might have been "removed" by the process object.
       */
      if ( ! pSession->terminated ) {
         /* ok we are still there */
         pCleanup = pSession->pCleanup;
         errcode = psaCloseSession( pSession );
         
         /*
          * psaCloseSession can be called by the api process object while
          * it holds the lock to the vdesProcess object. And the next
          * call locks it as well. To avoid a recursive lock (leading to 
          * a deadlock) we cannot include this call in psaCloseSession.
          */
         if ( errcode == 0 ) {
            ok = psnProcessRemoveSession( g_pProcessInstance->pCleanup, 
                                           pCleanup, 
                                           &pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
          }
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psaProcessUnlock();
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   /* 
    * This is a likely memory leak. But if we remove the session and there
    * are still objects open... we might have random crashes instead
    * (depending on what happen to the release memory - still attached to
    * the process or not - which is something that depends on the OS and
    * possibly other factors). 
    */
   if ( errcode == 0 && pSession->numberOfObjects == 0 ) {
      /* 
       * If someone tries to use the session handle after closing the
       * session, setting the type to zero is safer (if the free memory
       * is still attached to the process - which is often the case in
       * Unix to avoid going to the system for every malloc/free).
       */
      pSession->type = 0;
      /* Reset the list to spot logic errors (illegal accesses) with DBC */
      psaListReadersFini( &pSession->listReaders );
      free( pSession );
   }
   
   if ( errcode != 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetDefinition( PSO_HANDLE             sessionHandle,
                      const char           * objectName,
                      size_t                 nameLengthInBytes,
                      psoObjectDefinition ** ppDefinition )
{
   psaSession* pSession;
   int errcode = PSO_OK;
   psnFolder * pTree;
   psnFieldDef * pInternalDef;
   psoObjectDefinition *pDefinition;
   bool ok = true;
   
   pSession = (psaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ppDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   pDefinition = calloc( sizeof(psoObjectDefinition), 1 );
   if ( pDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderGetDef( pTree, 
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition,
                                   &pInternalDef,
                                   &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            if ( pDefinition->type == PSO_FOLDER ) {
               *ppDefinition = pDefinition;
            }
            else {
               errcode = psaGetDefinition( pInternalDef,
                                            (uint16_t)pDefinition->numFields,
                                            ppDefinition );
               if ( errcode == 0 ) {
                  (*ppDefinition)->type = pDefinition->type;
                  if ( pDefinition->type == PSO_HASH_MAP || 
                     pDefinition->type == PSO_FAST_MAP ) {
                     memcpy( &(*ppDefinition)->key, 
                             &pDefinition->key, 
                             sizeof(psoKeyDefinition) );
                  }
               }
            }
         }
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetInfo( PSO_HANDLE   sessionHandle,
                psoInfo    * pInfo )
{
   psaSession* pSession;
   int errcode = PSO_OK;
   bool ok = true;
   psnMemAlloc * pAlloc;
   struct psnMemoryHeader * pHead = g_pProcessInstance->pHeader;
   
   pSession = (psaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( pInfo == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   memset( pInfo, 0, sizeof(struct psoInfo) );
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pAlloc, pSession->pHeader->allocatorOffset, psnMemAlloc )
         ok = psnMemAllocStats( pAlloc, pInfo, &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            pInfo->memoryVersion = pHead->version;
            pInfo->bigEndian = 0;
            if (pHead->bigEndian) pInfo->bigEndian = 1;
            memcpy( pInfo->compiler,        pHead->compiler,        20 );
            memcpy( pInfo->compilerVersion, pHead->compilerVersion, 10 );
            memcpy( pInfo->platform,        pHead->cpu_type ,       20 );
            memcpy( pInfo->watchdogVersion, pHead->watchdogVersion, 10 );
            memcpy( pInfo->creationTime,    pHead->creationTime,    30 );
            strncpy( pInfo->dllVersion, PACKAGE_VERSION, 10 );
         }
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetStatus( PSO_HANDLE     sessionHandle,
                  const char   * objectName,
                  size_t         nameLengthInBytes,
                  psoObjStatus * pStatus )
{
   psaSession* pSession;
   int errcode = 0;
   psnFolder * pTree;
   bool ok = true;
   
   pSession = (psaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pStatus == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderGetStatus( pTree, 
                                      objectName,
                                      nameLengthInBytes,
                                      pStatus,
                                      &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoInitSession( PSO_HANDLE* sessionHandle )
{
   psoErrors errcode = PSO_OK;
   psaSession* pSession = NULL;
   void* ptr = NULL;
   bool ok;
   
   if ( sessionHandle == NULL ) return PSO_NULL_HANDLE;
   
   *sessionHandle = NULL;

   if ( g_pProcessInstance == NULL ) return PSO_PROCESS_NOT_INITIALIZED;
   
   pSession = (psaSession*) malloc(sizeof(psaSession));
   if ( pSession == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pSession, 0, sizeof(psaSession) );
   pSession->type = PSA_SESSION;
   pSession->context.pLogFile = NULL;

   pSession->context.pidLocker = getpid();
   pscInitErrorHandler( &pSession->context.errorHandler );

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   if ( g_protectionIsNeeded ) {
      ok = pscInitThreadLock( &pSession->mutex );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         errcode = PSO_NOT_ENOUGH_RESOURCES;
         goto error_handler;
      }
   }
   
   pSession->pHeader = g_pProcessInstance->pHeader;
   if ( pSession->pHeader == NULL ) {
      errcode = PSO_PROCESS_NOT_INITIALIZED;
      goto error_handler;
   }

   GET_PTR( pSession->context.pAllocator, pSession->pHeader->allocatorOffset, void );
   if ( pSession->pHeader->logON ) {
      ptr = malloc( sizeof(psnLogFile) );
      if ( ptr == NULL ) {
         errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
         goto error_handler;
      }

      pSession->context.pLogFile = (psnLogFile*)ptr;
      errcode = psnInitLogFile( 
         pSession->context.pLogFile, 
         g_pProcessInstance->logDirName,
         pSession,
         &pSession->context.errorHandler );
      if ( errcode != PSO_OK ) goto error_handler;
   }
   
   psaListReadersInit( &pSession->listReaders );
   
   /*
    * Once a session is added, it can be accessed by other threads
    * in the process (exiting, for example). So we must lock the
    * session, just in case.
    */
   if ( psaSessionLock( pSession ) ) {
      ok = psnProcessAddSession( g_pProcessInstance->pCleanup, 
                                  pSession, 
                                  &pSession->pCleanup, 
                                  &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      psaSessionUnlock( pSession );
      if ( ! ok ) goto error_handler;
   }
   else {
      /* 
       * A lock failure should be impossible since the session is
       * unknown to other and cannot be locked.
       */
      errcode = PSO_INTERNAL_ERROR;
      goto error_handler;
   }
   
   pSession->terminated = false;
   *sessionHandle = (PSO_HANDLE) pSession;

   return PSO_OK;
   
   /* Error processing... */

error_handler:

   /* 
    * The last operation that might fail is to add ourselves to the
    * psnProcess object. If this call succeeded, we would not come
    * here - this session is unknown to the rest of the software and
    * we can safely clean ourselves without locks.
    */
   if ( pSession->context.pLogFile != NULL ) {
      psnCloseLogFile( pSession->context.pLogFile, &pSession->context.errorHandler );
      free( pSession->context.pLogFile );
      pSession->context.pLogFile = NULL;
   }

   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLastError( PSO_HANDLE sessionHandle )
{
   psaSession* pSession;
   int rc = PSO_SESSION_CANNOT_GET_LOCK;
   
   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         rc = pscGetLastError( &pSession->context.errorHandler );
      }
      else {
         rc = PSO_SESSION_IS_TERMINATED;
      }
      psaSessionUnlock( pSession );
   }

   return rc;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoRollback( PSO_HANDLE sessionHandle )
{
   int errcode = PSO_OK;
   psaSession* pSession;

   pSession = (psaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         psnTxRollback( (psnTx*)pSession->context.pTransaction, 
                         &pSession->context );
         psaResetReaders( pSession );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaSessionCloseObj( psaSession             * pSession,
                        struct psaCommonObject * pObject )
{
   int errcode = PSO_OK;
   bool ok = true;
   
   PSO_PRE_CONDITION( pSession   != NULL );
   PSO_PRE_CONDITION( pObject    != NULL );

   if ( ! pSession->terminated ) {
      ok = psnSessionRemoveObj( pSession->pCleanup, 
                                 pObject->pObjectContext, 
                                 &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         ok = psnTopFolderCloseObject( &pObject->folderItem,
                                        &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) pSession->numberOfObjects--;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaCloseSession( psaSession* pSession )
{
   psnObjectContext* pObject = NULL;
   psaCommonObject* pCommonObject = NULL;
   int rc, errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pSession != NULL );

   if ( psaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         if ( pSession->context.pTransaction != NULL ) {
            psnTxRollback( (psnTx*)(pSession->context.pTransaction), 
                            &pSession->context );
            pSession->context.pTransaction = NULL;
         }
   
         if ( psnLock( &pSession->pCleanup->memObject, &pSession->context) ) {
            for (;;) {
               ok = psnSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
               PSO_POST_CONDITION( ok == true || ok == false );
               if ( ! ok ) break;

               /* This would be an internal error... */
               if ( pObject == NULL ) continue;
            
               if ( pObject->pCommonObject == NULL ) continue;
            
               pCommonObject = (psaCommonObject*) pObject->pCommonObject;
               rc = psnTopFolderCloseObject( &pCommonObject->folderItem, 
                                              &pSession->context );
               psaCommonCloseObject( pCommonObject );

               ok = psnSessionRemoveFirst(pSession->pCleanup, &pSession->context );
               PSO_POST_CONDITION( ok == true );
            }
            psnUnlock( &pSession->pCleanup->memObject, &pSession->context);
         }
      
         pSession->pCleanup = NULL;
         pSession->terminated = true;
        
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaSessionOpenObj( psaSession             * pSession,
                       enum psoObjectType       objectType,
                       psaEditMode              editMode,
                       const char             * objectName,
                       size_t                   nameLengthInBytes,
                       struct psaCommonObject * pObject )
{
   int errcode = PSO_OK;
   psnFolder * pTree;
   psnObjectDescriptor * pDesc;
   bool ok = true;
   
   PSO_PRE_CONDITION( pSession   != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pObject    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );
   PSO_PRE_CONDITION( nameLengthInBytes > 0 );
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder );
      if ( editMode == PSA_UPDATE_RO ) {
         ok = psnTopFolderEditObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         ok = psnTopFolderOpenObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      if ( ok ) {
         GET_PTR( pDesc, pObject->folderItem.pHashItem->dataOffset,
                          psnObjectDescriptor );

         ok = psnSessionAddObj( pSession->pCleanup,
                                 pDesc->offset,
                                 pDesc->apiType,
                                 pObject,
                                 &pObject->pObjectContext,
                                 &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         pSession->numberOfObjects++;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

