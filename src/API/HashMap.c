/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/psoHashMap.h>
#include "API/HashMap.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapClose( PSO_HANDLE objectHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL ) {
            if ( psonHashMapRelease( pMemHashMap,
                                     pHashMap->iterator.pHashItem,
                                     &pHashMap->object.pSession->context ) ) {
               memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
            }
            else {
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == 0 ) {
            errcode = psoaCommonObjClose( &pHashMap->object );
         }
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
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
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapDefinition( PSO_HANDLE            objectHandle,
                          psoBasicObjectDef * pDefinition,
                          psoUint32             numFields,
                          psoFieldDefinition  * pFields )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   psonSessionContext * pContext;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( numFields > 0 && pFields == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
      
         pDefinition->type = PSO_HASH_MAP;
         memcpy( &pDefinition->key, 
                 &pMemHashMap->keyDef, 
                 sizeof(psoKeyDefinition) );
         pDefinition->numFields = pMemHashMap->numFields;
         
         if ( numFields > 0 ) {
            errcode = psoaGetDefinition( pHashMap->pDefinition,
                                         pMemHashMap->numFields,
                                         pFields );
         }
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( key == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

         ok = psonHashMapDelete( pMemHashMap,
                                 key,
                                 keyLength,
                                 &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   uint32_t     keyLength,
                   void       * buffer,
                   uint32_t     bufferLength,
                   uint32_t   * returnedLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGet( pMemHashMap,
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

   psoaCommonUnlock( &pHashMap->object );
   
   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapGetFirst( PSO_HANDLE   objectHandle,
                        void       * key,
                        uint32_t     keyLength,
                        void       * buffer,
                        uint32_t     bufferLength,
                        uint32_t   * retKeyLength,
                        uint32_t   * retDataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   void * ptr;
   bool ok;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGetFirst( pMemHashMap,
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

   psoaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       uint32_t     keyLength,
                       void       * buffer,
                       uint32_t     bufferLength,
                       uint32_t   * retKeyLength,
                       uint32_t   * retDataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   void * ptr;
   bool ok;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   ok = psonHashMapGetNext( pMemHashMap,
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

   psoaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength,
                      const void * data,
                      uint32_t     dataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   bool ok = true;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyLength == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   if ( keyLength < pHashMap->minKeyLength || keyLength > pHashMap->maxKeyLength ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( dataLength < pHashMap->minLength || dataLength > pHashMap->maxLength ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

         ok = psonHashMapInsert( pMemHashMap,
                                 key,
                                 keyLength,
                                 data,
                                 dataLength,
                                 &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    uint32_t     nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaHashMap * pHashMap = NULL;
   psonHashMap * pMemHashMap;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( hashMapName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pHashMap = (psoaHashMap *) malloc(sizeof(psoaHashMap));
   if (  pHashMap == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(psoaHashMap) );
   pHashMap->object.type = PSOA_HASH_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = psoaCommonObjOpen( &pHashMap->object,
                                   PSO_HASH_MAP,
                                   PSOA_READ_WRITE,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == 0 ) {
         *objectHandle = (PSO_HANDLE) pHashMap;
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
         GET_PTR( pHashMap->pDefinition, 
                  pMemHashMap->dataDefOffset,
                  psonFieldDef );
         psoaGetLimits( pHashMap->pDefinition,
                        pMemHashMap->numFields,
                        &pHashMap->minLength,
                        &pHashMap->maxLength );
         psoaGetKeyLimits( &pMemHashMap->keyDef,
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

int psoHashMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( key == NULL || data == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyLength == 0 || dataLength == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

         ok = psonHashMapReplace( pMemHashMap,
                                  key,
                                  keyLength,
                                  data,
                                  dataLength,
                                  &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   psonSessionContext * pContext;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
      
         if ( psonLock(&pMemHashMap->memObject, pContext) ) {
            psonMemObjectStatus( &pMemHashMap->memObject, pStatus );

            psonHashMapStatus( pMemHashMap, pStatus );

            psonUnlock( &pMemHashMap->memObject, pContext );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaHashMapFirst( psoaHashMap      * pHashMap,
                      psoaHashMapEntry * pEntry )
{
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGetFirst( pMemHashMap,
                             &pHashMap->iterator,
                             (uint32_t) -1,
                             (uint32_t) -1,
                             &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;
   
   psoaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaHashMapNext( psoaHashMap      * pHashMap,
                     psoaHashMapEntry * pEntry )
{
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   ok = psonHashMapGetNext( pMemHashMap,
                            &pHashMap->iterator,
                            (uint32_t) -1,
                            (uint32_t) -1,
                            &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
   pEntry->dataLength = pHashMap->iterator.pHashItem->dataLength;
   pEntry->keyLength = pHashMap->iterator.pHashItem->keyLength;
   pEntry->key = pHashMap->iterator.pHashItem->key;

   psoaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaHashMapRetrieve( psoaHashMap  * pHashMap,
                        const void    * key,
                        uint32_t        keyLength,
                        psoaDataEntry * pEntry )
{
   psonHashMap * pMemHashMap;
   int errcode = 0;
   bool ok = true;
   psonHashTxItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGet( pMemHashMap,
                        key,
                        keyLength,
                        &pHashItem,
                        (uint32_t) -1,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   GET_PTR( pEntry->data, pHashItem->dataOffset, void );
   pEntry->length = pHashItem->dataLength;

   psoaCommonUnlock( &pHashMap->object );

   return 0;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
   if ( errcode != 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pHashMap->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

