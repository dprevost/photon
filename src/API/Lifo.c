/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock( &pLifo->object ) ) {
         pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

         /* Reinitialize the iterator, if needed */
         if ( pLifo->iterator != NULL ) {
            if ( psnQueueRelease( pMemLifo,
                                   pLifo->iterator,
                                   &pLifo->object.pSession->context ) ) {
               pLifo->iterator = NULL;
            }
            else {
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == PSO_OK ) {
            errcode = psaCommonObjClose( &pLifo->object );
            pLifo->pDefinition = NULL;
         }
         psaCommonUnlock( &pLifo->object );
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
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoDefinition( PSO_HANDLE             objectHandle,
                       psoObjectDefinition ** ppDefinition )
{
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( ppDefinition == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock( &pLifo->object ) ) {
         pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;
      
         errcode = psaGetDefinition( pLifo->pDefinition,
                                      pMemLifo->numFields,
                                      ppDefinition );
         if ( errcode == PSO_OK ) (*ppDefinition)->type = PSO_LIFO;
         psaCommonUnlock( &pLifo->object );
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

int psoLifoGetFirst( PSO_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength )
{
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = psnQueueGet( pMemLifo,
                      PSO_FIRST,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoGetNext( PSO_HANDLE   objectHandle,
                    void       * buffer,
                    size_t       bufferLength,
                    size_t     * returnedLength )
{
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

   ok = psnQueueGet( pMemLifo,
                      PSO_NEXT,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoOpen( PSO_HANDLE   sessionHandle,
                 const char * queueName,
                 size_t       nameLengthInBytes,
                 PSO_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaLifo * pLifo = NULL;
   psnQueue * pMemLifo;
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
   
   pLifo = (psaLifo *) malloc(sizeof(psaLifo));
   if (  pLifo == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pLifo, 0, sizeof(psaLifo) );
   pLifo->object.type = PSA_LIFO;
   pLifo->object.pSession = pSession;

   if ( ! pLifo->object.pSession->terminated ) {
      errcode = psaCommonObjOpen( &pLifo->object,
                                   PSO_LIFO,
                                   PSA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pLifo;
         pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;
         GET_PTR( pLifo->pDefinition, 
                  pMemLifo->dataDefOffset,
                  psnFieldDef );
         psaGetLimits( pLifo->pDefinition,
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
                size_t       bufferLength,
                size_t     * returnedLength )
{
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = psnQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSN_QUEUE_LAST,
                         bufferLength,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoPush( PSO_HANDLE   objectHandle,
                 const void * data,
                 size_t       dataLength )
{
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataLength < pLifo->minLength || dataLength > pLifo->maxLength ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock( &pLifo->object ) ) {
         pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

         ok = psnQueueInsert( pMemLifo,
                               data,
                               dataLength,
                               PSN_QUEUE_LAST,
                               &pLifo->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoLifoStatus( PSO_HANDLE     objectHandle,
                   psoObjStatus * pStatus )
{
   psaLifo * pLifo;
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return PSO_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock(&pLifo->object) ) {
         pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;
      
         if ( psnLock(&pMemLifo->memObject, pContext) ) {
            psnMemObjectStatus( &pMemLifo->memObject, pStatus );

            psnQueueStatus( pMemLifo, pStatus );

            psnUnlock( &pMemLifo->memObject, pContext );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      
         psaCommonUnlock( &pLifo->object );
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

int psaLifoFirst( psaLifo      * pLifo,
                  psaDataEntry * pEntry )
{
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSA_LIFO );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = psnQueueGet( pMemLifo,
                      PSO_FIRST,
                      &pLifo->iterator,
                      (size_t) -1,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   psaCommonUnlock( &pLifo->object );
   
   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaLifoNext( psaLifo      * pLifo,
                 psaDataEntry * pEntry )
{
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo != NULL );
   PSO_PRE_CONDITION( pEntry != NULL );
   PSO_PRE_CONDITION( pLifo->object.type == PSA_LIFO );
   PSO_PRE_CONDITION( pLifo->iterator != NULL );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

   ok = psnQueueGet( pMemLifo,
                      PSO_NEXT,
                      &pLifo->iterator,
                      (size_t) -1,
                      &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;

   psaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaLifoRemove( psaLifo      * pLifo,
                   psaDataEntry * pEntry )
{
   psnQueue * pMemLifo;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pLifo != NULL );
   PSO_PRE_CONDITION( pEntry != NULL )
   PSO_PRE_CONDITION( pLifo->object.type == PSA_LIFO )

   if ( pLifo->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemLifo = (psnQueue *) pLifo->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pMemLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = psnQueueRemove( pMemLifo,
                         &pLifo->iterator,
                         PSN_QUEUE_LAST,
                         (size_t) -1,
                         &pLifo->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = (const void *) pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   psaCommonUnlock( &pLifo->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

