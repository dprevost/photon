/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/Lifo.h>
#include "API/Lifo.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoClose( PSO_HANDLE objectHandle )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

      /* Reinitialize the iterator, if needed */
      if ( pLifo->iterator != NULL ) {
         if ( psonQueueRelease( pMemLifo,
                                pLifo->iterator,
                                &pLifo->object.pSession->context ) ) {
            pLifo->iterator = NULL;
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      }

      if ( errcode == PSO_OK ) {
         errcode = psoaCommonObjClose( &pLifo->object );
      }
      if ( errcode == PSO_OK ) {
         if ( pLifo->pRecordDefinition != NULL ) {
            pLifo->pRecordDefinition->ppApiObject = NULL;
            pLifo->pRecordDefinition = NULL;
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
      pLifo->object.type = 0; 
      free( pLifo );
   }
   else {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoDefinition( PSO_HANDLE   objectHandle, 
                       PSO_HANDLE * dataDefHandle )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoaDataDefinition * pDataDefinition = NULL;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( dataDefHandle == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   pDataDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDataDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
      pDataDefinition->pSession = pLifo->object.pSession;
      pDataDefinition->definitionType = PSOA_DEF_DATA;
      GET_PTR( pDataDefinition->pMemDefinition, 
               pMemLifo->dataDefOffset, 
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

int psoLifoGetFirst( PSO_HANDLE   objectHandle,
                     void       * buffer,
                     uint32_t     bufferLength,
                     uint32_t   * returnedLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pLifo->iterator = NULL;
   }

   ok = psonQueueGetFirst( pMemLifo,
                           &pLifo->iterator,
                           bufferLength,
                           &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );
   if ( pLifo->pRecordDefinition != NULL ) {
      GET_PTR( pLifo->pRecordDefinition->pMemDefinition, 
               pLifo->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoGetNext( PSO_HANDLE   objectHandle,
                    void       * buffer,
                    uint32_t     bufferLength,
                    uint32_t   * returnedLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->iterator == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   ok = psonQueueGetNext( pMemLifo,
                          &pLifo->iterator,
                          bufferLength,
                          &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );
   if ( pLifo->pRecordDefinition != NULL ) {
      GET_PTR( pLifo->pRecordDefinition->pMemDefinition, 
               pLifo->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoOpen( PSO_HANDLE   sessionHandle,
                 const char * queueName,
                 uint32_t     nameLengthInBytes,
                 PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaLifo * pLifo = NULL;
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
   
   pLifo = (psoaLifo *) malloc(sizeof(psoaLifo));
   if (  pLifo == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pLifo, 0, sizeof(psoaLifo) );
   pLifo->object.type = PSOA_LIFO;
   pLifo->object.pSession = pSession;

   if ( ! pLifo->object.pSession->terminated ) {
      errcode = psoaCommonObjOpen( &pLifo->object,
                                   PSO_LIFO,
                                   PSOA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
      free(pLifo);
   }
   else {
      *objectHandle = (PSO_HANDLE) pLifo;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoPop( PSO_HANDLE   objectHandle,
                void       * buffer,
                uint32_t     bufferLength,
                uint32_t   * returnedLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pLifo->iterator = NULL;
   }

   ok = psonQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSON_QUEUE_LAST,
                         bufferLength,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );
   if ( pLifo->pRecordDefinition != NULL ) {
      GET_PTR( pLifo->pRecordDefinition->pMemDefinition, 
               pLifo->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoPush( PSO_HANDLE   objectHandle,
                 const void * data,
                 uint32_t     dataLength,
                 PSO_HANDLE   dataDefHandle )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;
   psonDataDefinition * pMemDefinition = NULL;
   psoaDataDefinition * pDefinition = NULL;
   psonDataDefinition * pDef;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength == 0 ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( dataDefHandle != NULL ) {
      pDefinition = (psoaDataDefinition *)dataDefHandle;
      if ( pDefinition->definitionType != PSOA_DEF_DATA ) {
         psocSetError( &pLifo->object.pSession->context.errorHandler, 
            g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
         return PSO_WRONG_TYPE_HANDLE;
      }
   }
   
   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

      if ( pDefinition != NULL ) {
         if ( !(pMemLifo->flags & PSO_MULTIPLE_DATA_DEFINITIONS) ) {
            errcode = PSO_DATA_DEF_UNSUPPORTED;
         }
         else {
            pDef = GET_PTR_FAST( pMemLifo->dataDefOffset, psonDataDefinition );
            if ( pDefinition->pMemDefinition->type != pDef->type ) {
               errcode = PSO_INVALID_DATA_DEFINITION_TYPE;
            }
         }
         pMemDefinition = pDefinition->pMemDefinition;
      }

      if ( errcode == PSO_OK ) {
         ok = psonQueueInsert( pMemLifo,
                               data,
                               dataLength,
                               pMemDefinition,
                               PSON_QUEUE_LAST,
                               &pLifo->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoRecordDefinition( PSO_HANDLE   objectHandle,
                             PSO_HANDLE * dataDefHandle )
{
   psoaLifo  * pQueue;
   psonQueue * pMemQueue;
   int errcode = PSO_OK;
   psoaDataDefinition * pDefinition = NULL;
   
   pQueue = (psoaLifo *) objectHandle;
   if ( pQueue == NULL ) return PSO_NULL_HANDLE;
   
   if ( pQueue->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

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
         
      *dataDefHandle = (PSO_HANDLE) pDefinition;
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

int psoLifoStatus( PSO_HANDLE     objectHandle,
                   psoObjStatus * pStatus )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
      if ( psonLock(&pMemLifo->memObject, pContext) ) {
         psonMemObjectStatus( &pMemLifo->memObject, pStatus );

         psonQueueStatus( pMemLifo, pStatus );
         pStatus->type = PSO_LIFO;

         psonUnlock( &pMemLifo->memObject, pContext );
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

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoFirst( psoaLifo       * pLifo,
                   unsigned char ** pData,
                   uint32_t       * pLength )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo   != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      pLifo->iterator = NULL;
   }

   ok = psonQueueGetFirst( pMemLifo,
                           &pLifo->iterator,
                           (uint32_t) -1,
                           &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pLifo->iterator->data;
   *pLength = pLifo->iterator->dataLength;
   if ( pLifo->pRecordDefinition != NULL ) {
      GET_PTR( pLifo->pRecordDefinition->pMemDefinition, 
               pLifo->iterator->dataDefOffset, 
               psonDataDefinition );
   }
      
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoNext( psoaLifo       * pLifo,
                  unsigned char ** pData,
                  uint32_t       * pLength )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo   != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO );
   PSO_PRE_CONDITION( pLifo->iterator != NULL );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   ok = psonQueueGetNext( pMemLifo,
                          &pLifo->iterator,
                          (uint32_t) -1,
                          &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pLifo->iterator->data;
   *pLength = pLifo->iterator->dataLength;
   if ( pLifo->pRecordDefinition != NULL ) {
      GET_PTR( pLifo->pRecordDefinition->pMemDefinition, 
               pLifo->iterator->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaLifoRemove( psoaLifo       * pLifo,
                    unsigned char ** pData,
                    uint32_t       * pLength )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo   != NULL );
   PSO_PRE_CONDITION( pData   != NULL );
   PSO_PRE_CONDITION( pLength != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO )

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      pLifo->iterator = NULL;
   }

   ok = psonQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSON_QUEUE_LAST,
                         (uint32_t) -1,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *pData = pLifo->iterator->data;
   *pLength = pLifo->iterator->dataLength;
   if ( pLifo->pRecordDefinition != NULL ) {
      GET_PTR( pLifo->pRecordDefinition->pMemDefinition, 
               pLifo->iterator->dataDefOffset, 
               psonDataDefinition );
   }
      
   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

