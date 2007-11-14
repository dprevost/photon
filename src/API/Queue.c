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
#include <vdsf/vdsQueue.h>
#include "API/Queue.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueueClose( VDS_HANDLE objectHandle )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0;
   
   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pQueue->type != VDSA_QUEUE )
      return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pQueue->iterator != NULL )
      {
         if ( vdseQueueRelease( pVDSQueue,
                                pQueue->iterator,
                                &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }

      if ( errcode == 0 )
         errcode = vdsCommonObjectClose( &pQueue->object );

      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode == 0 )
   {
      free( pQueue );
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
   int errcode = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pQueue->type != VDSA_QUEUE )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL )
      return VDS_NULL_POINTER;

   if ( returnedLength == NULL )
      return VDS_NULL_POINTER;
   *returnedLength = 0;

   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pQueue->iterator != NULL )
      {
         if ( vdseQueueRelease( pVDSQueue,
                                pQueue->iterator,
                                &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }
      
      if ( errcode == 0 )
      {
         errcode = vdseQueueGet( pVDSQueue,
                                 VDS_FIRST,
                                 &pQueue->iterator,
                                 bufferLength,
                                 &pQueue->object.pSession->context );
         if ( errcode == 0 )
         {
            *returnedLength = pQueue->iterator->dataLength;
            memcpy( buffer, pQueue->iterator->data, *returnedLength );
         }
         else
         {
            errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
         }
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

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
   int errcode = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pQueue->type != VDSA_QUEUE )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL )
      return VDS_NULL_POINTER;

   if ( returnedLength == NULL )
      return VDS_NULL_POINTER;
   *returnedLength = 0;

   if ( pQueue->iterator == NULL )
      return VDS_INVALID_ITERATOR;
   
   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      errcode = vdseQueueGet( pVDSQueue,
                              VDS_NEXT,
                              &pQueue->iterator,
                              bufferLength,
                              &pQueue->object.pSession->context );
      if ( errcode == 0 )
      {
         *returnedLength = pQueue->iterator->dataLength;
         memcpy( buffer, pQueue->iterator->data, *returnedLength );
      }
      else
      {
         errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
      }
      if ( errcode == VDS_REACHED_THE_END )
      {
         /* Reinitialize the iterator, if needed */
         if ( pQueue->iterator != NULL )
         {
            if ( vdseQueueRelease( pVDSQueue,
                                   pQueue->iterator,
                                   &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         }
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

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
   int errcode;
   
   if ( objectHandle == NULL )
      return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( queueName == NULL )
      return VDS_INVALID_OBJECT_NAME;

   if ( nameLengthInBytes == 0 )
      return VDS_INVALID_LENGTH;

   pQueue = (vdsaQueue *) malloc(sizeof(vdsaQueue));
   if (  pQueue == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pQueue, 0, sizeof(vdsaQueue) );
   pQueue->type = VDSA_QUEUE;
   pQueue->object.pSession = pSession;

   errcode = vdsaCommonObjectOpen( &pQueue->object,
                                   VDS_QUEUE,
                                   queueName,
                                   nameLengthInBytes );
   if ( errcode == 0 )
      *objectHandle = (VDS_HANDLE) pQueue;

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
   int errcode = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pQueue->type != VDSA_QUEUE )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL )
      return VDS_NULL_POINTER;

   if ( returnedLength == NULL )
      return VDS_NULL_POINTER;
   *returnedLength = 0;
      
   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pQueue->iterator != NULL )
      {
         if ( vdseQueueRelease( pVDSQueue,
                                pQueue->iterator,
                                &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }
      
      if ( errcode == 0 )
      {
         errcode = vdseQueueRemove( pVDSQueue,
                                    &pQueue->iterator,
                                    VDSE_QUEUE_FIRST,
                                    bufferLength,
                                    &pQueue->object.pSession->context );

         if ( errcode == 0 )
         {
            *returnedLength = pQueue->iterator->dataLength;
            memcpy( buffer, pQueue->iterator->data, *returnedLength );
         }
         else
         {
            errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
         }
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsQueuePush( VDS_HANDLE   objectHandle,
                  const void * data,
                  size_t       dataLength )
{
   vdsaQueue * pQueue;
   vdseQueue * pVDSQueue;
   int errcode = 0;

   pQueue = (vdsaQueue *) objectHandle;
   if ( pQueue == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pQueue->type != VDSA_QUEUE )
      return VDS_WRONG_TYPE_HANDLE;

   if ( data == NULL )
      return VDS_NULL_POINTER;
   if ( dataLength == 0 )
      return VDS_INVALID_LENGTH;

   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      errcode = vdseQueueInsert( pVDSQueue,
                                 data,
                                 dataLength,
                                 VDSE_QUEUE_LAST,
                                 &pQueue->object.pSession->context );
      if ( errcode != 0 )
         errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );

      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
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
   if ( pQueue == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pQueue->type != VDSA_QUEUE )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pStatus == NULL )
      return VDS_NULL_POINTER;

   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;
      pContext = &pQueue->object.pSession->context;
      
      if ( vdseLock( &pVDSQueue->memObject, pContext ) == 0 )
      {
         vdseMemObjectStatus( &pVDSQueue->memObject, pStatus );

         vdseQueueStatus( pVDSQueue, pStatus );

         vdseUnlock( &pVDSQueue->memObject, pContext );
      }
      else
      {
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaQueueFirst( vdsaQueue     * pQueue,
                    vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSQueue;
   int errcode = 0;

   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pQueue->type == VDSA_QUEUE );
   
   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pQueue->iterator != NULL )
      {
         if ( vdseQueueRelease( pVDSQueue,
                                pQueue->iterator,
                                &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }
      
      if ( errcode == 0 )
      {
         errcode = vdseQueueGet( pVDSQueue,
                                 VDS_FIRST,
                                 &pQueue->iterator,
                                 (size_t) -1,
                                 &pQueue->object.pSession->context );
         if ( errcode == 0 )
         {
            pEntry->data = pQueue->iterator->data;
            pEntry->length = pQueue->iterator->dataLength;
         }
         else
         {
            errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
         }
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaQueueNext( vdsaQueue     * pQueue,
                   vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSQueue;
   int errcode = 0;

   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pEntry != NULL );
   VDS_PRE_CONDITION( pQueue->type == VDSA_QUEUE );
   VDS_PRE_CONDITION( pQueue->iterator != NULL );
   
   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      errcode = vdseQueueGet( pVDSQueue,
                              VDS_NEXT,
                              &pQueue->iterator,
                              (size_t) -1,
                              &pQueue->object.pSession->context );
      if ( errcode == 0 )
      {
         pEntry->data = pQueue->iterator->data;
         pEntry->length = pQueue->iterator->dataLength;
      }
      else
      {
         errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
         /* Reinitialize the iterator, if needed */
         if ( pQueue->iterator != NULL )
         {
            if ( vdseQueueRelease( pVDSQueue,
                                   pQueue->iterator,
                                   &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         }
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaQueueRemove( vdsaQueue     * pQueue,
                     vdsaDataEntry * pEntry )
{
   vdseQueue * pVDSQueue;
   int errcode = 0;

   VDS_PRE_CONDITION( pQueue != NULL );
   VDS_PRE_CONDITION( pEntry != NULL )
   VDS_PRE_CONDITION( pQueue->type == VDSA_QUEUE )

   if ( vdsaCommonLock( &pQueue->object ) == 0 )
   {
      pVDSQueue = (vdseQueue *) pQueue->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pQueue->iterator != NULL )
      {
         if ( vdseQueueRelease( pVDSQueue,
                                pQueue->iterator,
                                &pQueue->object.pSession->context ) == 0 )
            pQueue->iterator = NULL;
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }

      if ( errcode == 0 )
      {
         errcode = vdseQueueRemove( pVDSQueue,
                                    &pQueue->iterator,
                                    VDSE_QUEUE_FIRST,
                                    (size_t) -1,
                                    &pQueue->object.pSession->context );
         if ( errcode != 0 )
            errcode = vdscGetLastError( &pQueue->object.pSession->context.errorHandler );
         else
         {
            pEntry->data = (const void *) pQueue->iterator->data;
            pEntry->length = pQueue->iterator->dataLength;
         }
      }
      
      vdsaCommonUnlock( &pQueue->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

