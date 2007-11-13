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

#include "Common/Common.h"
#include <vdsf/vdsSession.h>
#include "API/Session.h"
#include "API/Process.h"
#include "API/CommonObject.h"
#include "Engine/MemoryHeader.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Process.h"
#include "Engine/Session.h"
#include "Engine/LogFile.h"
#include "Engine/Transaction.h"
#include "Engine/TreeNode.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsCommit( VDS_HANDLE sessionHandle )
{
   int rc = 0, errcode = 0;
   vdsaSession* pSession;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         rc = vdseTxCommit( (vdseTx*)pSession->context.pTransaction, 
                            &pSession->context );
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;
         
      vdsaSessionUnlock( pSession );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
int vdsCreateObject( VDS_HANDLE      sessionHandle,
                     const char    * objectName,
                     size_t          nameLengthInBytes,
                     vdsObjectType   objectType )
{
   vdsaSession* pSession;
   int rc = 0, errcode = 0;
   vdseFolder * pTree;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( objectType <= 0 || objectType >= VDS_LAST_OBJECT_TYPE )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_WRONG_OBJECT_TYPE );
      return VDS_WRONG_OBJECT_TYPE;
   }

   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, vdseFolder )
         rc = vdseTopFolderCreateObject( pTree,
                                         objectName,
                                         nameLengthInBytes,
                                         objectType,
                                         &pSession->context );
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

   return errcode;
}   
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsDestroyObject( VDS_HANDLE   sessionHandle,
                      const char * objectName,
                      size_t       nameLengthInBytes )
{
   vdsaSession* pSession;
   int rc = 0, errcode = 0;
   vdseFolder * pTree;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( objectName == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, vdseFolder )
         rc = vdseTopFolderDestroyObject( pTree,
                                          objectName,
                                          nameLengthInBytes,
                                          &pSession->context );
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

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
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( message == NULL )
      return VDS_NULL_POINTER;
   
   if ( msgLengthInBytes < 32 )
      return VDS_INVALID_LENGTH;
   
   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         if ( vdscGetLastError( &pSession->context.errorHandler ) != 0 )
           len = vdscGetErrorMsg( &pSession->context.errorHandler,
                                  message, 
                                  msgLengthInBytes );
         else
            strcpy( message, "No Error!" );
      }
      else
         rc = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   else
      rc = VDS_SESSION_CANNOT_GET_LOCK;

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsExitSession( VDS_HANDLE sessionHandle )
{
   vdsaSession * pSession;
   vdseSession * pCleanup;
   
   int errcode = -1;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaProcessLock() == 0 )
   {
      /*
       * if the exit process was called at the "same time" the session
       * might have been "removed" by the process object.
       */
      if ( ! pSession->terminated )
      {
         /* ok we are still there */
         pCleanup = pSession->pCleanup;
         errcode = vdsaCloseSession( pSession );
         
         /*
          * vdsaCloseSession can be called by the api process object while
          * it holds the lock to the vdesProcess object. And the next
          * call locks it as well. To avoid a recursive lock (leading to 
          * a deadlock) we cannot include this call in vdsaCloseSession.
          */
         if ( errcode == 0 )
            vdseProcessRemoveSession( g_pProcessInstance->pCleanup, 
                                      pCleanup, 
                                      &pSession->context );

      }
      
      vdsaProcessUnlock();
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode == 0 && pSession->numberOfObjects == 0 )
      free( pSession );

   if ( errcode != 0 )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsGetInfo( VDS_HANDLE   sessionHandle,
                vdsInfo    * pInfo )
{
   vdsaSession* pSession;
   int rc = 0, errcode = 0;
   vdseMemAlloc * pAlloc;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;
   
   if ( pInfo == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   
   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         GET_PTR( pAlloc, pSession->pHeader->allocatorOffset, vdseMemAlloc )
         rc = vdseMemAllocStats( pAlloc, pInfo, &pSession->context );
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;
      
      vdsaSessionUnlock( pSession );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsGetStatus( VDS_HANDLE     sessionHandle,
                  const char   * objectName,
                  size_t         nameLengthInBytes,
                  vdsObjStatus * pStatus )
{
   vdsaSession* pSession;
   int rc = 0, errcode = 0;
   vdseFolder * pTree;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;
   
   if ( objectName == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( pStatus == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         GET_PTR( pTree, pSession->pHeader->treeMgrOffset, vdseFolder )
         rc = vdseTopFolderGetStatus( pTree, 
                                      objectName,
                                      nameLengthInBytes,
                                      pStatus,
                                      &pSession->context );
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsInitSession( VDS_HANDLE* sessionHandle )
{
   vdsErrors errcode;
   vdsaSession* pSession = NULL;
   void* ptr = NULL;
   
   if ( sessionHandle == NULL )
      return VDS_NULL_HANDLE;
   
   *sessionHandle = NULL;

   if ( g_pProcessInstance == NULL )
      return VDS_PROCESS_NOT_INITIALIZED;
   
   pSession = (vdsaSession*) malloc(sizeof(vdsaSession));
   if ( pSession == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pSession, 0, sizeof(vdsaSession) );
   pSession->type = VDSA_SESSION;
   pSession->context.pLogFile = NULL;

   pSession->context.pidLocker = getpid();
   vdscInitErrorHandler( &pSession->context.errorHandler );

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   if ( g_protectionIsNeeded )
   {
      errcode = vdscInitThreadLock( &pSession->mutex );
      if ( errcode != 0 )
      {
         errcode = VDS_NOT_ENOUGH_RESOURCES;
         goto error_handler;
      }
   }

   
   pSession->pHeader = g_pProcessInstance->pHeader;
   if ( pSession->pHeader == NULL )
   {
      errcode = VDS_PROCESS_NOT_INITIALIZED;
      goto error_handler;
   }

   GET_PTR( pSession->context.pAllocator, pSession->pHeader->allocatorOffset, void );
   if ( pSession->pHeader->logON )
   {
      ptr = malloc( sizeof(vdseLogFile) );
      if ( ptr == NULL )
      {
         errcode = VDS_NOT_ENOUGH_HEAP_MEMORY;
         goto error_handler;
      }

      pSession->context.pLogFile = (vdseLogFile*)ptr;
      errcode = vdseInitLogFile( 
         pSession->context.pLogFile, 
         g_pProcessInstance->logDirName,
         pSession,
         &pSession->context.errorHandler );
      if ( errcode != VDS_OK )
         goto error_handler;
   }
   
   /*
    * Once a session is added, it can be accessed by other threads
    * in the process (exiting, for example). So we must lock the
    * session, just in case.
    */
   if ( vdsaSessionLock( pSession ) == 0 )
   {
      errcode = vdseProcessAddSession( g_pProcessInstance->pCleanup, 
                                       pSession, 
                                       &pSession->pCleanup, 
                                       &pSession->context );
      vdsaSessionUnlock( pSession );
      if ( errcode != VDS_OK ) goto error_handler;
   }
   else
   {
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
    * vdseProcess object. If this call succeeded, we would not come
    * here - this session is unknown to the rest of the software and
    * we can safely clean ourselves without locks.
    */
   if ( pSession->context.pLogFile != NULL )
   {
      vdseCloseLogFile( pSession->context.pLogFile, &pSession->context.errorHandler );
      free( pSession->context.pLogFile );
      pSession->context.pLogFile = NULL;
   }

   vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLastError( VDS_HANDLE sessionHandle )
{
   vdsaSession* pSession;
   int rc = VDS_SESSION_CANNOT_GET_LOCK;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
         rc = vdscGetLastError( &pSession->context.errorHandler );
      else
         rc = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }

   return rc;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsRollback( VDS_HANDLE sessionHandle )
{
   int errcode = 0;
   vdsaSession* pSession;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;

   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;
   
   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         vdseTxRollback( (vdseTx*)pSession->context.pTransaction, 
                         &pSession->context );
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   else 
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaSessionCloseObj( vdsaSession             * pSession,
                         struct vdsaCommonObject * pObject )
{
   int errcode = 0, rc = 0;
   
   VDS_PRE_CONDITION( pSession   != NULL );
   VDS_PRE_CONDITION( pObject    != NULL );

   if ( ! pSession->terminated )
   {
      rc = vdseTopFolderCloseObject( &pObject->folderItem,
                                          &pSession->context );
   }
   else
      errcode = VDS_SESSION_IS_TERMINATED;

   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaCloseSession( vdsaSession* pSession )
{
   vdseObjectContext* pObject = NULL;
   vdsaCommonObject* pCommonObject = NULL;
   int rc, errcode = 0;
   
   VDS_PRE_CONDITION( pSession != NULL );

   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( ! pSession->terminated )
      {
         if ( pSession->context.pTransaction != NULL )
         {
            vdseTxRollback( (vdseTx*)(pSession->context.pTransaction), 
                            &pSession->context );
            pSession->context.pTransaction = NULL;
         }
   
         rc = vdseLock( &pSession->pCleanup->memObject, &pSession->context);
         if ( rc == 0 )
         {
            for (;;)
            {
               rc = vdseSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
               if ( rc != 0 ) break;

               /* This would be an internal error... */
               if ( pObject == NULL ) continue;
            
               if ( pObject->pCommonObject == NULL ) continue;
            
               pCommonObject = (vdsaCommonObject*) pObject->pCommonObject;
            
               rc = vdseTopFolderCloseObject( &pCommonObject->folderItem, 
                                              &pSession->context );
               vdsaCommonCloseObject( pCommonObject );

               vdseSessionRemoveFirst(pSession->pCleanup, &pSession->context );
            }
            vdseUnlock( &pSession->pCleanup->memObject, &pSession->context);
         }
      
         pSession->pCleanup = NULL;
         pSession->terminated = true;
        
      }
      else
         errcode = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   else 
      errcode = VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaSessionOpenObj( vdsaSession             * pSession,
                        enum vdsObjectType        objectType, 
                        const char              * objectName,
                        size_t                    nameLengthInBytes,
                        struct vdsaCommonObject * pObject )
{
   int errcode = 0, rc = 0;
   vdseFolder * pTree;
   
   VDS_PRE_CONDITION( pSession   != NULL );
   VDS_PRE_CONDITION( objectName != NULL );
   VDS_PRE_CONDITION( pObject    != NULL );
   VDS_PRE_CONDITION( objectType > 0 && objectType < VDS_LAST_OBJECT_TYPE );
   VDS_PRE_CONDITION( nameLengthInBytes > 0 );
   
   if ( ! pSession->terminated )
   {
      GET_PTR( pTree, pSession->pHeader->treeMgrOffset, vdseFolder );
      rc = vdseTopFolderOpenObject( pTree,
                                    objectName,
                                    nameLengthInBytes,
                                    &pObject->folderItem,
                                    &pSession->context );
   }
   else
      errcode = VDS_SESSION_IS_TERMINATED;

   if ( errcode != VDS_OK )
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

