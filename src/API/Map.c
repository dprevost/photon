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
#include <photon/psoFastMap.h>
#include "API/Map.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapClose( PSO_HANDLE objectHandle )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL ) {
            if ( psnMapRelease( pVDSHashMap,
                                 pHashMap->iterator.pHashItem,
                                 &pHashMap->object.pSession->context ) ) {
               memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
            }
            else {
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == PSO_OK ) {
            errcode = psaCommonObjClose( &pHashMap->object );
         }
         psaCommonUnlock( &pHashMap->object );
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
       *
       */
      if ( pHashMap->editMode ) {
         pHashMap->object.pSession->numberOfEdits--;
      }
      else {
         psaListReadersRemove( &pHashMap->object.pSession->listReaders, 
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
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapDefinition( PSO_HANDLE             objectHandle,
                          psoObjectDefinition ** ppDefinition )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( ppDefinition == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;
      
         errcode = psaGetDefinition( pHashMap->pDefinition,
                                      pVDSHashMap->numFields,
                                      ppDefinition );
         if ( errcode == PSO_OK ) {
            (*ppDefinition)->type = PSO_FAST_MAP;
            memcpy( &(*ppDefinition)->key, 
                    &pVDSHashMap->keyDef, 
                    sizeof(psoKeyDefinition) );
         }
         psaCommonUnlock( &pHashMap->object );
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

int psoFastMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( key == NULL ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   if ( pHashMap->editMode == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

         ok = psnMapDelete( pVDSHashMap,
                             key,
                             keyLength,
                             &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapEdit( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaMap * pHashMap = NULL;
   psnMap * pVDSHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pHashMap = (psaMap *) malloc(sizeof(psaMap));
   if (  pHashMap == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(psaMap) );
   pHashMap->object.type = PSA_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = psaCommonObjOpen( &pHashMap->object,
                                   PSO_FAST_MAP,
                                   PSA_UPDATE_RO,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pHashMap;
         pHashMap->editMode = 1;
         
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;
         GET_PTR( pHashMap->pDefinition, 
                  pVDSHashMap->dataDefOffset,
                  psnFieldDef );
         psaGetLimits( pHashMap->pDefinition,
                        pVDSHashMap->numFields,
                        &pHashMap->minLength,
                        &pHashMap->maxLength );
         psaGetKeyLimits( &pVDSHashMap->keyDef,
                           &pHashMap->minKeyLength,
                           &pHashMap->maxKeyLength );
         pSession->numberOfEdits++;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapEmpty( PSO_HANDLE objectHandle )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( pHashMap->editMode == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   psnMapEmpty( pVDSHashMap, &pHashMap->object.pSession->context );

   psaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   size_t       keyLength,
                   void       * buffer,
                   size_t       bufferLength,
                   size_t     * returnedLength )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( key == NULL || buffer == NULL || returnedLength == NULL) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }

   *returnedLength = 0;

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnMapGet( pVDSHashMap,
                    key,
                    keyLength,
                    &pHashMap->iterator.pHashItem,
                    bufferLength,
                    &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *returnedLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *returnedLength );

   psaCommonUnlock( &pHashMap->object );
   
   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapGetFirst( PSO_HANDLE   objectHandle,
                        void       * key,
                        size_t       keyLength,
                        void       * buffer,
                        size_t       bufferLength,
                        size_t     * retKeyLength,
                        size_t     * retDataLength )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || retDataLength == NULL || retKeyLength == NULL ||
      key == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *retDataLength = *retKeyLength = 0;
   
   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnMapGetFirst( pVDSHashMap,
                         &pHashMap->iterator,
                         keyLength,
                         bufferLength,
                         &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   psaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       size_t       keyLength,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * retKeyLength,
                       size_t     * retDataLength )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( buffer == NULL || retDataLength == NULL || retKeyLength == NULL ||
      key == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }
   *retDataLength = *retKeyLength = 0;

   if ( keyLength == 0 ) {
      errcode = PSO_INVALID_LENGTH;
      goto error_handler;
   }
   
   if ( pHashMap->iterator.pHashItem == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   ok = psnMapGetNext( pVDSHashMap,
                        &pHashMap->iterator,
                        keyLength,
                        bufferLength,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );

   psaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   if ( keyLength < pHashMap->minKeyLength || keyLength > pHashMap->maxKeyLength ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( dataLength < pHashMap->minLength || dataLength > pHashMap->maxLength ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( pHashMap->editMode == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

         ok = psnMapInsert( pVDSHashMap,
                             key,
                             keyLength,
                             data,
                             dataLength,
                             &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaMap * pHashMap = NULL;
   psnMap * pVDSHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pHashMap = (psaMap *) malloc(sizeof(psaMap));
   if (  pHashMap == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(psaMap) );
   pHashMap->object.type = PSA_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = psaCommonObjOpen( &pHashMap->object,
                                   PSO_FAST_MAP,
                                   PSA_READ_ONLY,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pHashMap;
         pHashMap->editMode = 0;
         
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;
         pHashMap->reader.type = PSA_MAP;
         pHashMap->reader.address = pHashMap;
         psaListReadersPut( &pHashMap->object.pSession->listReaders, 
                             &pHashMap->reader );
         GET_PTR( pHashMap->pDefinition, 
                  pVDSHashMap->dataDefOffset,
                  psnFieldDef );
         psaGetLimits( pHashMap->pDefinition,
                        pVDSHashMap->numFields,
                        &pHashMap->minLength,
                        &pHashMap->maxLength );
         psaGetKeyLimits( &pVDSHashMap->keyDef,
                           &pHashMap->minKeyLength,
                           &pHashMap->maxKeyLength );
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       size_t       keyLength,
                       const void * data,
                       size_t       dataLength )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( pHashMap->editMode == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

         ok = psnMapReplace( pVDSHashMap,
                              key,
                              keyLength,
                              data,
                              dataLength,
                              &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus )
{
   psaMap * pHashMap;
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pHashMap = (psaMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;
      
         if ( psnLock(&pVDSHashMap->memObject, pContext) ) {
            psnMemObjectStatus( &pVDSHashMap->memObject, pStatus );

            psnMapStatus( pVDSHashMap, pStatus );

            psnUnlock( &pVDSHashMap->memObject, pContext );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      
         psaCommonUnlock( &pHashMap->object );
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

int psaMapFirst( psaMap          * pHashMap,
                 psaHashMapEntry * pEntry )
{
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSA_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnMapGetFirst( pVDSHashMap,
                         &pHashMap->iterator,
                         (size_t) -1,
                         (size_t) -1,
                         &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;
   
   psaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaMapNext( psaMap          * pHashMap,
                psaHashMapEntry * pEntry )
{
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSA_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   ok = psnMapGetNext( pVDSHashMap,
                        &pHashMap->iterator,
                        (size_t) -1,
                        (size_t) -1,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;

   psaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function is called when a session is committed or rollbacked.
 * The local lock on the session (if any) is already taken.
 */
void psaMapResetReader( void * map )
{
   psaMap * pHashMap = map;
   psnMap * pVDSHashMap, * pMapLatest;

   psnHashItem * pHashItemLatest;
   psnObjectDescriptor * pDesc;

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;
   pHashItemLatest = GET_PTR_FAST( pVDSHashMap->latestVersion, psnHashItem );
   pDesc = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                         psnObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDesc->offset, psnMap );
   if ( pMapLatest != pVDSHashMap ) {
      if ( pHashMap->iterator.pHashItem != NULL ) {
         psnMapRelease( pVDSHashMap,
                         pHashMap->iterator.pHashItem,
                         &pHashMap->object.pSession->context );
         memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
      }
      pHashMap->object.pMyVdsObject = pMapLatest;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaMapRetrieve( psaMap       * pHashMap,
                    const void   * key,
                    size_t         keyLength,
                    psaDataEntry * pEntry )
{
   psnMap * pVDSHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psnHashItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSA_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnMapRelease( pVDSHashMap,
                           pHashMap->iterator.pHashItem,
                           &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnMapGet( pVDSHashMap,
                    key,
                    keyLength,
                    &pHashItem,
                    (size_t) -1,
                    &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashItem->dataOffset, void );
   pEntry->length = pHashItem->dataLength;

   psaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

