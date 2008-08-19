/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
#include <photon/psoQueue.h>
#include "API/Queue.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueClose( PSO_HANDLE objectHandle )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   
   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pQueue->object.pSession->terminated ) {
      if ( psaCommonLock( &pQueue->object ) ) {
         pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pQueue->iterator != NULL ) {
            if ( psnQueueRelease( pVDSQueue,
                                   pQueue->iterator,
                                   &pQueue->object.pSession->context ) ) {
               pQueue->iterator = NULL;
            }
            else {
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == PSO_OK ) {
            errcode = psaCommonObjClose( &pQueue->object );
            pQueue->pDefinition = NULL;
         }
         psaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode == PSO_OK ) {
      /*
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pQueue->object.type = 0; 
      free( pQueue );
   }
   else {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueDefinition( PSO_HANDLE             objectHandle,
                        psoObjectDefinition ** ppDefinition )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( ppDefinition == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      if ( psaCommonLock( &pQueue->object ) ) {
         pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;
      
         errcode = psaGetDefinition( pQueue->pDefinition,
                                      pVDSQueue->numFields,
                                      ppDefinition );
         if ( errcode == PSO_OK ) (*ppDefinition)->type = PSO_QUEUE;
         psaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueGetFirst( PSO_HANDLE   objectHandle,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     * returnedLength )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psnQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pQueue->iterator = NULL;
   }

   ok = psnQueueGet( pVDSQueue,
                      PSO_FIRST,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   psaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueGetNext( PSO_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->iterator == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

   ok = psnQueueGet( pVDSQueue,
                      PSO_NEXT,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   psaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueOpen( PSO_HANDLE   sessionHandle,
                  const char * queueName,
                  size_t       nameLengthInBytes,
                  PSO_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaQueue * pQueue = NULL;
   psnQueue * pVDSQueue;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( queueName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pQueue = (psaQueue *) malloc(sizeof(psaQueue));
   if (  pQueue == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pQueue, 0, sizeof(psaQueue) );
   pQueue->object.type = PSA_QUEUE;
   pQueue->object.pSession = pSession;

   if ( ! pQueue->object.pSession->terminated ) {
      errcode = psaCommonObjOpen( &pQueue->object,
                                   PSO_QUEUE,
                                   PSA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pQueue;
         pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;
         GET_PTR( pQueue->pDefinition, 
                  pVDSQueue->dataDefOffset,
                  psnFieldDef );
         psaGetLimits( pQueue->pDefinition,
                        pVDSQueue->numFields,
                        &pQueue->minLength,
                        &pQueue->maxLength );
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePop( PSO_HANDLE   objectHandle,
                 void       * buffer,
                 size_t       bufferLength,
                 size_t     * returnedLength )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psnQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pQueue->iterator = NULL;
   }

   ok = psnQueueRemove( pVDSQueue,
                         &pQueue->iterator,
                         PSN_QUEUE_FIRST,
                         bufferLength,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   psaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePush( PSO_HANDLE   objectHandle,
                  const void * data,
                  size_t       dataLength )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength < pQueue->minLength || dataLength > pQueue->maxLength ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      if ( psaCommonLock( &pQueue->object ) ) {
         pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

         ok = psnQueueInsert( pVDSQueue,
                               data,
                               dataLength,
                               PSN_QUEUE_LAST,
                               &pQueue->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePushNow( PSO_HANDLE   objectHandle,
                     const void * data,
                     size_t       dataLength )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength < pQueue->minLength || dataLength > pQueue->maxLength ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      if ( psaCommonLock( &pQueue->object ) ) {
         pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

         ok = psnQueueInsertNow( pVDSQueue,
                                  data,
                                  dataLength,
                                  PSN_QUEUE_LAST,
                                  &pQueue->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueStatus( PSO_HANDLE     objectHandle,
                    psoObjStatus * pStatus )
{
   psaQueue * pQueue;
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pQueue = (psaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      if ( psaCommonLock( &pQueue->object ) ) {
         pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;
      
         if ( psnLock(&pVDSQueue->memObject, pContext) ) {
            psnMemObjectStatus( &pVDSQueue->memObject, pStatus );

            psnQueueStatus( pVDSQueue, pStatus );

            psnUnlock( &pVDSQueue->memObject, pContext );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      
         psaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaQueueFirst( psaQueue     * pQueue,
                   psaDataEntry * pEntry )
{
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSA_QUEUE );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psnQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pQueue->iterator = NULL;
   }

   ok = psnQueueGet( pVDSQueue,
                      PSO_FIRST,
                      &pQueue->iterator,
                      (size_t) -1,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
      
   psaCommonUnlock( &pQueue->object );
   
   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaQueueNext( psaQueue     * pQueue,
                  psaDataEntry * pEntry )
{
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSA_QUEUE );
   PSO_PRE_CONDITION( pQueue->iterator != NULL );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

   ok = psnQueueGet( pVDSQueue,
                      PSO_NEXT,
                      &pQueue->iterator,
                      (size_t) -1,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;

   psaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaQueueRemove( psaQueue     * pQueue,
                    psaDataEntry * pEntry )
{
   psnQueue * pVDSQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pEntry != NULL )
   PSO_PRE_CONDITION( pQueue->object.type == PSA_QUEUE )

   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSQueue = (psnQueue *) pQueue->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psnQueueRelease( pVDSQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pQueue->iterator = NULL;
   }

   ok = psnQueueRemove( pVDSQueue,
                         &pQueue->iterator,
                         PSN_QUEUE_FIRST,
                         (size_t) -1,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = (const void *) pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
      
   psaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

