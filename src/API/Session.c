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
#include <vdsf/vds_c.h>
#include "API/Session.h"
#include "API/Process.h"
#include "API/CommonObject.h"
#include "Engine/MemoryHeader.h"
#include "Engine/Process.h"
#include "Engine/Session.h"
#include "Engine/LogFile.h"
#include "Engine/Transaction.h"
#include "Engine/TreeNode.h"
#include "Engine/Folder.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsInitSession( VDS_HANDLE* sessionHandle )
{
   vdsErrors errcode;
   vdsaSession* pSession = NULL;
   void* ptr = NULL;
   
   if ( sessionHandle == NULL )
      return VDS_INVALID_HANDLE;
   
   *sessionHandle = NULL;

   if ( g_pProcessInstance == NULL )
      return VDS_PROCESS_NOT_INITIALIZED;
   
   pSession = (vdsaSession*) malloc(sizeof(vdsaSession));
   if ( pSession == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pSession, 0, sizeof(vdsaSession) );
   pSession->type = VDSA_SESSION;
   pSession->context.pLogFile = NULL;

   pSession->context.lockValue = getpid();
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

   pSession->context.pAllocator = GET_PTR( pSession->pHeader->allocatorOffset, 
                                           void );   
   if ( pSession->pHeader->logON )
   {
      ptr = malloc( sizeof(vdseLogFile) );
      if ( ptr == NULL )
         goto error_handler;
   
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
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsExitSession( VDS_HANDLE sessionHandle )
{
   vdsaSession* pSession;
//   vdseObjectContext* pObject = NULL;
//   vdsaCommonObject* pCommonObject = NULL;
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
         errcode = vdsaCloseSession( pSession );
         
         /*
          * vdsaCloseSession can be called by the api process object while
          * it holds the lock to the vdesProcess object. And the next
          * call locks it as well. To avoid a recursive lock (leading to 
          * a deadlock) we cannot include this call in vdsaCloseSession.
          */
         if ( errcode == 0 )
            vdseProcessRemoveSession( g_pProcessInstance->pCleanup, 
                                      pSession->pCleanup, 
                                      &pSession->context );

      }
      
      vdsaProcessUnlock();
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode == 0 && pSession->numberOfObjects == 0 )
      free( pSession );

   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
int vdsCreateObject( VDS_HANDLE      sessionHandle,
                     const char    * objectName,
                     vdsObjectType   objectType )
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
      {
         rc = vdseTopFolderCreateObject( GET_PTR( pSession->pHeader->treeMgrOffset, 
                                                  vdseFolder ),
                                         objectName, 
                                         objectType,
                                         &pSession->context );
         if ( rc != 0 )
            rc = vdscGetLastError( &pSession->context.errorHandler );
      }
      else
         rc = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }

   return rc;
}   
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsDestroyObject( VDS_HANDLE  sessionHandle,
                      const char* objectName )
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
      {
         rc = vdseTopFolderDestroyObject( GET_PTR( pSession->pHeader->treeMgrOffset, 
                                                   vdseFolder ),
                                          objectName, 
                                          &pSession->context );
         if ( rc != 0 )
            rc = vdscGetLastError( &pSession->context.errorHandler );
      }
      else
         rc = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   
   return rc;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsCommit( VDS_HANDLE sessionHandle )
{
   int rc = VDS_SESSION_CANNOT_GET_LOCK;
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
         if ( rc != 0 )
            rc = vdscGetLastError( &pSession->context.errorHandler );
      }
      else
         rc = VDS_SESSION_IS_TERMINATED;
         
      vdsaSessionUnlock( pSession );
   }
   
   return rc;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
   
int vdsRollback( VDS_HANDLE sessionHandle )
{
   int rc = VDS_SESSION_CANNOT_GET_LOCK;
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
         rc = 0;
         vdseTxRollback( (vdseTx*)pSession->context.pTransaction, 
                         &pSession->context );
      //if ( rc != 0 )
      //   rc = vdscGetLastError( &pSession->context.errorHandler );
      }
      else
         rc = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaCloseSession( vdsaSession* pSession )
{
   vdseObjectContext* pObject = NULL;
   vdsaCommonObject* pCommonObject = NULL;
   int errcode, rc = 0;
   
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
   
         errcode = vdseLock( &pSession->pCleanup->memObject, 
                             &pSession->context);
         if ( errcode == 0 )
         {
            while ( true )
            {
               errcode = vdseSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
               if ( errcode != 0 ) break;

               /* This would be an internal error... */
               if ( pObject == NULL ) continue;
            
               if ( pObject->pCommonObject == NULL ) continue;
            
               pCommonObject = (vdsaCommonObject*) pObject->pCommonObject;
            
               errcode = vdseTopFolderCloseObject( pCommonObject->pDesc, 
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
         rc = VDS_SESSION_IS_TERMINATED;

      vdsaSessionUnlock( pSession );

      return rc;
   }
   return VDS_SESSION_CANNOT_GET_LOCK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */


int vdsaSessionOpenObj( vdsaSession             * pSession,
                        enum vdsObjectType        objectType, 
                        const char              * objectName,
                        struct vdsaCommonObject * pObject )
{
   int errcode;
   vdseObjectDescriptor * pDescriptor;
   
   if ( ! pSession->terminated )
   {
      errcode = vdseTopFolderOpenObject( 
         GET_PTR( pSession->pHeader->treeMgrOffset, vdseFolder ),
         objectName, 
         &pDescriptor,
         &pSession->context );
      if ( errcode != 0 )
         errcode = vdscGetLastError( &pSession->context.errorHandler );
   }
   else
      errcode = VDS_SESSION_IS_TERMINATED;

   if ( errcode == 0 )
      pObject->pDesc = pDescriptor;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaSessionCloseObj( vdsaSession             * pSession,
                         struct vdsaCommonObject * pObject )
{
   int errcode;
   
   if ( ! pSession->terminated )
   {
      errcode = vdseTopFolderCloseObject( pObject->pDesc,
                                          &pSession->context );
      if ( errcode != 0 )
         errcode = vdscGetLastError( &pSession->context.errorHandler );
   }
   else
      errcode = VDS_SESSION_IS_TERMINATED;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

