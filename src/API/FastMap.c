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
#include <photon/psoFastMap.h>
#include "API/FastMap.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The API functions are first, in alphabetic order */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapClose( PSO_HANDLE objectHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL ) {
            if ( psonFastMapRelease( pMemHashMap,
                                     pHashMap->iterator.pHashItem,
                                     &pHashMap->object.pSession->context ) ) {
               memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
            }
            else {
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == PSO_OK ) {
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
   
   if ( errcode == PSO_OK ) {
      /*
       *
       */
      if ( pHashMap->editMode ) {
         pHashMap->object.pSession->numberOfEdits--;
      }
      else {
         psoaListReadersRemove( &pHashMap->object.pSession->listReaders, 
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
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                    g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapDefinition( PSO_HANDLE            objectHandle,
                          psoObjectDefinition * pDefinition,
                          unsigned char       * pKey,
                          uint32_t              keyLength,
                          unsigned char       * pFields,
                          uint32_t              fieldsLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( pKey != NULL && keyLength < pHashMap->keyDefLength ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pFields != NULL && fieldsLength < pHashMap->fieldsDefLength ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
      
         pDefinition->type = PSO_FAST_MAP;
         if ( pKey != NULL ) {
            memcpy( pKey, pHashMap->keyDef, pHashMap->keyDefLength );
         }
         pDefinition->fieldDefType = pMemHashMap->fieldDefType;
         pDefinition->keyDefType   = pMemHashMap->keyDefType;

         if ( pFields != NULL ) {
            memcpy( pFields, pHashMap->fieldsDef, pHashMap->fieldsDefLength );
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
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapDefLength( PSO_HANDLE   objectHandle, 
                         psoUint32  * keyLength,
                         psoUint32  * fieldsLength )
{
   psoaFastMap * pHashMap;
   int errcode = 0;
   psonSessionContext * pContext;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }

   pContext = &pHashMap->object.pSession->context;

   if ( keyLength == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                    g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( fieldsLength == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                    g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psoaCommonLock( &pHashMap->object ) ) {
         *keyLength = pHashMap->keyDefLength;
         *fieldsLength = pHashMap->fieldsDefLength;
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

int psoFastMapDelete( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
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
   if ( pHashMap->editMode == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                    g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

         ok = psonFastMapDelete( pMemHashMap,
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

int psoFastMapEdit( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    uint32_t     nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaFastMap * pHashMap = NULL;
   psonFastMap * pMemHashMap;
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
   
   pHashMap = (psoaFastMap *) malloc(sizeof(psoaFastMap));
   if (  pHashMap == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(psoaFastMap) );
   pHashMap->object.type = PSOA_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = psoaCommonObjOpen( &pHashMap->object,
                                   PSO_FAST_MAP,
                                   PSOA_UPDATE_RO,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pHashMap;
         pHashMap->editMode = 1;
         
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
         GET_PTR( pHashMap->fieldsDef, 
                  pMemHashMap->dataDefOffset,
                  unsigned char );
         GET_PTR( pHashMap->keyDef, 
                  pMemHashMap->keyDefOffset,
                  unsigned char );
         pHashMap->keyDefLength = pMemHashMap->keyDefLength;
         pHashMap->fieldsDefLength = pMemHashMap->dataDefLength;

//         psoaGetLimits( pHashMap->pDefinition,
//                        pMemHashMap->numFields,
//                        &pHashMap->minLength,
//                        &pHashMap->maxLength );
//         psoaGetKeyLimits( &pMemHashMap->keyDef,
//                           &pHashMap->minKeyLength,
//                           &pHashMap->maxKeyLength );
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
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( pHashMap->editMode == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   psonFastMapEmpty( pMemHashMap, &pHashMap->object.pSession->context );

   psoaCommonUnlock( &pHashMap->object );

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
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

int psoFastMapGet( PSO_HANDLE   objectHandle,
                   const void * key,
                   uint32_t     keyLength,
                   void       * buffer,
                   uint32_t     bufferLength,
                   uint32_t   * returnedLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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
   
   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;
      
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGet( pMemHashMap,
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

int psoFastMapGetFirst( PSO_HANDLE   objectHandle,
                        void       * key,
                        uint32_t     keyLength,
                        void       * buffer,
                        uint32_t     bufferLength,
                        uint32_t   * retKeyLength,
                        uint32_t   * retDataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGetFirst( pMemHashMap,
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

int psoFastMapGetNext( PSO_HANDLE   objectHandle,
                       void       * key,
                       uint32_t     keyLength,
                       void       * buffer,
                       uint32_t     bufferLength,
                       uint32_t   * retKeyLength,
                       uint32_t   * retDataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   void * ptr;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapGetNext( pMemHashMap,
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

int psoFastMapInsert( PSO_HANDLE   objectHandle,
                      const void * key,
                      uint32_t     keyLength,
                      const void * data,
                      uint32_t     dataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( dataLength == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

//   if ( keyLength < pHashMap->minKeyLength || keyLength > pHashMap->maxKeyLength ) {
//      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
//         g_psoErrorHandle, PSO_INVALID_LENGTH );
//      return PSO_INVALID_LENGTH;
//   }

//   if ( dataLength < pHashMap->minLength || dataLength > pHashMap->maxLength ) {
//      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
//         g_psoErrorHandle, PSO_INVALID_LENGTH );
//      return PSO_INVALID_LENGTH;
//   }

   if ( pHashMap->editMode == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

         ok = psonFastMapInsert( pMemHashMap,
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

int psoFastMapOpen( PSO_HANDLE   sessionHandle,
                    const char * hashMapName,
                    uint32_t     nameLengthInBytes,
                    PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaFastMap * pHashMap = NULL;
   psonFastMap * pMemHashMap;
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
   
   pHashMap = (psoaFastMap *) malloc(sizeof(psoaFastMap));
   if (  pHashMap == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pHashMap, 0, sizeof(psoaFastMap) );
   pHashMap->object.type = PSOA_MAP;
   pHashMap->object.pSession = pSession;

   if ( ! pHashMap->object.pSession->terminated ) {

      errcode = psoaCommonObjOpen( &pHashMap->object,
                                   PSO_FAST_MAP,
                                   PSOA_READ_ONLY,
                                   hashMapName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) {
         *objectHandle = (PSO_HANDLE) pHashMap;
         pHashMap->editMode = 0;
         
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
         pHashMap->reader.type = PSOA_MAP;
         pHashMap->reader.address = pHashMap;
         psoaListReadersPut( &pHashMap->object.pSession->listReaders, 
                             &pHashMap->reader );
         GET_PTR( pHashMap->fieldsDef, 
                  pMemHashMap->dataDefOffset,
                  unsigned char );
         GET_PTR( pHashMap->keyDef, 
                  pMemHashMap->keyDefOffset,
                  unsigned char );
         pHashMap->keyDefLength = pMemHashMap->keyDefLength;
         pHashMap->fieldsDefLength = pMemHashMap->dataDefLength;
//         psoaGetLimits( pHashMap->pDefinition,
//                        pMemHashMap->numFields,
//                        &pHashMap->minLength,
//                        &pHashMap->maxLength );
//         psoaGetKeyLimits( &pMemHashMap->keyDef,
//                           &pHashMap->minKeyLength,
//                           &pHashMap->maxKeyLength );
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
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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

   if ( pHashMap->editMode == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

         ok = psonFastMapReplace( pMemHashMap,
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

int psoFastMapStatus( PSO_HANDLE     objectHandle,
                      psoObjStatus * pStatus )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
      
         if ( psonLock(&pMemHashMap->memObject, pContext) ) {
            psonMemObjectStatus( &pMemHashMap->memObject, pStatus );

            psonFastMapStatus( pMemHashMap, pStatus );
            pStatus->type = PSO_FAST_MAP;

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
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFastMapFirst( psoaFastMap      * pHashMap,
                      psoaHashMapEntry * pEntry )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGetFirst( pMemHashMap,
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

int psoaFastMapNext( psoaFastMap      * pHashMap,
                     psoaHashMapEntry * pEntry )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapGetNext( pMemHashMap,
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

/*
 * This function is called when a session is committed or rollbacked.
 * The local lock on the session (if any) is already taken.
 */
void psoaFastMapResetReader( void * map )
{
   psoaFastMap * pHashMap = map;
   psonFastMap * pMemHashMap, * pMapLatest;

   psonHashTxItem * pHashItemLatest;
   psonObjectDescriptor * pDesc;

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
   pHashItemLatest = GET_PTR_FAST( pMemHashMap->latestVersion, psonHashTxItem );
   pDesc = GET_PTR_FAST( pHashItemLatest->dataOffset, 
                         psonObjectDescriptor );
   pMapLatest = GET_PTR_FAST( pDesc->offset, psonFastMap );
   if ( pMapLatest != pMemHashMap ) {
      if ( pHashMap->iterator.pHashItem != NULL ) {
         psonFastMapRelease( pMemHashMap,
                             pHashMap->iterator.pHashItem,
                             &pHashMap->object.pSession->context );
         memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
      }
      pHashMap->object.pMyMemObject = pMapLatest;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaFastMapRetrieve( psoaFastMap   * pHashMap,
                         const void    * key,
                         uint32_t        keyLength,
                         psoaDataEntry * pEntry )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psonHashItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pEntry   != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   if ( ! psoaCommonLock( &pHashMap->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler_unlock;

      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGet( pMemHashMap,
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

   return PSO_OK;

error_handler_unlock:
   psoaCommonUnlock( &pHashMap->object );

error_handler:
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

