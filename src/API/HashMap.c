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
#include <vdsf/vdsHashMap.h>
#include "API/HashMap.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapClose( VDS_HANDLE objectHandle )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 ) {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pHashMap->iterator.pHashItem != NULL ) {
         if ( vdseHashMapRelease( pVDSHashMap,
                                  pHashMap->iterator.pHashItem,
                                  &pHashMap->object.pSession->context ) == 0 )
            memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }

      if ( errcode == 0 )
         errcode = vdsaCommonObjClose( &pHashMap->object );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode == 0 )
      free( pHashMap );
   else
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 ) {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      rc = vdseHashMapDelete( pVDSHashMap,
                              key,
                              keyLength,
                              &pHashMap->object.pSession->context );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapGet( VDS_HANDLE   objectHandle,
                   const void * key,
                   size_t       keyLength,
                   void       * buffer,
                   size_t       bufferLength,
                   size_t     * returnedLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;
   void * ptr;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || buffer == NULL || returnedLength == NULL) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 ) {
      errcode = VDS_INVALID_LENGTH;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( vdsaCommonLock( &pHashMap->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      rc = vdseHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      if ( rc != 0 )
         goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   rc = vdseHashMapGet( pVDSHashMap,
                        key,
                        keyLength,
                        &pHashMap->iterator.pHashItem,
                        bufferLength,
                        &pHashMap->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;

   *returnedLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *returnedLength );

   vdsaCommonUnlock( &pHashMap->object );
   
   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapGetFirst( VDS_HANDLE   objectHandle,
                        void       * key,
                        size_t       keyLength,
                        void       * buffer,
                        size_t       bufferLength,
                        size_t     * retKeyLength,
                        size_t     * retDataLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;
   void * ptr;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || retDataLength == NULL || retKeyLength == NULL ||
      key == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   *retDataLength = *retKeyLength = 0;
   
   if ( keyLength == 0 ) {
      errcode = VDS_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( vdsaCommonLock( &pHashMap->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      rc = vdseHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      if ( rc != 0 )
         goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   rc = vdseHashMapGetFirst( pVDSHashMap,
                             &pHashMap->iterator,
                             keyLength,
                             bufferLength,
                             &pHashMap->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;

   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   vdsaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapGetNext( VDS_HANDLE   objectHandle,
                       void       * key,
                       size_t       keyLength,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * retKeyLength,
                       size_t     * retDataLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;
   void * ptr;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || retDataLength == NULL || retKeyLength == NULL ||
      key == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   *retDataLength = *retKeyLength = 0;

   if ( keyLength == 0 ) {
      errcode = VDS_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pHashMap->iterator.pHashItem == NULL ) {
      errcode = VDS_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( vdsaCommonLock( &pHashMap->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

   rc = vdseHashMapGetNext( pVDSHashMap,
                            &pHashMap->iterator,
                            keyLength,
                            bufferLength,
                            &pHashMap->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;
   
   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   vdsaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapInsert( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( vdsaCommonLock( &pHashMap->object ) == 0 ) {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      rc = vdseHashMapInsert( pVDSHashMap,
                              key,
                              keyLength,
                              data,
                              dataLength,
                              &pHashMap->object.pSession->context );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapOpen( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaHashMap * pHashMap = NULL;
   int errcode;
   
   if ( objectHandle == NULL )
      return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pHashMap = (vdsaHashMap *) malloc(sizeof(vdsaHashMap));
   if (  pHashMap == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(vdsaHashMap) );
   pHashMap->object.type = VDSA_HASH_MAP;
   pHashMap->object.pSession = pSession;

   errcode = vdsaCommonObjOpen( &pHashMap->object,
                                VDS_FOLDER,
                                hashMapName,
                                nameLengthInBytes );
   if ( errcode == 0 )
      *objectHandle = (VDS_HANDLE) pHashMap;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapReplace( VDS_HANDLE   objectHandle,
                       const void * key,
                       size_t       keyLength,
                       const void * data,
                       size_t       dataLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( vdsaCommonLock( &pHashMap->object ) == 0 ) {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      rc = vdseHashMapReplace( pVDSHashMap,
                               key,
                               keyLength,
                               data,
                               dataLength,
                               &pHashMap->object.pSession->context );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapStatus( VDS_HANDLE     objectHandle,
                      vdsObjStatus * pStatus )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;
   vdseSessionContext * pContext;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( vdsaCommonLock( &pHashMap->object ) == 0 ) {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;
      
      if ( vdseLock( &pVDSHashMap->memObject, pContext ) == 0 ) {
         vdseMemObjectStatus( &pVDSHashMap->memObject, pStatus );

         vdseHashMapStatus( pVDSHashMap, pStatus );

         vdseUnlock( &pVDSHashMap->memObject, pContext );
      }
      else
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      
      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != 0 )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaHashMapFirst( vdsaHashMap      * pHashMap,
                      vdsaHashMapEntry * pEntry )
{
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->object.type == VDSA_HASH_MAP );
   
   if ( vdsaCommonLock( &pHashMap->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      rc = vdseHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      
      if ( rc != 0 )
         goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   rc = vdseHashMapGetFirst( pVDSHashMap,
                             &pHashMap->iterator,
                             (size_t) -1,
                             (size_t) -1,
                             &pHashMap->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;
   
   vdsaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 )
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaHashMapNext( vdsaHashMap   * pHashMap,
                     vdsaHashMapEntry * pEntry )
{
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->object.type == VDSA_HASH_MAP );
   VDS_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( vdsaCommonLock( &pHashMap->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

   rc = vdseHashMapGetNext( pVDSHashMap,
                            &pHashMap->iterator,
                            (size_t) -1,
                            (size_t) -1,
                            &pHashMap->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;
   
   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;

   vdsaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaHashMapRetrieve( vdsaHashMap   * pHashMap,
                         const void    * key,
                         size_t          keyLength,
                         vdsaDataEntry * pEntry )
{
   vdseHashMap * pVDSHashMap;
   int errcode = 0, rc = 0;
   vdseHashItem * pHashItem;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( key      != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->object.type == VDSA_HASH_MAP );

   if ( vdsaCommonLock( &pHashMap->object ) != 0 ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      rc = vdseHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      if ( rc != 0 )
         goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   rc = vdseHashMapGet( pVDSHashMap,
                        key,
                        keyLength,
                        &pHashItem,
                        (size_t) -1,
                        &pHashMap->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashItem->dataOffset, void );
   pEntry->length = pHashItem->dataLength;

   vdsaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( rc != 0 ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

