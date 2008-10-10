/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

void psoaMapResetReader( void * map );

static inline
void psoaResetReaders( psoaSession * pSession )
{
   psoaReader * reader = NULL;
   
   if ( psoaListReadersPeakFirst( &pSession->listReaders, &reader ) ) {
      do {
         if ( reader->type == PSOA_MAP ) {
            psoaMapResetReader( reader->address );
         }
      } while (psoaListReadersPeakNext(&pSession->listReaders, reader, &reader) );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoCommit( PSO_HANDLE sessionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( pSession->numberOfEdits > 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, 
         PSO_NOT_ALL_EDIT_ARE_CLOSED );
      return PSO_NOT_ALL_EDIT_ARE_CLOSED;
   }
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         psonTxCommit( (psonTx*)pSession->context.pTransaction, 
                            &pSession->context );
         psoaResetReaders( pSession );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
int psoCreateObject( PSO_HANDLE            sessionHandle,
                     const char          * objectName,
                     size_t                nameLengthInBytes,
                     psoObjectDefinition * pDefinition )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   errcode = psoaValidateDefinition( pDefinition );
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
      return errcode;
   }
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
         ok = psonTopFolderCreateObject( pTree,
                                         objectName,
                                         nameLengthInBytes,
                                         pDefinition,
                                         &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoDestroyObject( PSO_HANDLE   sessionHandle,
                      const char * objectName,
                      size_t       nameLengthInBytes )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
         ok = psonTopFolderDestroyObject( pTree,
                                          objectName,
                                          nameLengthInBytes,
                                          &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoErrorMsg( PSO_HANDLE sessionHandle,
                 char *     message,
                 size_t     msgLengthInBytes )
{
   psoaSession* pSession;
   int rc = PSO_OK;
   size_t len;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( message == NULL ) return PSO_NULL_POINTER;
   
   if ( msgLengthInBytes < 32 ) return PSO_INVALID_LENGTH;
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         if ( psocGetLastError( &pSession->context.errorHandler ) != 0 ) {
            len = psocGetErrorMsg( &pSession->context.errorHandler,
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
      psoaSessionUnlock( pSession );
   }
   else {
      rc = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoExitSession( PSO_HANDLE sessionHandle )
{
   psoaSession * pSession;
   psonSession * pCleanup;
   bool ok;
   int errcode = 0;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( psoaProcessLock() ) {
      /*
       * if the exit process was called at the "same time" the session
       * might have been "removed" by the process object.
       */
      if ( ! pSession->terminated ) {
         /* ok we are still there */
         pCleanup = pSession->pCleanup;
         errcode = psoaCloseSession( pSession );
         
         /*
          * psoaCloseSession can be called by the api process object while
          * it holds the lock to the vdesProcess object. And the next
          * call locks it as well. To avoid a recursive lock (leading to 
          * a deadlock) we cannot include this call in psoaCloseSession.
          */
         if ( errcode == 0 ) {
            ok = psonProcessRemoveSession( g_pProcessInstance->pCleanup, 
                                           pCleanup, 
                                           &pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
          }
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psoaProcessUnlock();
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
      psoaListReadersFini( &pSession->listReaders );
      free( pSession );
   }
   
   if ( errcode != 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetDefinition( PSO_HANDLE             sessionHandle,
                      const char           * objectName,
                      size_t                 nameLengthInBytes,
                      psoObjectDefinition ** ppDefinition )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   psonFolder * pTree;
   psonFieldDef * pInternalDef;
   psoObjectDefinition *pDefinition;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ppDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   pDefinition = calloc( sizeof(psoObjectDefinition), 1 );
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
         ok = psonTopFolderGetDef( pTree, 
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
               errcode = psoaGetDefinition( pInternalDef,
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
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetInfo( PSO_HANDLE   sessionHandle,
                psoInfo    * pInfo )
{
   psoaSession* pSession;
   int errcode = PSO_OK;
   bool ok = true;
   psonMemAlloc * pAlloc;
   struct psonMemoryHeader * pHead = g_pProcessInstance->pHeader;
   
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( pInfo == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   memset( pInfo, 0, sizeof(struct psoInfo) );
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pAlloc, pSession->pHeader->allocatorOffset, psonMemAlloc )
         ok = psonMemAllocStats( pAlloc, pInfo, &pSession->context );
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
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoGetStatus( PSO_HANDLE     sessionHandle,
                  const char   * objectName,
                  size_t         nameLengthInBytes,
                  psoObjStatus * pStatus )
{
   psoaSession* pSession;
   int errcode = 0;
   psonFolder * pTree;
   bool ok = true;
   
   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pStatus == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder )
         ok = psonTopFolderGetStatus( pTree, 
                                      objectName,
                                      nameLengthInBytes,
                                      pStatus,
                                      &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoInitSession( PSO_HANDLE * sessionHandle )
{
   psoErrors errcode = PSO_OK;
   psoaSession* pSession = NULL;
   void* ptr = NULL;
   bool ok;
   
   if ( sessionHandle == NULL ) return PSO_NULL_HANDLE;
   
   *sessionHandle = NULL;

   if ( g_pProcessInstance == NULL ) return PSO_PROCESS_NOT_INITIALIZED;
   
   pSession = (psoaSession*) malloc(sizeof(psoaSession));
   if ( pSession == NULL ) return PSO_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pSession, 0, sizeof(psoaSession) );
   pSession->type = PSOA_SESSION;
   pSession->context.pLogFile = NULL;

   pSession->context.pidLocker = getpid();
   psocInitErrorHandler( &pSession->context.errorHandler );

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   if ( g_protectionIsNeeded ) {
      ok = psocInitThreadLock( &pSession->mutex );
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
      ptr = malloc( sizeof(psonLogFile) );
      if ( ptr == NULL ) {
         errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
         goto error_handler;
      }

      pSession->context.pLogFile = (psonLogFile*)ptr;
      errcode = psonInitLogFile( 
         pSession->context.pLogFile, 
         g_pProcessInstance->logDirName,
         pSession,
         &pSession->context.errorHandler );
      if ( errcode != PSO_OK ) goto error_handler;
   }
   
   psoaListReadersInit( &pSession->listReaders );
   
   /*
    * Once a session is added, it can be accessed by other threads
    * in the process (exiting, for example). So we must lock the
    * session, just in case.
    */
   if ( psoaSessionLock( pSession ) ) {
      ok = psonProcessAddSession( g_pProcessInstance->pCleanup, 
                                  pSession, 
                                  &pSession->pCleanup, 
                                  &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      psoaSessionUnlock( pSession );
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
    * psonProcess object. If this call succeeded, we would not come
    * here - this session is unknown to the rest of the software and
    * we can safely clean ourselves without locks.
    */
   if ( pSession->context.pLogFile != NULL ) {
      psonCloseLogFile( pSession->context.pLogFile, &pSession->context.errorHandler );
      free( pSession->context.pLogFile );
      pSession->context.pLogFile = NULL;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLastError( PSO_HANDLE sessionHandle )
{
   psoaSession* pSession;
   int rc = PSO_SESSION_CANNOT_GET_LOCK;
   
   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         rc = psocGetLastError( &pSession->context.errorHandler );
      }
      else {
         rc = PSO_SESSION_IS_TERMINATED;
      }
      psoaSessionUnlock( pSession );
   }

   return rc;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoRollback( PSO_HANDLE sessionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;

   pSession = (psoaSession*) sessionHandle;

   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;
   
   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         psonTxRollback( (psonTx*)pSession->context.pTransaction, 
                         &pSession->context );
         psoaResetReaders( pSession );
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaSessionCloseObj( psoaSession             * pSession,
                        struct psoaCommonObject * pObject )
{
   int errcode = PSO_OK;
   bool ok = true;
   
   PSO_PRE_CONDITION( pSession   != NULL );
   PSO_PRE_CONDITION( pObject    != NULL );

   if ( ! pSession->terminated ) {
      ok = psonSessionRemoveObj( pSession->pCleanup, 
                                 pObject->pObjectContext, 
                                 &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         ok = psonTopFolderCloseObject( &pObject->folderItem,
                                        &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) pSession->numberOfObjects--;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaCloseSession( psoaSession * pSession )
{
   psonObjectContext* pObject = NULL;
   psoaCommonObject* pCommonObject = NULL;
   int rc, errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pSession != NULL );

   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         if ( pSession->context.pTransaction != NULL ) {
            psonTxRollback( (psonTx*)(pSession->context.pTransaction), 
                            &pSession->context );
            pSession->context.pTransaction = NULL;
         }
   
         if ( psonLock( &pSession->pCleanup->memObject, &pSession->context) ) {
            for (;;) {
               ok = psonSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
               PSO_POST_CONDITION( ok == true || ok == false );
               if ( ! ok ) break;

               /* This would be an internal error... */
               if ( pObject == NULL ) continue;
            
               if ( pObject->pCommonObject == NULL ) continue;
            
               pCommonObject = (psoaCommonObject*) pObject->pCommonObject;
               rc = psonTopFolderCloseObject( &pCommonObject->folderItem, 
                                              &pSession->context );
               psoaCommonCloseObject( pCommonObject );

               ok = psonSessionRemoveFirst(pSession->pCleanup, &pSession->context );
               PSO_POST_CONDITION( ok == true );
            }
            psonUnlock( &pSession->pCleanup->memObject, &pSession->context);
         }
      
         pSession->pCleanup = NULL;
         pSession->terminated = true;
        
      }
      else {
         errcode = PSO_SESSION_IS_TERMINATED;
      }
      
      psoaSessionUnlock( pSession );
   }
   else {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaSessionOpenObj( psoaSession             * pSession,
                       enum psoObjectType       objectType,
                       psoaEditMode              editMode,
                       const char             * objectName,
                       size_t                   nameLengthInBytes,
                       struct psoaCommonObject * pObject )
{
   int errcode = PSO_OK;
   psonFolder * pTree;
   psonObjectDescriptor * pDesc;
   bool ok = true;
   
   PSO_PRE_CONDITION( pSession   != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pObject    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );
   PSO_PRE_CONDITION( nameLengthInBytes > 0 );
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psonFolder );
      if ( editMode == PSOA_UPDATE_RO ) {
         ok = psonTopFolderEditObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      else {
         ok = psonTopFolderOpenObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      if ( ok ) {
         GET_PTR( pDesc, pObject->folderItem.pHashItem->dataOffset,
                          psonObjectDescriptor );

         ok = psonSessionAddObj( pSession->pCleanup,
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
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

