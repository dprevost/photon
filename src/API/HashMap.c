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
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapClose( VDS_HANDLE objectHandle )
{
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL ) {
            if ( psnHashMapRelease( pVDSHashMap,
                                     pHashMap->iterator.pHashItem,
                                     &pHashMap->object.pSession->context ) ) {
               memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
            }
            else {
               errcode = VDS_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == 0 ) {
            errcode = psaCommonObjClose( &pHashMap->object );
         }
         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode == 0 ) {
      /*
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pHashMap->object.type = 0; 
      free( pHashMap );
   }
   else {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapDefinition( VDS_HANDLE             objectHandle,
                          vdsObjectDefinition ** ppDefinition )
{
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   psnSessionContext * pContext;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( ppDefinition == NULL ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;
      
         errcode = psaGetDefinition( pHashMap->pDefinition,
                                      pVDSHashMap->numFields,
                                      ppDefinition );
         if ( errcode == 0 ) {
            (*ppDefinition)->type = VDS_HASH_MAP;
            memcpy( &(*ppDefinition)->key, 
                    &pVDSHashMap->keyDef, 
                    sizeof(vdsKeyDefinition) );
         }
         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength )
{
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( key == NULL ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

         ok = psnHashMapDelete( pVDSHashMap,
                                 key,
                                 keyLength,
                                 &pHashMap->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
         
         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != VDS_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
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
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

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

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnHashMapGet( pVDSHashMap,
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

   psaCommonUnlock( &pHashMap->object );
   
   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   } 
   else {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
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
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   void * ptr;
   bool ok;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

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

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnHashMapGetFirst( pVDSHashMap,
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

   psaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
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
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   void * ptr;
   bool ok;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

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

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

   ok = psnHashMapGetNext( pVDSHashMap,
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

   psaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapInsert( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength,
                      const void * data,
                      size_t       dataLength )
{
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   bool ok = true;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   if ( keyLength < pHashMap->minKeyLength || keyLength > pHashMap->maxKeyLength ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( dataLength < pHashMap->minLength || dataLength > pHashMap->maxLength ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

         ok = psnHashMapInsert( pVDSHashMap,
                                 key,
                                 keyLength,
                                 data,
                                 dataLength,
                                 &pHashMap->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapOpen( VDS_HANDLE   sessionHandle,
                    const char * hashMapName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaHashMap * pHashMap = NULL;
   psnHashMap * pVDSHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pHashMap = (psaHashMap *) malloc(sizeof(psaHashMap));
   if (  pHashMap == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(psaHashMap) );
   pHashMap->object.type = PSA_HASH_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = psaCommonObjOpen( &pHashMap->object,
                                   VDS_HASH_MAP,
                                   PSA_READ_WRITE,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == 0 ) {
         *objectHandle = (VDS_HANDLE) pHashMap;
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;
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
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapReplace( VDS_HANDLE   objectHandle,
                       const void * key,
                       size_t       keyLength,
                       const void * data,
                       size_t       dataLength )
{
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok = true;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

         ok = psnHashMapReplace( pVDSHashMap,
                                  key,
                                  keyLength,
                                  data,
                                  dataLength,
                                  &pHashMap->object.pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapStatus( VDS_HANDLE     objectHandle,
                      vdsObjStatus * pStatus )
{
   psaHashMap * pHashMap;
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   psnSessionContext * pContext;
   
   pHashMap = (psaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return VDS_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSA_HASH_MAP ) return VDS_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psaCommonLock( &pHashMap->object ) ) {
         pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;
      
         if ( psnLock(&pVDSHashMap->memObject, pContext) ) {
            psnMemObjectStatus( &pVDSHashMap->memObject, pStatus );

            psnHashMapStatus( pVDSHashMap, pStatus );

            psnUnlock( &pVDSHashMap->memObject, pContext );
         }
         else {
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         }
      
         psaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaHashMapFirst( psaHashMap      * pHashMap,
                     psaHashMapEntry * pEntry )
{
   psnHashMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->object.type == PSA_HASH_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnHashMapGetFirst( pVDSHashMap,
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
   
   psaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaHashMapNext( psaHashMap      * pHashMap,
                    psaHashMapEntry * pEntry )
{
   psnHashMap * pVDSHashMap;
   int errcode = VDS_OK;
   bool ok;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->object.type == PSA_HASH_MAP );
   VDS_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

   ok = psnHashMapGetNext( pVDSHashMap,
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

   psaCommonUnlock( &pHashMap->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psaHashMapRetrieve( psaHashMap   * pHashMap,
                        const void   * key,
                        size_t         keyLength,
                        psaDataEntry * pEntry )
{
   psnHashMap * pVDSHashMap;
   int errcode = 0;
   bool ok = true;
   psnHashItem * pHashItem;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( key      != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->object.type == PSA_HASH_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psaCommonLock( &pHashMap->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSHashMap = (psnHashMap *) pHashMap->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psnHashMapRelease( pVDSHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psnHashMapItem) );
   }

   ok = psnHashMapGet( pVDSHashMap,
                        key,
                        keyLength,
                        &pHashItem,
                        (size_t) -1,
                        &pHashMap->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashItem->dataOffset, void );
   pEntry->length = pHashItem->dataLength;

   psaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   psaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      pscSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

