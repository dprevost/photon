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
   int errcode;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;
   
   errcode = vdsCommonObjectClose( &pHashMap->object );
   if ( errcode == 0 )
   {
      free( pHashMap );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapDelete( VDS_HANDLE   objectHandle,
                      const void * key,
                      size_t       keyLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL )
      return VDS_NULL_POINTER;
   if ( keyLength == 0 )
      return VDS_INVALID_LENGTH;
      
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapDelete( pVDSHashMap,
                                   key,
                                   keyLength,
                                   &pHashMap->object.pSession->context );
      if ( errcode != 0 )
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
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
   int errcode = 0;
   void * ptr;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL )
      return VDS_NULL_POINTER;
   if ( keyLength == 0 )
      return VDS_INVALID_LENGTH;
   if ( buffer == NULL )
      return VDS_NULL_POINTER;

   if ( returnedLength == NULL )
      return VDS_NULL_POINTER;
   *returnedLength = 0;

   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pHashMap->iterator.pHashItem != NULL )
      {
         if ( vdseHashMapRelease( pVDSHashMap,
                                  pHashMap->iterator.pHashItem,
                                  &pHashMap->object.pSession->context ) == 0 )
            memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
         else
            return VDS_OBJECT_CANNOT_GET_LOCK;
      }

      errcode = vdseHashMapGet( pVDSHashMap,
                                key,
                                keyLength,
                                &pHashMap->iterator.pHashItem,
                                bufferLength,
                                &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         *returnedLength = pHashMap->iterator.pHashItem->dataLength;
         GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
         memcpy( buffer, ptr, *returnedLength );
      }
      else
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapGetFirst( VDS_HANDLE   objectHandle,
                        void       * buffer,
                        size_t       bufferLength,
                        size_t     * returnedLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;
   void * ptr;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL )
      return VDS_NULL_POINTER;

   if ( returnedLength == NULL )
      return VDS_NULL_POINTER;
   *returnedLength = 0;
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pHashMap->iterator.pHashItem != NULL )
      {
         if ( vdseHashMapRelease( pVDSHashMap,
                                  pHashMap->iterator.pHashItem,
                                  &pHashMap->object.pSession->context ) == 0 )
            memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
         else
            return VDS_OBJECT_CANNOT_GET_LOCK;
      }
         
      errcode = vdseHashMapGetFirst( pVDSHashMap,
                                     &pHashMap->iterator,
                                     bufferLength,
                                     &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         *returnedLength = pHashMap->iterator.pHashItem->dataLength;
         GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
         memcpy( buffer, ptr, *returnedLength );
      }
      else
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapGetNext( VDS_HANDLE   objectHandle,
                       void       * buffer,
                       size_t       bufferLength,
                       size_t     * returnedLength )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;
   void * ptr;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( buffer == NULL )
      return VDS_NULL_POINTER;

   if ( returnedLength == NULL )
      return VDS_NULL_POINTER;
   *returnedLength = 0;

   if ( pHashMap->iterator.pHashItem == NULL )
      return VDS_INVALID_ITERATOR;
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapGetNext( pVDSHashMap,
                                    &pHashMap->iterator,
                                    bufferLength,
                                    &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         *returnedLength = pHashMap->iterator.pHashItem->dataLength;
         GET_PTR( ptr, pHashMap->iterator.pHashItem->dataOffset, void );
         memcpy( buffer, ptr, *returnedLength );
      }
      else
      {
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
      }
      if ( errcode == VDS_REACHED_THE_END )
      {
         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL )
         {
            if ( vdseHashMapRelease( pVDSHashMap,
                                     pHashMap->iterator.pHashItem,
                                     &pHashMap->object.pSession->context ) == 0 )
            memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
         }
      }
      
      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

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
   int errcode = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL )
      return VDS_NULL_POINTER;
   if ( keyLength == 0 )
      return VDS_INVALID_LENGTH;
   if ( data == NULL )
      return VDS_NULL_POINTER;
   if ( dataLength == 0 )
      return VDS_INVALID_LENGTH;

   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapInsert( pVDSHashMap,
                                   key,
                                   keyLength,
                                   data,
                                   dataLength,
                                   &pHashMap->object.pSession->context );
      if ( errcode != 0 )
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapOpen( VDS_HANDLE   sessionHandle,
                    const char * folderName,
                    size_t       nameLengthInBytes,
                    VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaHashMap * pHashMap = NULL;
   int errcode;
   
   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( folderName == NULL )
      return VDS_INVALID_OBJECT_NAME;
   
   if ( objectHandle == NULL )
      return VDS_NULL_HANDLE;

   if ( nameLengthInBytes == 0 )
      return VDS_INVALID_LENGTH;
   
   *objectHandle = NULL;
   
   pHashMap = (vdsaHashMap *) malloc(sizeof(vdsaHashMap));
   if (  pHashMap == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pHashMap, 0, sizeof(vdsaHashMap) );
   pHashMap->type = VDSA_HASH_MAP;
   pHashMap->object.pSession = pSession;

   errcode = vdsaCommonObjectOpen( &pHashMap->object,
                                   VDS_FOLDER,
                                   folderName,
                                   nameLengthInBytes );
   if ( errcode == 0 )
      *objectHandle = (VDS_HANDLE) pHashMap;

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
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pStatus == NULL )
      return VDS_NULL_POINTER;

   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;
      pContext = &pHashMap->object.pSession->context;
      
      if ( vdseLock( &pVDSHashMap->memObject, pContext ) == 0 )
      {
         vdseMemObjectStatus( &pVDSHashMap->memObject, pStatus );

         vdseHashMapStatus( pVDSHashMap, pStatus );

         vdseUnlock( &pVDSHashMap->memObject, pContext );
      }
      else
      {
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      }
      
      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* The non-API functions in alphabetic order */


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaHashMapFirst( vdsaHashMap   * pHashMap,
                      vdsaDataEntry * pEntry )
{
   vdseHashMap * pVDSHashMap;
   int errcode = 0;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->type == VDSA_HASH_MAP );
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pHashMap->iterator.pHashItem != NULL )
      {
         if ( vdseHashMapRelease( pVDSHashMap,
                                  pHashMap->iterator.pHashItem,
                                  &pHashMap->object.pSession->context ) == 0 )
            memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
         else
            return VDS_OBJECT_CANNOT_GET_LOCK;
      }
         
      errcode = vdseHashMapGetFirst( pVDSHashMap,
                                     &pHashMap->iterator,
                                     (size_t) -1,
                                     &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
         pEntry->length = pHashMap->iterator.pHashItem->dataLength;
      }
      else
      {
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
      }
      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaHashMapNext( vdsaHashMap   * pHashMap,
                     vdsaDataEntry * pEntry )
{
   vdseHashMap * pVDSHashMap;
   int errcode = 0;

   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( pHashMap->type == VDSA_HASH_MAP );
   VDS_PRE_CONDITION( pHashMap->iterator.pHashItem != NULL );
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapGetNext( pVDSHashMap,
                                    &pHashMap->iterator,
                                    (size_t) -1,
                                    &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         GET_PTR( pEntry->data, pHashMap->iterator.pHashItem->dataOffset, void );
         pEntry->length = pHashMap->iterator.pHashItem->dataLength;
      }
      else
      {
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
         /* Reinitialize the iterator, if needed */
         if ( pHashMap->iterator.pHashItem != NULL )
         {
            if ( vdseHashMapRelease( pVDSHashMap,
                                     pHashMap->iterator.pHashItem,
                                     &pHashMap->object.pSession->context ) == 0 )
            memset( &pHashMap->iterator, 0, sizeof(vdseHashMapItem) );
         }
      }
      
      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsaHashMapRetrieve( vdsaHashMap   * pHashMap,
                         const void    * key,
                         size_t          keyLength,
                         vdsaDataEntry * pEntry )
{
   vdseHashMap * pVDSHashMap;
   int errcode = 0;
   vdseHashItem * pHashItem;
   
   VDS_PRE_CONDITION( pHashMap != NULL );
   VDS_PRE_CONDITION( key      != NULL );
   VDS_PRE_CONDITION( pEntry   != NULL );
   VDS_PRE_CONDITION( keyLength > 0 );
   VDS_PRE_CONDITION( pHashMap->type == VDSA_HASH_MAP );

   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapGet( pVDSHashMap,
                                key,
                                keyLength,
                                &pHashItem,
                                (size_t) -1,
                                &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         GET_PTR( pEntry->data, pHashItem->dataOffset, void );
         pEntry->length = pHashItem->dataLength;
      }
      else
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );

      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

