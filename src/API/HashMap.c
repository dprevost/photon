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

int vdsHashMapGet( VDS_HANDLE        objectHandle,
                   const void      * key,
                   size_t            keyLength,
                   vdsHashMapEntry * pEntry )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;
   vdseHashItem * pHashItem;
   
   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( key == NULL )
      return VDS_NULL_POINTER;
   if ( keyLength == 0 )
      return VDS_INVALID_LENGTH;
   if ( pEntry == NULL )
      return VDS_NULL_POINTER;
      
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapGet( pVDSHashMap,
                                key,
                                keyLength,
                                &pHashItem,
                                &pHashMap->object.pSession->context );
      if ( errcode != 0 )
         errcode = vdscGetLastError( &pHashMap->object.pSession->context.errorHandler );
      else
      {
         pEntry->data = GET_PTR( pHashItem->dataOffset, void );
         pEntry->length = pHashItem->dataLength;
      }
      vdsaCommonUnlock( &pHashMap->object );
   }
   else
      return VDS_SESSION_CANNOT_GET_LOCK;
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsHashMapGetFirst( VDS_HANDLE        objectHandle,
                        vdsHashMapEntry * pEntry )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pEntry == NULL )
      return VDS_NULL_POINTER;
   
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
                                     &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         pEntry->data = GET_PTR( pHashMap->iterator.pHashItem->dataOffset, void );
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

int vdsHashMapGetNext( VDS_HANDLE       objectHandle,
                      vdsHashMapEntry * pEntry )
{
   vdsaHashMap * pHashMap;
   vdseHashMap * pVDSHashMap;
   int errcode = 0;

   pHashMap = (vdsaHashMap *) objectHandle;
   if ( pHashMap == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pHashMap->type != VDSA_HASH_MAP )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pEntry == NULL )
      return VDS_NULL_POINTER;

   if ( pHashMap->iterator.pHashItem == NULL )
      return VDS_INVALID_ITERATOR;
   
   if ( vdsaCommonLock( &pHashMap->object ) == 0 )
   {
      pVDSHashMap = (vdseHashMap *) pHashMap->object.pMyVdsObject;

      errcode = vdseHashMapGetNext( pVDSHashMap,
                                    &pHashMap->iterator,
                                    &pHashMap->object.pSession->context );
      if ( errcode == 0 )
      {
         pEntry->data = GET_PTR( pHashMap->iterator.pHashItem->dataOffset, void );
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

int vdsHashMapStatus( VDS_HANDLE   objectHandle,
                      size_t     * numChildren )
{
   return VDS_INTERNAL_ERROR;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

