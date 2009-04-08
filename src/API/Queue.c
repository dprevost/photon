/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
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
      }
      if ( errcode == PSO_OK ) {
         if ( pQueue->pRecordDefinition != NULL ) {
            pQueue->pRecordDefinition->definitionType = 0; 
            free(pQueue->pRecordDefinition);
            pQueue->pRecordDefinition = NULL;
         }
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

int psoQueueDefinition( PSO_HANDLE   objectHandle, 
                        PSO_HANDLE * dataDefHandle )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoaDataDefinition * pDataDefinition = NULL;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pQueue->object.pSession->context;

   if ( dataDefHandle == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   pDataDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDataDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }

   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      
      pDataDefinition->pSession = pQueue->object.pSession;
      pDataDefinition->definitionType = PSOA_DEF_DATA;
      GET_PTR( pDataDefinition->pMemDefinition, 
               pMemQueue->dataDefOffset, 
               psonDataDefinition );
      *dataDefHandle = (PSO_HANDLE) pDataDefinition;
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
                      uint32_t     bufferLength,
                      uint32_t   * returnedLength )
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

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pQueue->iterator = NULL;
   }

   ok = psonQueueGet( pMemQueue,
                      PSO_FIRST,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );
   if ( pQueue->pRecordDefinition != NULL ) {
      GET_PTR( pQueue->pRecordDefinition->pMemDefinition, 
               pQueue->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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
                     uint32_t     bufferLength,
                     uint32_t   * returnedLength )
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

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   ok = psonQueueGet( pMemQueue,
                      PSO_NEXT,
                      &pQueue->iterator,
                      bufferLength,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );
   if ( pQueue->pRecordDefinition != NULL ) {
      GET_PTR( pQueue->pRecordDefinition->pMemDefinition, 
               pQueue->iterator->dataDefOffset, 
               psonDataDefinition );
   }
   
   return PSO_OK;

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
                  uint32_t     nameLengthInBytes,
                  PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaQueue * pQueue = NULL;
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
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
      free(pQueue);
   }
   else {
      *objectHandle = (PSO_HANDLE) pQueue;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoQueuePop( PSO_HANDLE   objectHandle,
                 void       * buffer,
                 uint32_t     bufferLength,
                 uint32_t   * returnedLength )
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

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         bufferLength,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pQueue->iterator->dataLength;
   memcpy( buffer, pQueue->iterator->data, *returnedLength );
   if ( pQueue->pRecordDefinition != NULL ) {
      GET_PTR( pQueue->pRecordDefinition->pMemDefinition, 
               pQueue->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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
                  uint32_t     dataLength,
                  PSO_HANDLE   dataDefHandle )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;
   psonDataDefinition * pMemDefinition = NULL;
   psoaDataDefinition * pDefinition = NULL;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength == 0 ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( dataDefHandle != NULL ) {
      pDefinition = (psoaDataDefinition *)dataDefHandle;
      if ( pDefinition->definitionType != PSOA_DEF_DATA ) {
         psocSetError( &pQueue->object.pSession->context.errorHandler, 
            g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
         return PSO_WRONG_TYPE_HANDLE;
      }
   }

   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      if ( pDefinition != NULL ) {
         pMemDefinition = pDefinition->pMemDefinition;
         if ( !(pMemQueue->flags & PSO_MULTIPLE_DATA_DEFINITIONS) ) {
            errcode = PSO_DATA_DEF_UNSUPPORTED;
         }
      }
      if ( errcode == PSO_OK ) {
         ok = psonQueueInsert( pMemQueue,
                               data,
                               dataLength,
                               pMemDefinition,
                               PSON_QUEUE_LAST,
                               &pQueue->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
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
                     uint32_t     dataLength,
                     PSO_HANDLE   dataDefHandle )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   bool ok = true;
   psoaDataDefinition * pDefinition = NULL;
   psonDataDefinition * pMemDefinition = NULL;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength == 0 ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( dataDefHandle != NULL ) {
      pDefinition = (psoaDataDefinition *) dataDefHandle;
   
      if ( pDefinition->definitionType != PSOA_DEF_DATA )  {
         psocSetError( &pQueue->object.pSession->context.errorHandler, 
            g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
         return PSO_WRONG_TYPE_HANDLE;
      }
   }
   
   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      if ( pDefinition != NULL ) {
         pMemDefinition = pDefinition->pMemDefinition;
         if ( !(pMemQueue->flags & PSO_MULTIPLE_DATA_DEFINITIONS) ) {
            errcode = PSO_DATA_DEF_UNSUPPORTED;
         }
      }
      if ( errcode == PSO_OK ) {
         ok = psonQueueInsertNow( pMemQueue,
                                  data,
                                  dataLength,
                                  pMemDefinition,
                                  PSON_QUEUE_LAST,
                                  &pQueue->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
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

int psoQueueRecordDefinition( PSO_HANDLE   objectHandle,
                              PSO_HANDLE * dataDefHandle )
{
   psoaQueue * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psoaDataDefinition * pDefinition = NULL;
   
   pQueue = (psoaQueue *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_QUEUE ) return PSO_WRONG_TYPE_HANDLE;

   if ( dataDefHandle == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler,
                    g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   pDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDefinition == NULL ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler,
                    g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   pDefinition->pSession = pQueue->object.pSession;
   pDefinition->definitionType = PSOA_DEF_DATA;
   
   if ( ! pQueue->object.pSession->terminated ) {
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
 
      /* We use the global queue definition as the initial value
       * to avoid an uninitialized pointer.
       */
      GET_PTR( pDefinition->pMemDefinition, 
               pMemQueue->dataDefOffset, 
               psonDataDefinition );
      pDefinition->ppApiObject = &pQueue->pRecordDefinition;
      pQueue->pRecordDefinition = pDefinition;

      dataDefHandle = (PSO_HANDLE) pDefinition;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pQueue->object.pSession->context.errorHandler, 
                    g_psoErrorHandle, errcode );
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
      pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;
      
      if ( psonLock(&pMemQueue->memObject, pContext) ) {
         psonMemObjectStatus( &pMemQueue->memObject, pStatus );

         psonQueueStatus( pMemQueue, pStatus );
         pStatus->type = PSO_QUEUE;

         psonUnlock( &pMemQueue->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
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

/* The not-published API functions in alphabetic order */

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

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pQueue->iterator = NULL;
   }

   ok = psonQueueGet( pMemQueue,
                      PSO_FIRST,
                      &pQueue->iterator,
                      (uint32_t) -1,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
   if ( pQueue->pRecordDefinition != NULL ) {
      GET_PTR( pQueue->pRecordDefinition->pMemDefinition, 
               pQueue->iterator->dataDefOffset, 
               psonDataDefinition );
   }
      
   return PSO_OK;

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

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   ok = psonQueueGet( pMemQueue,
                      PSO_NEXT,
                      &pQueue->iterator,
                      (uint32_t) -1,
                      &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   pEntry->data = pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
   if ( pQueue->pRecordDefinition != NULL ) {
      GET_PTR( pQueue->pRecordDefinition->pMemDefinition, 
               pQueue->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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

   pMemQueue = (psonQueue *) pQueue->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pQueue->iterator != NULL ) {
      ok = psonQueueRelease( pMemQueue,
                             pQueue->iterator,
                             &pQueue->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pQueue->iterator = NULL;
   }

   ok = psonQueueRemove( pMemQueue,
                         &pQueue->iterator,
                         PSON_QUEUE_FIRST,
                         (uint32_t) -1,
                         &pQueue->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   pEntry->data = (const void *) pQueue->iterator->data;
   pEntry->length = pQueue->iterator->dataLength;
   if ( pQueue->pRecordDefinition != NULL ) {
      GET_PTR( pQueue->pRecordDefinition->pMemDefinition, 
               pQueue->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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

