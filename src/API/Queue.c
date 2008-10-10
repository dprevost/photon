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
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pQueue->object.pSession->terminated ) {
      if ( psoaCommonLock( &pQueue->object ) ) {
         pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

         /* Reinitialize the iterator, if needed */
         if ( pQueue->iterator != NULL ) {
            if ( psonQueueRelease( pMemQueue,
                                   pQueue->iterator,
                                   &pQueue->object.pSession->context ) ) {
               pQueue->iterator = NULL;
            }
            else {
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == PSO_OK ) {
            errcode = psoaCommonObjClose( &pQueue->object );
            pQueue->pDefinition = NULL;
         }
         psoaCommonUnlock( &pQueue->object );
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
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueDefinition( PSO_HANDLE             objectHandle,
                        psoObjectDefinition ** ppDefinition )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( ppDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      if ( psoaCommonLock( &pQueue->object ) ) {
         pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      
         errcode = psoaGetDefinition( pQueue->pDefinition,
                                      pMemQueue->numFields,
                                      ppDefinition );
         if ( errcode == PSO_OK ) (*ppDefinition)->type = PSO_QUEUE;
         psoaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueGetFirst( PSO_HANDLE   objectHandle,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     * returnedLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pQueue->iterator = NULL;
   }

   ok = psonQueueGet( pMemQueue,
                      PSO_FIRST,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   psoaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueGetNext( PSO_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( ! psoaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   ok = psonQueueGet( pMemQueue,
                      PSO_NEXT,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   psoaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueOpen( PSO_HANDLE   sessionHandle,
                  const char * queueName,
                  size_t       nameLengthInBytes,
                  PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaQueue * pQueue = NULL;
   psonQueue * pMemQueue;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( queueName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pQueue = (psoaQueue *) malloc(sizeof(psoaQueue));
   if (  pQueue == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pQueue, 0, sizeof(psoaQueue) );
   pQueue->object.type = PSOA_QUEUE;
   pQueue->object.pSession = pSession;

   if ( ! pQueue->object.pSession->terminated ) {
      errcode = psoaCommonObjOpen( &pQueue->object,
                                   PSO_QUEUE,
                                   PSOA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pQueue;
         pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
         GET_PTR( pQueue->pDefinition, 
                  pMemQueue->dataDefOffset,
                  psonFieldDef );
         psoaGetLimits( pQueue->pDefinition,
                        pMemQueue->numFields,
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
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         bufferLength,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );

   psoaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePush( PSO_HANDLE   objectHandle,
                  const void * data,
                  size_t       dataLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength < pQueue->minLength || dataLength > pQueue->maxLength ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      if ( psoaCommonLock( &pQueue->object ) ) {
         pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

         ok = psonQueueInsert( pMemQueue,
                               data,
                               dataLength,
                               PSON_QUEUE_LAST,
                               &pQueue->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psoaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePushNow( PSO_HANDLE   objectHandle,
                     const void * data,
                     size_t       dataLength )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength < pQueue->minLength || dataLength > pQueue->maxLength ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      if ( psoaCommonLock( &pQueue->object ) ) {
         pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

         ok = psonQueueInsertNow( pMemQueue,
                                  data,
                                  dataLength,
                                  PSON_QUEUE_LAST,
                                  &pQueue->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psoaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueueStatus( PSO_HANDLE     objectHandle,
                    psoObjStatus * pStatus )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      if ( psoaCommonLock( &pQueue->object ) ) {
         pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      
         if ( psonLock(&pMemQueue->memObject, pContext) ) {
            psonMemObjectStatus( &pMemQueue->memObject, pStatus );

            psonQueueStatus( pMemQueue, pStatus );

            psonUnlock( &pMemQueue->memObject, pContext );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      
         psoaCommonUnlock( &pQueue->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueFirst( psoaQueue     * pQueue,
                   psoaDataEntry * pEntry )
{
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSOA_QUEUE );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pQueue->iterator = NULL;
   }

   ok = psonQueueGet( pMemQueue,
                      PSO_FIRST,
                      &pQueue->iterator,
                      (size_t) -1,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
      
   psoaCommonUnlock( &pQueue->object );
   
   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueNext( psoaQueue     * pQueue,
                  psoaDataEntry * pEntry )
{
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pQueue->object.type == PSOA_QUEUE );
   PSO_PRE_CONDITION( pQueue->iterator != NULL );
   
   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   ok = psonQueueGet( pMemQueue,
                      PSO_NEXT,
                      &pQueue->iterator,
                      (size_t) -1,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;

   psoaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaQueueRemove( psoaQueue     * pQueue,
                    psoaDataEntry * pEntry )
{
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pQueue != NULL );
   PSO_PRE_CONDITION( pEntry != NULL )
   PSO_PRE_CONDITION( pQueue->object.type == PSOA_QUEUE )

   if ( pQueue->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pQueue->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         (size_t) -1,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = (const void *) pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
      
   psoaCommonUnlock( &pQueue->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pQueue->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pQueue->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

