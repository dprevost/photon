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
#include "API/ProxyObject.h"
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
   
   if ( g_protectionIsNeeded )
   {
      errcode = vdscInitThreadLock( &pSession->mutex );
      if ( errcode != 0 )
      {
         free( pSession );
         return VDS_NOT_ENOUGH_RESOURCES;
      }
   }

   /*
    * From this point we can use "goto error_handler" to recover
    * from errors;
    */
   
   pSession->pHeader = g_pProcessInstance->pHeader;
   if ( pSession->pHeader == NULL )
   {
      errcode = VDS_PROCESS_NOT_INITIALIZED;
      goto error;
   }

   pSession->context.lockValue = getpid();
   pSession->context.pAllocator = GET_PTR( pSession->pHeader->allocatorOffset, 
                                           void );   
   if ( pSession->pHeader->logON )
   {
      ptr = malloc( sizeof(vdseLogFile) );
      if ( ptr == NULL )
         goto error;
   
      pSession->context.pLogFile = (vdseLogFile*)ptr;
      errcode = vdseInitLogFile( 
         pSession->context.pLogFile, 
         g_pProcessInstance->logDirName,
         pSession,
         &pSession->context.errorHandler );
      if ( errcode != VDS_OK )
         goto error;
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
      if ( errcode != VDS_OK ) goto error;
   }
   else
   {
      /* 
       * A lock failure should be impossible since the session is
       * unknown to other and cannot be locked.
       */
      errcode = VDS_INTERNAL_ERROR;
      goto error;
   }
   *sessionHandle = (VDS_HANDLE) pSession;

   return VDS_OK;
   
   /* Error processing... */

error:

   /* 
    * We lock ourselves, just in case. We can't rely on the value
    * of pSession->pCleanup (being null that is). The only way to
    * be able to do this safely without locks would be to use 
    * memory barriers (ref.: discussions on the net about the 
    * singleton pattern).
    */
   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( pSession->pCleanup != NULL )
      {
         vdseProcessRemoveSession( g_pProcessInstance->pCleanup,
                                   pSession->pCleanup,
                                   &pSession->context );
         pSession->pCleanup = NULL;
      }

      if ( pSession->context.pLogFile != NULL )
      {
         vdseCloseLogFile( pSession->context.pLogFile, &pSession->context.errorHandler );
         free( pSession->context.pLogFile );
         pSession->context.pLogFile = NULL;
      }
      vdsaSessionUnlock( pSession );
   }
   pSession->pHeader = NULL;
   
   free( pSession );
   
   return errcode;   
} 
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsExitSession( VDS_HANDLE sessionHandle )
{
   vdsaSession* pSession;
   vdseObjectContext* pObject = NULL;
   vdsaProxyObject* pProxyObject = NULL;
   int errcode;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaSessionLock( pSession ) == 0 )
   {
      if ( pSession->context.pTransaction != NULL )
      {
         vdseTxRollback( (vdseTx*)(pSession->context.pTransaction), &pSession->context );
         pSession->context.pTransaction = NULL;
      }
   
      if ( pSession->pCleanup != NULL )
      {
         errcode = vdseLock( &pSession->pCleanup->memObject, &pSession->context);
         if ( errcode == 0 )
         {
            while ( true )
            {
               errcode = vdseSessionGetFirst( pSession->pCleanup, &pObject, &pSession->context );
               if ( errcode != 0 ) break;

               /* This would be an internal error... */
               if ( pObject == NULL ) continue;
            
               if ( pObject->pProxyObject == NULL ) continue;
            
               pProxyObject = (vdsaProxyObject*) pObject->pProxyObject;
            
               errcode = vdsaProxyLock( pProxyObject );
               if ( errcode == 0 )
               {
                  vdseObjectDescriptor desc;

                  vdsaProxyCloseObject( pProxyObject );

                  desc.offset = pObject->offset;
                  desc.type   = pObject->type;

                  errcode = vdseTopFolderCloseObject( &desc, 
                                                   &pSession->context );

                  vdsaProxyUnlock( pProxyObject );
               }

               vdseSessionRemoveFirst(pSession->pCleanup, &pSession->context );
            }
            vdseUnlock( &pSession->pCleanup->memObject, &pSession->context);
         }
      
         vdseProcessRemoveSession( g_pProcessInstance->pCleanup, 
                                   pSession->pCleanup, 
                                   &pSession->context );
         pSession->pCleanup = NULL;
      }
      vdsaSessionUnlock( pSession );

      free( pSession );

      return VDS_OK;
   }
   
   return VDS_SESSION_CANNOT_GET_LOCK;
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
      rc = vdseTopFolderCreateObject( GET_PTR( pSession->pHeader->treeMgrOffset, 
                                               vdseFolder ),
                                      objectName, 
                                      objectType,
                                      &pSession->context );
      if ( rc != 0 )
         rc = vdscGetLastError( &pSession->context.errorHandler );
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
      rc = vdseTopFolderDestroyObject( GET_PTR( pSession->pHeader->treeMgrOffset, 
                                                vdseFolder ),
                                       objectName, 
                                       &pSession->context );
      if ( rc != 0 )
         rc = vdscGetLastError( &pSession->context.errorHandler );
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
      rc = vdseTxCommit( (vdseTx*)pSession->context.pTransaction, 
                         &pSession->context );
      if ( rc != 0 )
         rc = vdscGetLastError( &pSession->context.errorHandler );
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
      rc = 0;
      vdseTxRollback( (vdseTx*)pSession->context.pTransaction, 
                           &pSession->context );
      //if ( rc != 0 )
      //   rc = vdscGetLastError( &pSession->context.errorHandler );
      vdsaSessionUnlock( pSession );
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

