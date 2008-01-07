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
#include <vdsf/vdsFolder.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderClose( VDS_HANDLE objectHandle )
{
   vdsaFolder * pFolder;
   vdseFolder * pVDSFolder;
   int errcode = 0;
   
   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;
   
   if ( vdsaCommonLock( &pFolder->object ) == 0 )
   {
      pVDSFolder = (vdseFolder *) pFolder->object.pMyVdsObject;

      /* Reinitialize the iterator, if needed */
      if ( pFolder->iterator.pHashItem != NULL )
      {
         if ( vdseFolderRelease( pVDSFolder,
                                 &pFolder->iterator,
                                 &pFolder->object.pSession->context ) == 0 )
            memset( &pFolder->iterator, 0, sizeof(vdseFolderItem) );
         else
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      }

      if ( errcode == 0 )
         errcode = vdsaCommonObjClose( &pFolder->object );

      vdsaCommonUnlock( &pFolder->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;

   if ( errcode == 0 )
      free( pFolder );
   else
      vdscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetFirst( VDS_HANDLE       objectHandle,
                       vdsFolderEntry * pEntry )
{
   vdsaFolder * pFolder;
   vdseFolder * pVDSFolder;
   int errcode = 0, rc = 0;
   vdseObjectDescriptor * pDescriptor;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   const wchar_t * name;
#endif

   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pEntry == NULL )
   {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }
   
   if ( vdsaCommonLock( &pFolder->object ) != 0 )
   {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSFolder = (vdseFolder *) pFolder->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pFolder->iterator.pHashItem != NULL )
   {
      rc = vdseFolderRelease( pVDSFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
      if ( rc == 0 )
         memset( &pFolder->iterator, 0, sizeof(vdseFolderItem) );
      else
         goto error_handler_unlock;
   }

   rc = vdseFolderGetFirst( pVDSFolder,
                            &pFolder->iterator,
                            &pFolder->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;

   memset( pEntry, 0, sizeof( vdsFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         vdseObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
#if VDS_SUPPORT_i18n
   memset( &ps, 0, sizeof(mbstate_t) );
   name = pDescriptor->originalName;
   pEntry->nameLengthInBytes = wcsrtombs( pEntry->name, 
                                          &name,
                                          VDS_MAX_NAME_LENGTH*4,
                                          &ps );
   if ( pEntry->nameLengthInBytes == (size_t) -1 )
   {
      /* A conversion error */
      vdscSetError( &pFolder->object.pSession->context.errorHandler, 
                    VDSC_ERRNO_HANDLE, errno );
      errcode = VDS_I18N_CONVERSION_ERROR;
      goto error_handler_unlock;
   }
#else
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );
#endif

   vdsaCommonUnlock( &pFolder->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pFolder->object );

error_handler:
   if ( errcode != 0 )
      vdscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pFolder->object.pSession->context.errorHandler );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetNext( VDS_HANDLE       objectHandle,
                      vdsFolderEntry * pEntry )
{
   vdsaFolder * pFolder;
   vdseFolder * pVDSFolder;
   int errcode = 0, rc = 0;
   vdseObjectDescriptor * pDescriptor;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   const wchar_t * name;
#endif

   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pEntry == NULL )
   {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->iterator.pHashItem == NULL )
   {
      errcode = VDS_INVALID_ITERATOR;
      goto error_handler;
   }

   if ( vdsaCommonLock( &pFolder->object ) != 0 )
   {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSFolder = (vdseFolder *) pFolder->object.pMyVdsObject;

   rc = vdseFolderGetNext( pVDSFolder,
                           &pFolder->iterator,
                           &pFolder->object.pSession->context );
   if ( rc != 0 )
      goto error_handler_unlock;
   
   memset( pEntry, 0, sizeof( vdsFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         vdseObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
#if VDS_SUPPORT_i18n
   memset( &ps, 0, sizeof(mbstate_t) );
   name = pDescriptor->originalName;
   pEntry->nameLengthInBytes = wcsrtombs( pEntry->name, 
                                          &name, 
                                          VDS_MAX_NAME_LENGTH*4,
                                          &ps );
   if ( pEntry->nameLengthInBytes == (size_t) -1 )
   {
      /* A conversion error */
      vdscSetError( &pFolder->object.pSession->context.errorHandler, 
                    VDSC_ERRNO_HANDLE, errno );
      errcode = VDS_I18N_CONVERSION_ERROR;
      goto error_handler_unlock;
   }
#else
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );
#endif

   vdsaCommonUnlock( &pFolder->object );

   return 0;

error_handler_unlock:
   vdsaCommonUnlock( &pFolder->object );

error_handler:
   if ( errcode != 0 )
      vdscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );

   if ( rc != 0 )
      errcode = vdscGetLastError( &pFolder->object.pSession->context.errorHandler );
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderOpen( VDS_HANDLE   sessionHandle,
                   const char * folderName,
                   size_t       nameLengthInBytes,
                   VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaFolder * pFolder = NULL;
   int errcode;
   
   if ( objectHandle == NULL )
      return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (vdsaSession*) sessionHandle;
   if ( pSession == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION )
      return VDS_WRONG_TYPE_HANDLE;

   if ( folderName == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pFolder = (vdsaFolder *) malloc(sizeof(vdsaFolder));
   if (  pFolder == NULL )
   {
      vdscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pFolder, 0, sizeof(vdsaFolder) );
   pFolder->object.type = VDSA_FOLDER;
   pFolder->object.pSession = pSession;

   errcode = vdsaCommonObjOpen( &pFolder->object,
                                VDS_FOLDER,
                                folderName,
                                nameLengthInBytes );
   if ( errcode == 0 )
      *objectHandle = (VDS_HANDLE) pFolder;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderStatus( VDS_HANDLE     objectHandle,
                     vdsObjStatus * pStatus )
{
   vdsaFolder * pFolder;
   vdseFolder * pVDSFolder;
   int errcode = 0;
   vdseSessionContext * pContext;
   
   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;

   pContext = &pFolder->object.pSession->context;

   if ( pStatus == NULL )
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   
   if ( vdsaCommonLock( &pFolder->object ) == 0 )
   {
      pVDSFolder = (vdseFolder *) pFolder->object.pMyVdsObject;
      
      if ( vdseLock( &pVDSFolder->memObject, pContext ) == 0 )
      {
         vdseMemObjectStatus( &pVDSFolder->memObject, pStatus );

         vdseFolderStatus( pVDSFolder, pStatus );

         vdseUnlock( &pVDSFolder->memObject, pContext );
      }
      else
         errcode = VDS_OBJECT_CANNOT_GET_LOCK;
      
      vdsaCommonUnlock( &pFolder->object );
   }
   else
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
   
   if ( errcode != 0 )
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

