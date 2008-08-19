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

#include "Common/Common.h"
#include <vdsf/vdsSession.h>
#include "API/Session.h"
#include "API/Process.h"
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "Engine/MemoryHeader.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Process.h"
#include "Engine/Session.h"
#include "Engine/LogFile.h"
#include "Engine/Transaction.h"
#include "Engine/TreeNode.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsaMapResetReader( void * map );

static inline
void vdsaResetReaders( vdsaSession * pSession )
{
   vdsaReader * reader = NULL;
   
   if ( vdsaListReadersPeakFirst( &pSession->listReaders, &reader ) ) {
      do {
         if ( reader->type == VDSA_MAP ) {
            vdsaMapResetReader( reader->address );
         }
      } while (vdsaListReadersPeakNext(&pSession->listReaders, reader, &reader) );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsCommit( VDS_HANDLE sessionHandle )
{
   int errcode = VDS_OK;
   vdsaSession* pSession;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( pSession->numberOfEdits > 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, 
         VDS_NOT_ALL_EDIT_ARE_CLOSED );
      return VDS_NOT_ALL_EDIT_ARE_CLOSED;
   }
   
   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         psnTxCommit( (psnTx*)pSession->context.pTransaction, 
                            &pSession->context );
         vdsaResetReaders( pSession );
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
int vdsCreateObject( VDS_HANDLE            sessionHandle,
                     const char          * objectName,
                     size_t                nameLengthInBytes,
                     vdsObjectDefinition * pDefinition )
{
   vdsaSession* pSession;
   int errcode = VDS_OK;
   psnFolder * pTree;
   bool ok = true;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   errcode = vdsaValidateDefinition( pDefinition );
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
      return errcode;
   }
   
   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderCreateObject( pTree,
                                         objectName,
                                         nameLengthInBytes,
                                         pDefinition,
                                         &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsDestroyObject( VDS_HANDLE   sessionHandle,
                      const char * objectName,
                      size_t       nameLengthInBytes )
{
   vdsaSession* pSession;
   int errcode = VDS_OK;
   psnFolder * pTree;
   bool ok = true;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderDestroyObject( pTree,
                                          objectName,
                                          nameLengthInBytes,
                                          &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsErrorMsg( VDS_HANDLE sessionHandle,
                 char *     message,
                 size_t     msgLengthInBytes )
{
   vdsaSession* pSession;
   int rc = VDS_OK;
   size_t len;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( message == NULL ) return VDS_NULL_POINTER;
   
   if ( msgLengthInBytes < 32 ) return VDS_INVALID_LENGTH;
   
   if ( vdsaSessionLock( pSession ) ) {
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
         rc = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      rc = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsExitSession( VDS_HANDLE sessionHandle )
{
   vdsaSession * pSession;
   psnSession * pCleanup;
   bool ok;
   int errcode = 0;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaProcessLock() ) {
      /*
       * if the exit process was called at the "same time" the session
       * might have been "removed" by the process object.
       */
      if ( ! pSession->terminated ) {
         /* ok we are still there */
         pCleanup = pSession->pCleanup;
         errcode = vdsaCloseSession( pSession );
         
         /*
          * vdsaCloseSession can be called by the api process object while
          * it holds the lock to the vdesProcess object. And the next
          * call locks it as well. To avoid a recursive lock (leading to 
          * a deadlock) we cannot include this call in vdsaCloseSession.
          */
         if ( errcode == 0 ) {
            ok = psnProcessRemoveSession( g_pProcessInstance->pCleanup, 
                                           pCleanup, 
                                           &pSession->context );
            VDS_POST_CONDITION( ok == true || ok == false );
          }
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      
      vdsaProcessUnlock();
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
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
      vdsaListReadersFini( &pSession->listReaders );
      free( pSession );
   }
   
   if ( errcode != 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsGetDefinition( VDS_HANDLE             sessionHandle,
                      const char           * objectName,
                      size_t                 nameLengthInBytes,
                      vdsObjectDefinition ** ppDefinition )
{
   vdsaSession* pSession;
   int errcode = VDS_OK;
   psnFolder * pTree;
   psnFieldDef * pInternalDef;
   vdsObjectDefinition *pDefinition;
   bool ok = true;
   
   pSession = (vdsaSession*) sessionHandle;

   if ( pSession == NULL ) return VDS_NULL_HANDLE;   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( ppDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   pDefinition = calloc( sizeof(vdsObjectDefinition), 1 );
   if ( pDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderGetDef( pTree, 
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition,
                                   &pInternalDef,
                                   &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            if ( pDefinition->type == VDS_FOLDER ) {
               *ppDefinition = pDefinition;
            }
            else {
               errcode = vdsaGetDefinition( pInternalDef,
                                            (uint16_t)pDefinition->numFields,
                                            ppDefinition );
               if ( errcode == 0 ) {
                  (*ppDefinition)->type = pDefinition->type;
                  if ( pDefinition->type == VDS_HASH_MAP || 
                     pDefinition->type == VDS_FAST_MAP ) {
                     memcpy( &(*ppDefinition)->key, 
                             &pDefinition->key, 
                             sizeof(vdsKeyDefinition) );
                  }
               }
            }
         }
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsGetInfo( VDS_HANDLE   sessionHandle,
                vdsInfo    * pInfo )
{
   vdsaSession* pSession;
   int errcode = VDS_OK;
   bool ok = true;
   psnMemAlloc * pAlloc;
   struct psnMemoryHeader * pHead = g_pProcessInstance->pHeader;
   
   pSession = (vdsaSession*) sessionHandle;

   if ( pSession == NULL ) return VDS_NULL_HANDLE;   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;
   
   if ( pInfo == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   memset( pInfo, 0, sizeof(struct vdsInfo) );
   
   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pAlloc, pSession->pHeader->allocatorOffset, psnMemAlloc )
         ok = psnMemAllocStats( pAlloc, pInfo, &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
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
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsGetStatus( VDS_HANDLE     sessionHandle,
                  const char   * objectName,
                  size_t         nameLengthInBytes,
                  vdsObjStatus * pStatus )
{
   vdsaSession* pSession;
   int errcode = 0;
   psnFolder * pTree;
   bool ok = true;
   
   pSession = (vdsaSession*) sessionHandle;

   if ( pSession == NULL ) return VDS_NULL_HANDLE;   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( pStatus == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder )
         ok = psnTopFolderGetStatus( pTree, 
                                      objectName,
                                      nameLengthInBytes,
                                      pStatus,
                                      &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsInitSession( VDS_HANDLE* sessionHandle )
{
   vdsErrors errcode = VDS_OK;
   vdsaSession* pSession = NULL;
   void* ptr = NULL;
   bool ok;
   
   if ( sessionHandle == NULL ) return VDS_NULL_HANDLE;
   
   *sessionHandle = NULL;

   if ( g_pProcessInstance == NULL ) return VDS_PROCESS_NOT_INITIALIZED;
   
   pSession = (vdsaSession*) malloc(sizeof(vdsaSession));
   if ( pSession == NULL ) return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pSession, 0, sizeof(vdsaSession) );
   pSession->type = VDSA_SESSION;
   pSession->context.pLogFile = NULL;

   pSession->context.pidLocker = getpid();
   pscInitErrorHandler( &pSession->context.errorHandler );

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   if ( g_protectionIsNeeded ) {
      ok = pscInitThreadLock( &pSession->mutex );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) {
         errcode = VDS_NOT_ENOUGH_RESOURCES;
         goto error_handler;
      }
   }
   
   pSession->pHeader = g_pProcessInstance->pHeader;
   if ( pSession->pHeader == NULL ) {
      errcode = VDS_PROCESS_NOT_INITIALIZED;
      goto error_handler;
   }

   GET_PTR( pSession->context.pAllocator, pSession->pHeader->allocatorOffset, void );
   if ( pSession->pHeader->logON ) {
      ptr = malloc( sizeof(psnLogFile) );
      if ( ptr == NULL ) {
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto error_handler;
      }

      pSession->context.pLogFile = (psnLogFile*)ptr;
      errcode = psnInitLogFile( 
         pSession->context.pLogFile, 
         g_pProcessInstance->logDirName,
         pSession,
         &pSession->context.errorHandler );
      if ( errcode != VDS_OK ) goto error_handler;
   }
   
   vdsaListReadersInit( &pSession->listReaders );
   
   /*
    * Once a session is added, it can be accessed by other threads
    * in the process (exiting, for example). So we must lock the
    * session, just in case.
    */
   if ( vdsaSessionLock( pSession ) ) {
      ok = psnProcessAddSession( g_pProcessInstance->pCleanup, 
                                  pSession, 
                                  &pSession->pCleanup, 
                                  &pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      vdsaSessionUnlock( pSession );
      if ( ! ok ) goto error_handler;
   }
   else {
      /* 
       * A lock failure should be impossible since the session is
       * unknown to other and cannot be locked.
       */
      errcode = VDS_INTERNAL_ERROR;
      goto error_handler;
   }
   
   pSession->terminated = false;
   *sessionHandle = (VDS_HANDLE) pSession;

   return VDS_OK;
   
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

   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLastError( VDS_HANDLE sessionHandle )
{
   vdsaSession* pSession;
   int rc = VDS_SESSION_CANNOT_GET_LOCK;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         rc = pscGetLastError( &pSession->context.errorHandler );
      }
      else {
         rc = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }

   return rc;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsRollback( VDS_HANDLE sessionHandle )
{
   int errcode = VDS_OK;
   vdsaSession* pSession;

   pSession = (vdsaSession*) sessionHandle;

   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;
   
   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         psnTxRollback( (psnTx*)pSession->context.pTransaction, 
                         &pSession->context );
         vdsaResetReaders( pSession );
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaSessionCloseObj( vdsaSession             * pSession,
                         struct vdsaCommonObject * pObject )
{
   int errcode = VDS_OK;
   bool ok = true;
   
   VDS_PRE_CONDITION( pSession   != NULL );
   VDS_PRE_CONDITION( pObject    != NULL );

   if ( ! pSession->terminated ) {
      ok = psnSessionRemoveObj( pSession->pCleanup, 
                                 pObject->pObjectContext, 
                                 &pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         ok = psnTopFolderCloseObject( &pObject->folderItem,
                                        &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
         if ( ok ) pSession->numberOfObjects--;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaCloseSession( vdsaSession* pSession )
{
   psnObjectContext* pObject = NULL;
   vdsaCommonObject* pCommonObject = NULL;
   int rc, errcode = VDS_OK;
   bool ok;
   
   VDS_PRE_CONDITION( pSession != NULL );

   if ( vdsaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         if ( pSession->context.pTransaction != NULL ) {
            psnTxRollback( (psnTx*)(pSession->context.pTransaction), 
                            &pSession->context );
            pSession->context.pTransaction = NULL;
         }
   
         if ( psnLock( &pSession->pCleanup->memObject, &pSession->context) ) {
            for (;;) {
               ok = psnSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
               VDS_POST_CONDITION( ok == true || ok == false );
               if ( ! ok ) break;

               /* This would be an internal error... */
               if ( pObject == NULL ) continue;
            
               if ( pObject->pCommonObject == NULL ) continue;
            
               pCommonObject = (vdsaCommonObject*) pObject->pCommonObject;
               rc = psnTopFolderCloseObject( &pCommonObject->folderItem, 
                                              &pSession->context );
               vdsaCommonCloseObject( pCommonObject );

               ok = psnSessionRemoveFirst(pSession->pCleanup, &pSession->context );
               VDS_POST_CONDITION( ok == true );
            }
            psnUnlock( &pSession->pCleanup->memObject, &pSession->context);
         }
      
         pSession->pCleanup = NULL;
         pSession->terminated = true;
        
      }
      else {
         errcode = VDS_SESSION_IS_TERMINATED;
      }
      
      vdsaSessionUnlock( pSession );
   }
   else {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaSessionOpenObj( vdsaSession             * pSession,
                        enum vdsObjectType        objectType,
                        vdsaEditMode              editMode,
                        const char              * objectName,
                        size_t                    nameLengthInBytes,
                        struct vdsaCommonObject * pObject )
{
   int errcode = VDS_OK;
   psnFolder * pTree;
   psnObjectDescriptor * pDesc;
   bool ok = true;
   
   VDS_PRE_CONDITION( pSession   != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pObject    != NULL );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );
   VDS_PRE_CONDITION( nameLengthInBytes > 0 );
   
   if ( ! pSession->terminated ) {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, psnFolder );
      if ( editMode == VDSA_UPDATE_RO ) {
         ok = psnTopFolderEditObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
      }
      else {
         ok = psnTopFolderOpenObject( pTree,
                                       objectName,
                                       nameLengthInBytes,
                                       objectType,
                                       &pObject->folderItem,
                                       &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
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
         VDS_POST_CONDITION( ok == true || ok == false );
         pSession->numberOfObjects++;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

