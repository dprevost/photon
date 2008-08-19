/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/vdsLifo.h>
#include "API/Lifo.h"
#include "API/Session.h"
#include <photon/vdsErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoClose( VDS_HANDLE objectHandle )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock( &pLifo->object ) ) {
         pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pLifo->iterator != NULL ) {
            if ( psnQueueRelease( pVDSLifo,
                                   pLifo->iterator,
                                   &pLifo->object.pSession->context ) ) {
               pLifo->iterator = NULL;
            }
            else {
               errcode = VDS_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == VDS_OK ) {
            errcode = psaCommonObjClose( &pLifo->object );
            pLifo->pDefinition = NULL;
         }
         psaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode == VDS_OK ) {
      /*
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pLifo->object.type = 0; 
      free( pLifo );
   }
   else {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoDefinition( VDS_HANDLE             objectHandle,
                       vdsObjectDefinition ** ppDefinition )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   psnSessionContext * pContext;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( ppDefinition == NULL ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock( &pLifo->object ) ) {
         pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;
      
         errcode = psaGetDefinition( pLifo->pDefinition,
                                      pVDSLifo->numFields,
                                      ppDefinition );
         if ( errcode == VDS_OK ) (*ppDefinition)->type = VDS_LIFO;
         psaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoGetFirst( VDS_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = psnQueueGet( pVDSLifo,
                      VDS_FIRST,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoGetNext( VDS_HANDLE   objectHandle,
                    void       * buffer,
                    size_t       bufferLength,
                    size_t     * returnedLength )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->iterator == NULL ) {
      errcode = VDS_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

   ok = psnQueueGet( pVDSLifo,
                      VDS_NEXT,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoOpen( VDS_HANDLE   sessionHandle,
                 const char * queueName,
                 size_t       nameLengthInBytes,
                 VDS_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaLifo * pLifo = NULL;
   psnQueue * pVDSLifo;
   int errcode;
   
   if ( objectHandle == NULL ) return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( queueName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pLifo = (psaLifo *) malloc(sizeof(psaLifo));
   if (  pLifo == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pLifo, 0, sizeof(psaLifo) );
   pLifo->object.type = PSA_LIFO;
   pLifo->object.pSession = pSession;

   if ( ! pLifo->object.pSession->terminated ) {
      errcode = psaCommonObjOpen( &pLifo->object,
                                   VDS_LIFO,
                                   PSA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
      if ( errcode == VDS_OK ) {
         *objectHandle = (VDS_HANDLE) pLifo;
         pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;
         GET_PTR( pLifo->pDefinition, 
                  pVDSLifo->dataDefOffset,
                  psnFieldDef );
         psaGetLimits( pLifo->pDefinition,
                        pVDSLifo->numFields,
                        &pLifo->minLength,
                        &pLifo->maxLength );
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoPop( VDS_HANDLE   objectHandle,
                void       * buffer,
                size_t       bufferLength,
                size_t     * returnedLength )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = psnQueueRemove( pVDSLifo,
                         &pLifo->iterator,
                         PSN_QUEUE_LAST,
                         bufferLength,
                         &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   psaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoPush( VDS_HANDLE   objectHandle,
                 const void * data,
                 size_t       dataLength )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   
   if ( dataLength < pLifo->minLength || dataLength > pLifo->maxLength ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock( &pLifo->object ) ) {
         pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

         ok = psnQueueInsert( pVDSLifo,
                               data,
                               dataLength,
                               PSN_QUEUE_LAST,
                               &pLifo->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoStatus( VDS_HANDLE     objectHandle,
                   vdsObjStatus * pStatus )
{
   psaLifo * pLifo;
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   psnSessionContext * pContext;
   
   pLifo = (psaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != PSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( psaCommonLock(&pLifo->object) ) {
         pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;
      
         if ( psnLock(&pVDSLifo->memObject, pContext) ) {
            psnMemObjectStatus( &pVDSLifo->memObject, pStatus );

            psnQueueStatus( pVDSLifo, pStatus );

            psnUnlock( &pVDSLifo->memObject, pContext );
         }
         else {
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         }
      
         psaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaLifoFirst( psaLifo      * pLifo,
                  psaDataEntry * pEntry )
{
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pLifo != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pLifo->object.type == PSA_LIFO );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = psnQueueGet( pVDSLifo,
                      VDS_FIRST,
                      &pLifo->iterator,
                      (size_t) -1,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   psaCommonUnlock( &pLifo->object );
   
   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
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
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pLifo != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pLifo->object.type == PSA_LIFO );
   VDS_PRE_CONDITION( pLifo->iterator != NULL );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

   ok = psnQueueGet( pVDSLifo,
                      VDS_NEXT,
                      &pLifo->iterator,
                      (size_t) -1,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;

   psaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
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
   psnQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pLifo != NULL );
   VDS_PRE_CONDITION( pEntry != NULL )
   VDS_PRE_CONDITION( pLifo->object.type == PSA_LIFO )

   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (psnQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = psnQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = psnQueueRemove( pVDSLifo,
                         &pLifo->iterator,
                         PSN_QUEUE_LAST,
                         (size_t) -1,
                         &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = (const void *) pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   psaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

