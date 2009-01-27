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
#include <photon/psoLifo.h>
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
      if ( psoaCommonLock( &pLifo->object ) ) {
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
            pLifo->pDefinition = NULL;
         }
         psoaCommonUnlock( &pLifo->object );
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

int psoLifoDefinition( PSO_HANDLE            objectHandle,
                       psoBasicObjectDef * pDefinition,
                       psoUint32             numFields,
                       psoFieldDefinition  * pFields )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( numFields > 0 && pFields == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psoaCommonLock( &pLifo->object ) ) {
         pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
         pDefinition->type = PSO_LIFO;
         pDefinition->numFields = pMemLifo->numFields;
         if ( numFields > 0 ) {
            errcode = psoaGetDefinition( pLifo->pDefinition,
                                         pMemLifo->numFields,
                                         pFields );
         }
         psoaCommonUnlock( &pLifo->object );
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

   if ( ! psoaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = psonQueueGet( pMemLifo,
                      PSO_FIRST,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psoaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pLifo->object );

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

   if ( ! psoaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   ok = psonQueueGet( pMemLifo,
                      PSO_NEXT,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psoaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pLifo->object );

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
   psonQueue * pMemLifo;
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
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pLifo;
         pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
         GET_PTR( pLifo->pDefinition, 
                  pMemLifo->dataDefOffset,
                  psonFieldDef );
         psoaGetLimits( pLifo->pDefinition,
                        pMemLifo->numFields,
                        &pLifo->minLength,
                        &pLifo->maxLength );
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
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

   if ( ! psoaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = psonQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSON_QUEUE_LAST,
                         bufferLength,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psoaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pLifo->object );

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
                 uint32_t     dataLength )
{
   psoaLifo * pLifo;
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psoaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSOA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength < pLifo->minLength || dataLength > pLifo->maxLength ) {
      psocSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pLifo->object.pSession->terminated ) {
      if ( psoaCommonLock( &pLifo->object ) ) {
         pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

         ok = psonQueueInsert( pMemLifo,
                               data,
                               dataLength,
                               PSON_QUEUE_LAST,
                               &pLifo->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psoaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
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
      if ( psoaCommonLock(&pLifo->object) ) {
         pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;
      
         if ( psonLock(&pMemLifo->memObject, pContext) ) {
            psonMemObjectStatus( &pMemLifo->memObject, pStatus );

            psonQueueStatus( pMemLifo, pStatus );

            psonUnlock( &pMemLifo->memObject, pContext );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      
         psoaCommonUnlock( &pLifo->object );
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

int psoaLifoFirst( psoaLifo      * pLifo,
                   psoaDataEntry * pEntry )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = psonQueueGet( pMemLifo,
                      PSO_FIRST,
                      &pLifo->iterator,
                      (uint32_t) -1,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   psoaCommonUnlock( &pLifo->object );
   
   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pLifo->object );

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

int psoaLifoNext( psoaLifo      * pLifo,
                  psoaDataEntry * pEntry )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO );
   PSO_PRE_CONDITION( pLifo->iterator != NULL );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   ok = psonQueueGet( pMemLifo,
                      PSO_NEXT,
                      &pLifo->iterator,
                      (uint32_t) -1,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;

   psoaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pLifo->object );

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

int psoaLifoRemove( psoaLifo      * pLifo,
                    psoaDataEntry * pEntry )
{
   psonQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo != NULL );
   PSO_PRE_CONDITION( pEntry != NULL )
   PSO_PRE_CONDITION( pLifo->object.type == PSOA_LIFO )

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psonQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psonQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = psonQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSON_QUEUE_LAST,
                         (uint32_t) -1,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = (const void *) pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   psoaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pLifo->object );

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

