/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <vdsf/vdsLifo.h>
#include "API/Lifo.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoClose( VDS_HANDLE objectHandle )
{
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   
   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( ! pLifo->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pLifo->object ) ) {
         pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pLifo->iterator != NULL ) {
            if ( vdseQueueRelease( pVDSLifo,
                                   pLifo->iterator,
                                   &pLifo->object.pSession->context ) ) {
               pLifo->iterator = NULL;
            }
            else {
               errcode = VDS_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == VDS_OK ) {
            errcode = vdsaCommonObjClose( &pLifo->object );
            pLifo->pDefinition = NULL;
         }
         vdsaCommonUnlock( &pLifo->object );
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
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoDefinition( VDS_HANDLE             objectHandle,
                       vdsObjectDefinition ** ppDefinition )
{
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   vdseSessionContext * pContext;
   
   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( ppDefinition == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pLifo->object ) ) {
         pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;
      
         errcode = vdsaGetDefinition( pLifo->pDefinition,
                                      pVDSLifo->numFields,
                                      ppDefinition );
         if ( errcode == VDS_OK ) (*ppDefinition)->type = VDS_LIFO;
         vdsaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoGetFirst( VDS_HANDLE   objectHandle,
                     void       * buffer,
                     size_t       bufferLength,
                     size_t     * returnedLength )
{
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = vdseQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = vdseQueueGet( pVDSLifo,
                      VDS_FIRST,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   vdsaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoGetNext( VDS_HANDLE   objectHandle,
                    void       * buffer,
                    size_t       bufferLength,
                    size_t     * returnedLength )
{
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;
   
   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

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

   if ( ! vdsaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

   ok = vdseQueueGet( pVDSLifo,
                      VDS_NEXT,
                      &pLifo->iterator,
                      bufferLength,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   vdsaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoOpen( VDS_HANDLE   sessionHandle,
                 const char * queueName,
                 size_t       nameLengthInBytes,
                 VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaLifo * pLifo = NULL;
   vdseQueue * pVDSLifo;
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
   
   pLifo = (vdsaLifo *) malloc(sizeof(vdsaLifo));
   if (  pLifo == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pLifo, 0, sizeof(vdsaLifo) );
   pLifo->object.type = VDSA_LIFO;
   pLifo->object.pSession = pSession;

   if ( ! pLifo->object.pSession->terminated ) {
      errcode = vdsaCommonObjOpen( &pLifo->object,
                                   VDS_LIFO,
                                   VDSA_READ_WRITE,
                                   queueName,
                                   nameLengthInBytes );
      if ( errcode == VDS_OK ) {
         *objectHandle = (VDS_HANDLE) pLifo;
         pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;
         GET_PTR( pLifo->pDefinition, 
                  pVDSLifo->dataDefOffset,
                  vdseFieldDef );
         vdsaGetLimits( pLifo->pDefinition,
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
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || returnedLength == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   *returnedLength = 0;
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = vdseQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = vdseQueueRemove( pVDSLifo,
                         &pLifo->iterator,
                         VDSE_QUEUE_LAST,
                         bufferLength,
                         &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pLifo->iterator->dataLength;
   memcpy( buffer, pLifo->iterator->data, *returnedLength );

   vdsaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoPush( VDS_HANDLE   objectHandle,
                 const void * data,
                 size_t       dataLength )
{
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   if ( data == NULL ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   
   if ( dataLength < pLifo->minLength || dataLength > pLifo->maxLength ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( ! pLifo->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pLifo->object ) ) {
         pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

         ok = vdseQueueInsert( pVDSLifo,
                               data,
                               dataLength,
                               VDSE_QUEUE_LAST,
                               &pLifo->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );

         vdsaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsLifoStatus( VDS_HANDLE     objectHandle,
                   vdsObjStatus * pStatus )
{
   vdsaLifo * pLifo;
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   vdseSessionContext * pContext;
   
   pLifo = (vdsaLifo *) objectHandle;
   if ( pLifo == NULL ) return VDS_NULL_HANDLE;
   
   if ( pLifo->object.type != VDSA_LIFO ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pLifo->object.pSession->context;

   if ( pStatus == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pLifo->object.pSession->terminated ) {
      if ( vdsaCommonLock(&pLifo->object) ) {
         pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;
      
         if ( vdseLock(&pVDSLifo->memObject, pContext) ) {
            vdseMemObjectStatus( &pVDSLifo->memObject, pStatus );

            vdseQueueStatus( pVDSLifo, pStatus );

            vdseUnlock( &pVDSLifo->memObject, pContext );
         }
         else {
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         }
      
         vdsaCommonUnlock( &pLifo->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != VDS_OK ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaLifoFirst( vdsaLifo      * pLifo,
                   vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pLifo != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pLifo->object.type == VDSA_LIFO );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = vdseQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      pLifo->iterator = NULL;
   }

   ok = vdseQueueGet( pVDSLifo,
                      VDS_FIRST,
                      &pLifo->iterator,
                      (size_t) -1,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   vdsaCommonUnlock( &pLifo->object );
   
   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaLifoNext( vdsaLifo      * pLifo,
                  vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pLifo != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pLifo->object.type == VDSA_LIFO );
   VDS_PRE_CONDITION( pLifo->iterator != NULL );
   
   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

   ok = vdseQueueGet( pVDSLifo,
                      VDS_NEXT,
                      &pLifo->iterator,
                      (size_t) -1,
                      &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;

   vdsaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaLifoRemove( vdsaLifo      * pLifo,
                    vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSLifo;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pLifo != NULL );
   VDS_PRE_CONDITION( pEntry != NULL )
   VDS_PRE_CONDITION( pLifo->object.type == VDSA_LIFO )

   if ( pLifo->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pLifo->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSLifo = (vdseQueue *) pLifo->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pLifo->iterator != NULL ) {
      ok = vdseQueueRelease( pVDSLifo,
                             pLifo->iterator,
                             &pLifo->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      pLifo->iterator = NULL;
   }

   ok = vdseQueueRemove( pVDSLifo,
                         &pLifo->iterator,
                         VDSE_QUEUE_LAST,
                         (size_t) -1,
                         &pLifo->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   pEntry->data = (const void *) pLifo->iterator->data;
   pEntry->length = pLifo->iterator->dataLength;
      
   vdsaCommonUnlock( &pLifo->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pLifo->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pLifo->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pLifo->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

