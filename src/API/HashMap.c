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
#include <photon/HashMap.h>
#include "API/HashMap.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

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
      if ( errcode == PSO_OK ) {
         if ( pHashMap->pRecordDefinition != NULL ) {
            pHashMap->pRecordDefinition->ppApiObject = NULL;
            pHashMap->pRecordDefinition = NULL;
         }
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

int psoHashMapDefinition( PSO_HANDLE   objectHandle,
                          PSO_HANDLE * keyDefHandle,
                          PSO_HANDLE * dataDefHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   psonSessionContext * pContext;
   psoaDataDefinition * pDataDefinition = NULL;
   psoaKeyDefinition  * pKeyDefinition = NULL;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   pContext = &pHashMap->object.pSession->context;

   if ( keyDefHandle == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataDefHandle == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   pDataDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDataDefinition == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   pKeyDefinition = malloc( sizeof(psoaKeyDefinition) );
   if ( pKeyDefinition == NULL ) {
      free( pDataDefinition );
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( ! pHashMap->object.pSession->terminated ) {
      pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
      
      pDataDefinition->pSession = pHashMap->object.pSession;
      pDataDefinition->definitionType = PSOA_DEF_DATA;
      GET_PTR( pDataDefinition->pMemDefinition, 
               pMemHashMap->dataDefOffset, 
               psonDataDefinition );
      *dataDefHandle = (PSO_HANDLE) pDataDefinition;

      pKeyDefinition->pSession = pHashMap->object.pSession;
      pKeyDefinition->definitionType = PSOA_DEF_KEY;
      GET_PTR( pKeyDefinition->pMemDefinition, 
               pMemHashMap->keyDefOffset, 
               psonKeyDefinition );
      *keyDefHandle = (PSO_HANDLE) pKeyDefinition;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != 0 ) {
      if ( pDataDefinition ) free(pDataDefinition);
      if ( pKeyDefinition) free(pKeyDefinition);
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

      pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

      if ( pMemHashMap->isSystemObject ) {
         errcode = PSO_SYSTEM_OBJECT;
      }
      else {
         ok = psonHashMapDelete( pMemHashMap,
                                 key,
                                 keyLength,
                                 &pHashMap->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
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

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
      
      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGet( pMemHashMap,
                        key,
                        keyLength,
                        &pHashMap->iterator.pHashItem,
                        bufferLength,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *returnedLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *returnedLength );
   if ( pHashMap->pRecordDefinition != NULL ) {
      GET_PTR( pHashMap->pRecordDefinition->pMemDefinition, 
               pHashMap->iterator.pHashItem->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGetFirst( pMemHashMap,
                             &pHashMap->iterator,
                             keyLength,
                             bufferLength,
                             &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );
   if ( pHashMap->pRecordDefinition != NULL ) {
      GET_PTR( pHashMap->pRecordDefinition->pMemDefinition, 
               pHashMap->iterator.pHashItem->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   ok = psonHashMapGetNext( pMemHashMap,
                            &pHashMap->iterator,
                            keyLength,
                            bufferLength,
                            &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   *retDataLength = pHashMap->iterator.pHashItem->dataLength;
   GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
   memcpy( buffer, ptr, *retDataLength );
   *retKeyLength = pHashMap->iterator.pHashItem->keyLength;
   memcpy( key, pHashMap->iterator.pHashItem->key, *retKeyLength );
   if ( pHashMap->pRecordDefinition != NULL ) {
      GET_PTR( pHashMap->pRecordDefinition->pMemDefinition, 
               pHashMap->iterator.pHashItem->dataDefOffset, 
               psonDataDefinition );
   }

   return 0;

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
                      uint32_t     dataLength,
                      PSO_HANDLE   dataDefHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = 0;
   bool ok = true;
   psoaDataDefinition * pDefinition = NULL;
   psonDataDefinition * pMemDefinition = NULL;
   psonDataDefinition * pDef;
   
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

   if ( dataLength == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( dataDefHandle != NULL ) {
      pDefinition = (psoaDataDefinition *) dataDefHandle;
   
      if ( pDefinition->definitionType != PSOA_DEF_DATA )  {
         psocSetError( &pHashMap->object.pSession->context.errorHandler, 
            g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
         return PSO_WRONG_TYPE_HANDLE;
      }
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

      if ( pDefinition != NULL ) {
         if ( !(pMemHashMap->flags & PSO_MULTIPLE_DATA_DEFINITIONS) ) {
            errcode = PSO_DATA_DEF_UNSUPPORTED;
         }
         else {
            pDef = GET_PTR_FAST( pMemHashMap->dataDefOffset, psonDataDefinition );
            if ( pDefinition->pMemDefinition->type != pDef->type ) {
               errcode = PSO_INVALID_DATA_DEFINITION_TYPE;
            }
         }
         pMemDefinition = pDefinition->pMemDefinition;
      }

      if ( errcode == PSO_OK ) {
         if ( pMemHashMap->isSystemObject ) {
            errcode = PSO_SYSTEM_OBJECT;
         }
         else {
            ok = psonHashMapInsert( pMemHashMap,
                                    key,
                                    keyLength,
                                    data,
                                    dataLength,
                                    pMemDefinition,
                                    &pHashMap->object.pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
         }
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
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
      free(pHashMap);
   }
   else {
      *objectHandle = (PSO_HANDLE) pHashMap;
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapRecordDefinition( PSO_HANDLE   objectHandle,
                                PSO_HANDLE * dataDefHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   psoaDataDefinition * pDefinition = NULL;
   
   pHashMap = (psoaHashMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_HASH_MAP ) return PSO_WRONG_TYPE_HANDLE;

   if ( dataDefHandle == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler,
                    g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   pDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDefinition == NULL ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler,
                    g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   pDefinition->pSession = pHashMap->object.pSession;
   pDefinition->definitionType = PSOA_DEF_DATA;
   
   if ( ! pHashMap->object.pSession->terminated ) {
      pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
 
      /* We use the global queue definition as the initial value
       * to avoid an uninitialized pointer.
       */
      GET_PTR( pDefinition->pMemDefinition, 
               pMemHashMap->dataDefOffset, 
               psonDataDefinition );
      pDefinition->ppApiObject = &pHashMap->pRecordDefinition;
      pHashMap->pRecordDefinition = pDefinition;
         
      *dataDefHandle = (PSO_HANDLE) pDefinition;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
                    g_psoErrorHandle, errcode );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoHashMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength,
                       PSO_HANDLE   dataDefHandle )
{
   psoaHashMap * pHashMap;
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psoaDataDefinition * pDefinition = NULL;
   psonDataDefinition * pMemDefinition = NULL;
   psonDataDefinition * pDef;
   
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

   if ( dataDefHandle != NULL ) {
      pDefinition = (psoaDataDefinition *) dataDefHandle;
   
      if ( pDefinition->definitionType != PSOA_DEF_DATA )  {
         psocSetError( &pHashMap->object.pSession->context.errorHandler, 
            g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
         return PSO_WRONG_TYPE_HANDLE;
      }
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

      if ( pDefinition != NULL ) {
         if ( !(pMemHashMap->flags & PSO_MULTIPLE_DATA_DEFINITIONS) ) {
            errcode = PSO_DATA_DEF_UNSUPPORTED;
         }
         else {
            pDef = GET_PTR_FAST( pMemHashMap->dataDefOffset, psonDataDefinition );
            if ( pDefinition->pMemDefinition->type != pDef->type ) {
               errcode = PSO_INVALID_DATA_DEFINITION_TYPE;
            }
         }
         pMemDefinition = pDefinition->pMemDefinition;
      }

      if ( errcode == PSO_OK ) {
         if ( pMemHashMap->isSystemObject ) {
            errcode = PSO_SYSTEM_OBJECT;
         }
         else {
            ok = psonHashMapReplace( pMemHashMap,
                                     key,
                                     keyLength,
                                     data,
                                     dataLength,
                                     pMemDefinition,
                                     &pHashMap->object.pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
         }
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

      pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;
      
      if ( psonLock(&pMemHashMap->memObject, pContext) ) {
         psonMemObjectStatus( &pMemHashMap->memObject, pStatus );

         psonHashMapStatus( pMemHashMap, pStatus );
         pStatus->type = PSO_HASH_MAP;
            
         psonUnlock( &pMemHashMap->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
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

int psoaHashMapFirst( psoaHashMap    * pHashMap,
                      unsigned char ** pKey,
                      uint32_t       * pKeyLength,
                      unsigned char ** pData,
                      uint32_t       * pDataLength )
{
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pHashMap    != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGetFirst( pMemHashMap,
                             &pHashMap->iterator,
                             (uint32_t) -1,
                             (uint32_t) -1,
                             &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   GET_PTR( *pData, pHashMap->iterator.pHashItem->dataOffset, void );
   *pDataLength = pHashMap->iterator.pHashItem->dataLength;
   *pKeyLength = pHashMap->iterator.pHashItem->keyLength;
   *pKey = pHashMap->iterator.pHashItem->key;
   if ( pHashMap->pRecordDefinition != NULL ) {
      GET_PTR( pHashMap->pRecordDefinition->pMemDefinition, 
               pHashMap->iterator.pHashItem->dataDefOffset, 
               psonDataDefinition );
   }
   
   return 0;

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

int psoaHashMapNext( psoaHashMap    * pHashMap,
                     unsigned char ** pKey,
                     uint32_t       * pKeyLength,
                     unsigned char ** pData,
                     uint32_t       * pDataLength )
{
   psonHashMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok;
   
   PSO_PRE_CONDITION( pHashMap    != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   ok = psonHashMapGetNext( pMemHashMap,
                            &pHashMap->iterator,
                            (uint32_t) -1,
                            (uint32_t) -1,
                            &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   GET_PTR( *pData, pHashMap->iterator.pHashItem->dataOffset, void );
   *pDataLength = pHashMap->iterator.pHashItem->dataLength;
   *pKeyLength = pHashMap->iterator.pHashItem->keyLength;
   *pKey = pHashMap->iterator.pHashItem->key;
   if ( pHashMap->pRecordDefinition != NULL ) {
      GET_PTR( pHashMap->pRecordDefinition->pMemDefinition, 
               pHashMap->iterator.pHashItem->dataDefOffset, 
               psonDataDefinition );
   }

   return PSO_OK;

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

int psoaHashMapRetrieve( psoaHashMap    * pHashMap,
                         const void     * key,
                         uint32_t         keyLength,
                         unsigned char ** pData,
                         uint32_t       * pLength )
{
   psonHashMap * pMemHashMap;
   int errcode = 0;
   bool ok = true;
   psonHashTxItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pData    != NULL );
   PSO_PRE_CONDITION( pLength  != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_HASH_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonHashMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonHashMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonHashMapItem) );
   }

   ok = psonHashMapGet( pMemHashMap,
                        key,
                        keyLength,
                        &pHashItem,
                        (uint32_t) -1,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   GET_PTR( *pData, pHashItem->dataOffset, void );
   *pLength = pHashItem->dataLength;
   if ( pHashMap->pRecordDefinition != NULL ) {
      GET_PTR( pHashMap->pRecordDefinition->pMemDefinition, 
               pHashMap->iterator.pHashItem->dataDefOffset, 
               psonDataDefinition );
   }

   return 0;

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

