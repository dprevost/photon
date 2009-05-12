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
#include <photon/FastMap.h>
#include "API/FastMap.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

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

int psoFastMapDefinition( PSO_HANDLE   objectHandle,
                          PSO_HANDLE * keyDefHandle,
                          PSO_HANDLE * dataDefHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   psoaDataDefinition * pDataDefinition = NULL;
   psoaKeyDefinition  * pKeyDefinition = NULL;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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
      pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
      
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
   
   if ( errcode != PSO_OK ) {
      if ( pDataDefinition ) free(pDataDefinition);
      if ( pKeyDefinition) free(pKeyDefinition);
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

      pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

      ok = psonFastMapDelete( pMemHashMap,
                              key,
                              keyLength,
                              &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
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

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler;
      
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   psonFastMapEmpty( pMemHashMap, &pHashMap->object.pSession->context );

   return PSO_OK;

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

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler;
      
      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGet( pMemHashMap,
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

   return PSO_OK;

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

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );

      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGetFirst( pMemHashMap,
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

   return PSO_OK;

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

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapGetNext( pMemHashMap,
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

   return PSO_OK;

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
                      uint32_t     dataLength,
                      PSO_HANDLE   dataDefHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psoaDataDefinition * pDefinition = NULL;
   psonDataDefinition * pMemDefinition = NULL;
   psonDataDefinition * pDef;

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

   if ( pHashMap->editMode == 0 ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, PSO_OBJECT_IS_READ_ONLY );
      return PSO_OBJECT_IS_READ_ONLY;
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

      pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

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
         ok = psonFastMapInsert( pMemHashMap,
                                 key,
                                 keyLength,
                                 data,
                                 dataLength,
                                 pMemDefinition,
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
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pHashMap->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
      free(pHashMap);
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFastMapRecordDefinition( PSO_HANDLE   objectHandle,
                                PSO_HANDLE * dataDefHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psoaDataDefinition * pDefinition = NULL;
   
   pHashMap = (psoaFastMap *) objectHandle;
   if ( pHashMap == NULL ) return PSO_NULL_HANDLE;
   
   if ( pHashMap->object.type != PSOA_MAP ) return PSO_WRONG_TYPE_HANDLE;

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
      pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
 
      /* We use the global queue definition as the initial value
       * to avoid an uninitialized pointer.
       */
      GET_PTR( pDefinition->pMemDefinition, 
               pMemHashMap->dataDefOffset, 
               psonDataDefinition );
      pDefinition->ppApiObject = &pHashMap->pRecordDefinition;
      pHashMap->pRecordDefinition = pDefinition;
         
      dataDefHandle = (PSO_HANDLE) pDefinition;
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

int psoFastMapReplace( PSO_HANDLE   objectHandle,
                       const void * key,
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength,
                       PSO_HANDLE   dataDefHandle )
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psoaDataDefinition * pDefinition = NULL;
   psonDataDefinition * pMemDefinition = NULL;
   psonDataDefinition * pDef;

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

   if ( dataDefHandle != NULL ) {
      pDefinition = (psoaDataDefinition *) dataDefHandle;
   
      if ( pDefinition->definitionType != PSOA_DEF_DATA )  {
         psocSetError( &pHashMap->object.pSession->context.errorHandler, 
            g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
         return PSO_WRONG_TYPE_HANDLE;
      }
   }

   if ( ! pHashMap->object.pSession->terminated ) {

      pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

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
         ok = psonFastMapReplace( pMemHashMap,
                                  key,
                                  keyLength,
                                  data,
                                  dataLength,
                                  pMemDefinition,
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

int psoaFastMapFirst( psoaFastMap    * pHashMap,
                      unsigned char ** pKey,
                      uint32_t       * pKeyLength,
                      unsigned char ** pData,
                      uint32_t       * pDataLength )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap    != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGetFirst( pMemHashMap,
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
   
   return PSO_OK;

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

int psoaFastMapNext( psoaFastMap    * pHashMap,
                     unsigned char ** pKey,
                     uint32_t       * pKeyLength,
                     unsigned char ** pData,
                     uint32_t       * pDataLength )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;

   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( pKey        != NULL );
   PSO_PRE_CONDITION( pKeyLength  != NULL );
   PSO_PRE_CONDITION( pData       != NULL );
   PSO_PRE_CONDITION( pDataLength != NULL );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );
   PSO_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   ok = psonFastMapGetNext( pMemHashMap,
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

   return PSO_OK;

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

int psoaFastMapRetrieve( psoaFastMap    * pHashMap,
                         const void     * key,
                         uint32_t         keyLength,
                         unsigned char ** pData,
                         uint32_t       * pLength )
{
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   bool ok = true;
   psonHashItem * pHashItem;
   
   PSO_PRE_CONDITION( pHashMap != NULL );
   PSO_PRE_CONDITION( key      != NULL );
   PSO_PRE_CONDITION( pData    != NULL );
   PSO_PRE_CONDITION( pLength  != NULL );
   PSO_PRE_CONDITION( keyLength > 0 );
   PSO_PRE_CONDITION( pHashMap->object.type == PSOA_MAP );

   if ( pHashMap->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pHashMap->iterator.pHashItem != NULL ) {
      ok = psonFastMapRelease( pMemHashMap,
                               pHashMap->iterator.pHashItem,
                               &pHashMap->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;

      memset( &pHashMap->iterator, 0, sizeof(psonFastMapItem) );
   }

   ok = psonFastMapGet( pMemHashMap,
                        key,
                        keyLength,
                        &pHashItem,
                        (uint32_t) -1,
                        &pHashMap->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   GET_PTR( *pData, pHashItem->dataOffset, void );
   *pLength = pHashItem->dataLength;

   return PSO_OK;

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

