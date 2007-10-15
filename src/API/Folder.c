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
#include <vdsf/vdsFolder.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <vdsf/vdsErrors.h>
#include "API/CommonObject.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderOpen( VDS_HANDLE   sessionHandle,
                   const char * folderName,
                   size_t       nameLengthInBytes,
                   VDS_HANDLE * objectHandle )
{
   vdsaSession * pSession;
   vdsaFolder * pFolder = NULL;
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
   
   pFolder = (vdsaFolder *) malloc(sizeof(vdsaFolder));
   if (  pFolder == NULL )
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   
   memset( pFolder, 0, sizeof(vdsaFolder) );
   pFolder->type = VDSA_FOLDER;
   pFolder->object.pSession = pSession;

   errcode = vdsaCommonObjectOpen( &pFolder->object,
                                   VDS_FOLDER,
                                   folderName,
                                   nameLengthInBytes );
   if ( errcode == 0 )
      *objectHandle = (VDS_HANDLE) pFolder;

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderClose( VDS_HANDLE objectHandle )
{
   vdsaFolder * pFolder;
   int errcode;
   
   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;
   
   errcode = vdsCommonObjectClose( &pFolder->object );
   if ( errcode == 0 )
   {
      free( pFolder );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetFirst( VDS_HANDLE       objectHandle,
                       vdsFolderEntry * pEntry )
{
   vdsaFolder * pFolder;
   vdseFolder * pVDSFolder;
   int errcode = 0;
   vdseObjectDescriptor * pDescriptor;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   const wchar_t * name;
#endif

   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pEntry == NULL )
      return VDS_NULL_POINTER;
   
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
            return VDS_OBJECT_CANNOT_GET_LOCK;
      }
         
      errcode = vdseFolderGetFirst( pVDSFolder,
                                    &pFolder->iterator,
                                    &pFolder->object.pSession->context );
      if ( errcode == 0 )
      {
         memset( pEntry, 0, sizeof( vdsFolderEntry ) );
         pDescriptor = GET_PTR( pFolder->iterator.pHashItem->dataOffset, 
                                vdseObjectDescriptor );
         pEntry->type = pDescriptor->type;
#if VDS_SUPPORT_i18n
         memset( &ps, 0, sizeof(mbstate_t) );
         name = pDescriptor->originalName;
         pEntry->nameLengthInBytes = wcsrtombs( pEntry->name, 
                                                &name,
                                                VDS_MAX_NAME_LENGTH*4,
                                                &ps );
#else
         pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
         memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );
#endif
      }
      else
      {
         errcode = vdscGetLastError( &pFolder->object.pSession->context.errorHandler );
      }
      vdsaCommonUnlock( &pFolder->object );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetNext( VDS_HANDLE       objectHandle,
                      vdsFolderEntry * pEntry )
{
   vdsaFolder * pFolder;
   vdseFolder * pVDSFolder;
   int errcode = 0;
   vdseObjectDescriptor * pDescriptor;
#if VDS_SUPPORT_i18n
   mbstate_t ps;
   const wchar_t * name;
#endif

   pFolder = (vdsaFolder *) objectHandle;
   if ( pFolder == NULL )
      return VDS_NULL_HANDLE;
   
   if ( pFolder->type != VDSA_FOLDER )
      return VDS_WRONG_TYPE_HANDLE;

   if ( pEntry == NULL )
      return VDS_NULL_POINTER;

   if ( pFolder->iterator.pHashItem == NULL )
      return VDS_INVALID_ITERATOR;
   
   if ( vdsaCommonLock( &pFolder->object ) == 0 )
   {
      pVDSFolder = (vdseFolder *) pFolder->object.pMyVdsObject;

      errcode = vdseFolderGetNext( pVDSFolder,
                                   &pFolder->iterator,
                                   &pFolder->object.pSession->context );
      if ( errcode == 0 )
      {
         memset( pEntry, 0, sizeof( vdsFolderEntry ) );
         pDescriptor = GET_PTR( pFolder->iterator.pHashItem->dataOffset, 
                                vdseObjectDescriptor );
         pEntry->type = pDescriptor->type;
#if VDS_SUPPORT_i18n
         memset( &ps, 0, sizeof(mbstate_t) );
         name = pDescriptor->originalName;
         pEntry->nameLengthInBytes = wcsrtombs( pEntry->name, 
                                                &name, 
                                                VDS_MAX_NAME_LENGTH*4,
                                                &ps );
#else
         pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
         memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );
#endif
      }
      else
      {
         errcode = vdscGetLastError( &pFolder->object.pSession->context.errorHandler );
         /* Reinitialize the iterator, if needed */
         if ( pFolder->iterator.pHashItem != NULL )
         {
            if ( vdseFolderRelease( pVDSFolder,
                                    &pFolder->iterator,
                                    &pFolder->object.pSession->context ) == 0 )
            memset( &pFolder->iterator, 0, sizeof(vdseFolderItem) );
         }
      }
      
      vdsaCommonUnlock( &pFolder->object );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

