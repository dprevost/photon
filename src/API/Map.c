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
#include <vdsf/vdsFastMap.h>
#include "API/Map.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapClose( VDS_HANDLE objectHandle )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( vdsaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL ) {
            if ( vdseMapRelease( pVDSHashMap,
                                 pHashMap->iterator.pHashItem,
                                 &pHashMap->object.pSession->context ) ) {
               memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
            }
            else {
               errcode = VDS_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == VDS_OK ) {
            errcode = vdsaCommonObjClose( &pHashMap->object );
         }
         vdsaCommonUnlock( &pHashMap->object );
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
       *
       */
      if ( pHashMap->editMode ) {
         pHashMap->object.pSession->numberOfEdits--;
      }
      else {
         vdsaListReadersRemove( &pHashMap->object.pSession->listReaders, 
                                &pHashMap->reader );
      }
      /*
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pHashMap->object.type = 0; 
      free( pHashMap );
   }
   else {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapDefinition( VDS_HANDLE             objectHandle,
                          vdsObjectDefinition ** ppDefinition )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   vdseSessionContext * pContext;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( ppDefinition == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( vdsaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;
      
         errcode = vdsaGetDefinition( pHashMap->pDefinition,
                                      pVDSHashMap->numFields,
                                      ppDefinition );
         if ( errcode == VDS_OK ) {
            (*ppDefinition)->type = VDS_FAST_MAP;
            memcpy( &(*ppDefinition)->key, 
                    &pVDSHashMap->keyDef, 
                    sizeof(vdsKeyDefinition) );
         }
         vdsaCommonUnlock( &pHashMap->object );
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

int vdsFastMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;

   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
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
   if ( pHashMap->editMode == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_OBJECT_IS_READ_ONLY );
      return VDS_OBJECT_IS_READ_ONLY;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( vdsaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

         ok = vdseMapDelete( pVDSHashMap,
                             key,
                             keyLength,
                             &pHashMap->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );

         vdsaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapEdit( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaMap * pHashMap = NULL;
   vdseMap * pVDSHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pHashMap = (vdsaMap *) malloc(sizeof(vdsaMap));
   if (  pHashMap == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(vdsaMap) );
   pHashMap->object.type = VDSA_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = vdsaCommonObjOpen( &pHashMap->object,
                                   VDS_FAST_MAP,
                                   VDSA_UPDATE_RO,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == VDS_OK ) {
         *objectHandle = (VDS_HANDLE) pHashMap;
         pHashMap->editMode = 1;
         
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;
         GET_PTR( pHashMap->pDefinition, 
                  pVDSHashMap->dataDefOffset,
                  vdseFieldDef );
         vdsaGetLimits( pHashMap->pDefinition,
                        pVDSHashMap->numFields,
                        &pHashMap->minLength,
                        &pHashMap->maxLength );
         vdsaGetKeyLimits( &pVDSHashMap->keyDef,
                           &pHashMap->minKeyLength,
                           &pHashMap->maxKeyLength );
         pSession->numberOfEdits++;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapEmpty( VDS_HANDLE objectHandle )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( pHashMap->editMode == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_OBJECT_IS_READ_ONLY );
      return VDS_OBJECT_IS_READ_ONLY;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = vdseMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   vdseMapEmpty( pVDSHashMap, &pHashMap->object.pSession->context );

   vdsaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapGet( VDS_HANDLE   objectHandle,
                   const void * key,
                   size_t       keyLength,
                   void       * buffer,
                   size_t       bufferLength,
                   size_t     * returnedLength )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || buffer == NULL || returnedLength == NULL) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 ) {
      errcode = VDS_INVALID_LENGTH;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = vdseMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   ok = vdseMapGet( pVDSHashMap,
                    key,
                    keyLength,
                    &pHashMap->iterator.pHashItem,
                    bufferLength,
                    &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *returnedLength );

   vdsaCommonUnlock( &pHashMap->object );
   
   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapGetFirst( VDS_HANDLE   objectHandle,
                        void       * key,
                        size_t       keyLength,
                        void       * buffer,
                        size_t       bufferLength,
                        size_t     * retKeyLength,
                        size_t     * retDataLength )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

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
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = vdseMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   ok = vdseMapGetFirst( pVDSHashMap,
                         &pHashMap->iterator,
                         keyLength,
                         bufferLength,
                         &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   vdsaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapGetNext( VDS_HANDLE   objectHandle,
                       void       * key,
                       size_t       keyLength,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * retKeyLength,
                       size_t     * retDataLength )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

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
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   ok = vdseMapGetNext( pVDSHashMap,
                        &pHashMap->iterator,
                        keyLength,
                        bufferLength,
                        &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   vdsaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapInsert( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;

   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   if ( keyLength < pHashMap->minKeyLength || keyLength > pHashMap->maxKeyLength ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( dataLength < pHashMap->minLength || dataLength > pHashMap->maxLength ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( pHashMap->editMode == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_OBJECT_IS_READ_ONLY );
      return VDS_OBJECT_IS_READ_ONLY;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( vdsaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

         ok = vdseMapInsert( pVDSHashMap,
                             key,
                             keyLength,
                             data,
                             dataLength,
                             &pHashMap->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );

         vdsaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapOpen( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaMap * pHashMap = NULL;
   vdseMap * pVDSHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pHashMap = (vdsaMap *) malloc(sizeof(vdsaMap));
   if (  pHashMap == NULL ) {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(vdsaMap) );
   pHashMap->object.type = VDSA_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = vdsaCommonObjOpen( &pHashMap->object,
                                   VDS_FAST_MAP,
                                   VDSA_READ_ONLY,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == VDS_OK ) {
         *objectHandle = (VDS_HANDLE) pHashMap;
         pHashMap->editMode = 0;
         
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;
         pHashMap->reader.type = VDSA_MAP;
         pHashMap->reader.address = pHashMap;
         vdsaListReadersPut( &pHashMap->object.pSession->listReaders, 
                             &pHashMap->reader );
         GET_PTR( pHashMap->pDefinition, 
                  pVDSHashMap->dataDefOffset,
                  vdseFieldDef );
         vdsaGetLimits( pHashMap->pDefinition,
                        pVDSHashMap->numFields,
                        &pHashMap->minLength,
                        &pHashMap->maxLength );
         vdsaGetKeyLimits( &pVDSHashMap->keyDef,
                           &pHashMap->minKeyLength,
                           &pHashMap->maxKeyLength );
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapReplace( VDS_HANDLE   objectHandle,
                       const void * key,
                       size_t       keyLength,
                       const void * data,
                       size_t       dataLength )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;

   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

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

   if ( pHashMap->editMode == 0 ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_OBJECT_IS_READ_ONLY );
      return VDS_OBJECT_IS_READ_ONLY;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( vdsaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

         ok = vdseMapReplace( pVDSHashMap,
                              key,
                              keyLength,
                              data,
                              dataLength,
                              &pHashMap->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );

         vdsaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFastMapStatus( VDS_HANDLE     objectHandle,
                      vdsObjStatus * pStatus )
{
   vdsaMap * pHashMap;
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   vdseSessionContext * pContext;
   
   pHashMap = (vdsaMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != VDSA_MAP ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( vdsaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;
      
         if ( vdseLock(&pVDSHashMap->memObject, pContext) ) {
            vdseMemObjectStatus( &pVDSHashMap->memObject, pStatus );

            vdseMapStatus( pVDSHashMap, pStatus );

            vdseUnlock( &pVDSHashMap->memObject, pContext );
         }
         else {
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         }
      
         vdsaCommonUnlock( &pHashMap->object );
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

int vdsaMapFirst( vdsaMap          * pHashMap,
                  vdsaHashMapEntry * pEntry )
{
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->object.type == VDSA_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = vdseMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   ok = vdseMapGetFirst( pVDSHashMap,
                         &pHashMap->iterator,
                         (size_t) -1,
                         (size_t) -1,
                         &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;
   
   vdsaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaMapNext( vdsaMap          * pHashMap,
                 vdsaHashMapEntry * pEntry )
{
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->object.type == VDSA_MAP );
   VDS_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   ok = vdseMapGetNext( pVDSHashMap,
                        &pHashMap->iterator,
                        (size_t) -1,
                        (size_t) -1,
                        &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;

   vdsaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function is called when a session is committed or rollbacked.
 * The local lock on the session (if any) is already taken.
 */
void vdsaMapResetReader( void * map )
{
   vdsaMap * pHashMap = map;
   vdseMap * pVDSHashMap, * pMapLatest;

   vdseHashItem * pHashItemLatest;
   vdseObjectDescriptor * pDesc;

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;
   pHashItemLatest = GET_PTR_FAST( pVDSHashMap->latestVersion, vdseHashItem );
   pDesc = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                         vdseObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDesc->offset, vdseMap );
   if ( pMapLatest != pVDSHashMap ) {
      if ( pHashMap->iterator.pHashItem != NULL ) {
         vdseMapRelease( pVDSHashMap,
                         pHashMap->iterator.pHashItem,
                         &pHashMap->object.pSession->context );
         memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
      }
      pHashMap->object.pMyVdsObject = pMapLatest;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaMapRetrieve( vdsaMap       * pHashMap,
                     const void    * key,
                     size_t          keyLength,
                     vdsaDataEntry * pEntry )
{
   vdseMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   vdseHashItem * pHashItem;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( key      != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->object.type == VDSA_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! vdsaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (vdseMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = vdseMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
   }

   ok = vdseMapGet( pVDSHashMap,
                    key,
                    keyLength,
                    &pHashItem,
                    (size_t) -1,
                    &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashItem->dataOffset, void );
   pEntry->length = pHashItem->dataLength;

   vdsaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   vdsaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      vdscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

