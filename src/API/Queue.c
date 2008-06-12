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
#include <vdsf/vdsQueue.h>
#include "API/Queue.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueClose( VDS_HANDLE objectHandle )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0;
   
   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   if ( ! pQueue->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pQueue->object ) == 0 ) {
         pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pQueue->iterator != NULL ) {
            if ( vdseQueueRelease( pVDSQueue,
                                   pQueue->iterator,
                                   &pQueue->object.pSession->context ) == 0 ) {
               pQueue->iterator = NULL;
            }
            else {
               errcode = VDS_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == 0 ) {
            errcode = vdsaCommonObjClose( &pQueue->object );
            pQueue->pDefinition = NULL;
         }
         vdsaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode == 0 ) {
      free( pQueue );
   }
   else {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueDefinition( VDS_HANDLE             objectHandle,
                        vdsObjectDefinition ** ppDefinition )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0;
   vdseSessionContext * pContext;
   
   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( ppDefinition == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pQueue->object ) == 0 ) {
         pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;
      
         errcode = vdsaGetDefinition( pQueue->pDefinition,
                                      pVDSQueue->numFields,
                                      ppDefinition );
         if ( errcode == 0 ) (*ppDefinition)->type = VDS_QUEUE;
         vdsaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueGetFirst( VDS_HANDLE   objectHandle,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     * returnedLength )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pQueue->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      rc = vdseQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      if ( rc != 0 ) goto error_handler_unlock;
      
      pQueue->iterator = NULL;
   }

   rc = vdseQueueGet( pVDSQueue,
                      VDS_FIRST,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   if ( rc != 0 ) goto error_handler_unlock;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   vdsaCommonUnlock( &pQueue->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueGetNext( VDS_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->iterator == NULL ) {
      errcode = VDS_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pQueue->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

   rc = vdseQueueGet( pVDSQueue,
                      VDS_NEXT,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   if ( rc != 0 ) goto error_handler_unlock;
   
   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   vdsaCommonUnlock( &pQueue->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueOpen( VDS_HANDLE   sessionHandle,
                  const char * queueName,
                  size_t       nameLengthInBytes,
                  VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaQueue * pQueue = NULL;
   vdseQueue * pVDSQueue;
   int errcode;
   
   if ( objectHandle == NULL ) return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( queueName == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pQueue = (vdsaQueue *) malloc(sizeof(vdsaQueue));
   if (  pQueue == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pQueue, 0, sizeof(vdsaQueue) );
   pQueue->object.type = VDSA_QUEUE;
   pQueue->object.pSession = pSession;

   if ( ! pQueue->object.pSession->terminated ) {
      errcode = vdsaCommonObjOpen( &pQueue->object,
                                   VDS_QUEUE,
                                   queueName,
                                   nameLengthInBytes );
      if ( errcode == 0 ) {
         *objectHandle = (VDS_HANDLE) pQueue;
         pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;
         GET_PTR( pQueue->pDefinition, 
                  pVDSQueue->dataDefOffset,
                  vdseFieldDef );
         vdsaGetLimits( pQueue->pDefinition,
                        pVDSQueue->numFields,
                        &pQueue->minLength,
                        &pQueue->maxLength );
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueuePop( VDS_HANDLE   objectHandle,
                 void       * buffer,
                 size_t       bufferLength,
                 size_t     * returnedLength )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pQueue->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      rc = vdseQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      if ( rc != 0 ) goto error_handler_unlock;

      pQueue->iterator = NULL;
   }
      
   rc = vdseQueueRemove( pVDSQueue,
                         &pQueue->iterator,
                         VDSE_QUEUE_FIRST,
                         bufferLength,
                         &pQueue->object.pSession->context );
   if ( rc != 0 ) goto error_handler_unlock;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   vdsaCommonUnlock( &pQueue->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueuePush( VDS_HANDLE   objectHandle,
                  const void * data,
                  size_t       dataLength )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   
   if ( dataLength < pQueue->minLength || dataLength > pQueue->maxLength ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pQueue->object ) == 0 ) {
         pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

         rc = vdseQueueInsert( pVDSQueue,
                               data,
                               dataLength,
                               VDSE_QUEUE_LAST,
                               &pQueue->object.pSession->context );

         vdsaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueStatus( VDS_HANDLE     objectHandle,
                    vdsObjStatus * pStatus )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0;
   vdseSessionContext * pContext;
   
   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL ) return VDS_NULL_HANDLE;
   
   if ( pQueue->object.type != VDSA_QUEUE ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( pStatus == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pQueue->object ) == 0 ) {
         pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;
      
         if ( vdseLock( &pVDSQueue->memObject, pContext ) == 0 ) {
            vdseMemObjectStatus( &pVDSQueue->memObject, pStatus );

            vdseQueueStatus( pVDSQueue, pStatus );

            vdseUnlock( &pVDSQueue->memObject, pContext );
         }
         else {
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         }
      
         vdsaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaQueueFirst( vdsaQueue     * pQueue,
                    vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pQueue->object.type == VDSA_QUEUE );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pQueue->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      rc = vdseQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      if ( rc != 0 ) goto error_handler_unlock;
      
      pQueue->iterator = NULL;
   }

   rc = vdseQueueGet( pVDSQueue,
                      VDS_FIRST,
                      &pQueue->iterator,
                      (size_t) -1,
                      &pQueue->object.pSession->context );
   if ( rc != 0 ) goto error_handler_unlock;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
      
   vdsaCommonUnlock( &pQueue->object );
   
   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaQueueNext( vdsaQueue     * pQueue,
                   vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pQueue->object.type == VDSA_QUEUE );
   VDS_PRE_CONDITION( pQueue->iterator != NULL );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pQueue->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

   rc = vdseQueueGet( pVDSQueue,
                      VDS_NEXT,
                      &pQueue->iterator,
                      (size_t) -1,
                      &pQueue->object.pSession->context );
   if ( rc != 0 ) goto error_handler_unlock;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;

   vdsaCommonUnlock( &pQueue->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaQueueRemove( vdsaQueue     * pQueue,
                     vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSQueue;
   int errcode = 0, rc = 0;

   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pEntry != NULL )
   VDS_PRE_CONDITION( pQueue->object.type == VDSA_QUEUE )

   if ( pQueue->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pQueue->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      rc = vdseQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      if ( rc != 0 ) goto error_handler_unlock;

      pQueue->iterator = NULL;
   }

   rc = vdseQueueRemove( pVDSQueue,
                         &pQueue->iterator,
                         VDSE_QUEUE_FIRST,
                         (size_t) -1,
                         &pQueue->object.pSession->context );
   if ( rc != 0 ) goto error_handler_unlock;

   pEntry->data = (const void *) pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
      
   vdsaCommonUnlock( &pQueue->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

