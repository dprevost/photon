/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
#include <photon/vdsFolder.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <photon/vdsErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 * 
 * This file contains the source code for some functions of the C API. 
 * For documentation for these functions, please see the distributed
 * header files.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderClose( VDS_HANDLE objectHandle )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = VDS_OK;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pFolder->object.pSession->terminated ) {

      if ( psaCommonLock( &pFolder->object ) ) {
        pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

         /* Reinitialize the iterator, if needed */
         if ( pFolder->iterator.pHashItem != NULL ) {
            if ( psnFolderRelease( pVDSFolder,
                                    &pFolder->iterator,
                                    &pFolder->object.pSession->context ) == 0 ) {
               memset( &pFolder->iterator, 0, sizeof(psnFolderItem) );
            }
            else {
               errcode = VDS_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == VDS_OK ) {
            errcode = psaCommonObjClose( &pFolder->object );
         }
         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   if ( errcode == VDS_OK ) {
      /*
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pFolder->object.type = 0; 
      free( pFolder );
   }
   else {
      pscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderCreateObject( VDS_HANDLE            objectHandle,
                           const char          * objectName,
                           size_t                nameLengthInBytes,
                           vdsObjectDefinition * pDefinition )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = VDS_OK;
   bool ok = true;
   psaSession* pSession;

   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }

   errcode = psaValidateDefinition( pDefinition );
   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
      return errcode;
   }

   if ( ! pSession->terminated ) {
      if ( psaCommonLock( &pFolder->object ) ) {
         pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

         ok = psnFolderCreateObject( pVDSFolder,
                                      objectName,
                                      nameLengthInBytes,
                                      pDefinition,
                                      &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );
         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderCreateObjectXML( VDS_HANDLE   objectHandle,
                              const char * xmlBuffer,
                              size_t       lengthInBytes )
{
   vdsObjectDefinition * pDefinition = NULL;
   int errcode = VDS_OK;
   char * objectName = NULL;
   size_t nameLengthInBytes = 0;
   
   if ( xmlBuffer == NULL ) return VDS_NULL_POINTER;
   if ( lengthInBytes == 0 ) return VDS_INVALID_LENGTH;
   
   errcode = psaXmlToDefinition( xmlBuffer,
                                  lengthInBytes,
                                  &pDefinition,
                                  &objectName,
                                  &nameLengthInBytes );
   if ( errcode == VDS_OK ) {
      errcode = vdsFolderCreateObject( objectHandle,
                                       objectName,
                                       nameLengthInBytes,
                                       pDefinition );
   }
   
   if ( pDefinition != NULL ) free(pDefinition);
   if ( objectName != NULL )  free(objectName);
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderDestroyObject( VDS_HANDLE   objectHandle,
                            const char * objectName,
                            size_t       nameLengthInBytes )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = VDS_OK;
   psaSession* pSession;
   bool ok = true;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   if ( ! pSession->terminated ) {
      if ( psaCommonLock( &pFolder->object ) ) {
         pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

         ok = psnFolderDestroyObject( pVDSFolder,
                                       objectName,
                                       nameLengthInBytes,
                                       &pSession->context );
         VDS_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }

   if ( errcode != VDS_OK ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetFirst( VDS_HANDLE       objectHandle,
                       vdsFolderEntry * pEntry )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = VDS_OK;
   psnObjectDescriptor * pDescriptor;
   bool ok;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( pEntry == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   if ( ! psaCommonLock( &pFolder->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pFolder->iterator.pHashItem != NULL ) {
      ok = psnFolderRelease( pVDSFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
      VDS_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         memset( &pFolder->iterator, 0, sizeof(psnFolderItem) );
      }
      else {
         goto error_handler_unlock;
      }
   }

   ok = psnFolderGetFirst( pVDSFolder,
                            &pFolder->iterator,
                            &pFolder->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   memset( pEntry, 0, sizeof( vdsFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         psnObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );

   psaCommonUnlock( &pFolder->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pFolder->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   } 
   else {
      errcode = pscGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderGetNext( VDS_HANDLE       objectHandle,
                      vdsFolderEntry * pEntry )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = VDS_OK;
   psnObjectDescriptor * pDescriptor;
   bool ok;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( pEntry == NULL ) {
      errcode = VDS_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->iterator.pHashItem == NULL ) {
      errcode = VDS_INVALID_ITERATOR;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = VDS_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   if ( ! psaCommonLock( &pFolder->object ) ) {
      errcode = VDS_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

   ok = psnFolderGetNext( pVDSFolder,
                           &pFolder->iterator,
                           &pFolder->object.pSession->context );
   VDS_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   memset( pEntry, 0, sizeof( vdsFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         psnObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );

   psaCommonUnlock( &pFolder->object );

   return VDS_OK;

error_handler_unlock:
   psaCommonUnlock( &pFolder->object );

error_handler:
   if ( errcode != VDS_OK ) {
      pscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_vdsErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderOpen( VDS_HANDLE   sessionHandle,
                   const char * folderName,
                   size_t       nameLengthInBytes,
                   VDS_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaFolder * pFolder = NULL;
   int errcode;
   
   if ( objectHandle == NULL ) return VDS_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   if ( folderName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_OBJECT_NAME );
      return VDS_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_INVALID_LENGTH );
      return VDS_INVALID_LENGTH;
   }
   
   pFolder = (psaFolder *) malloc(sizeof(psaFolder));
   if (  pFolder == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_vdsErrorHandle, VDS_NOT_ENOUGH_HEAP_MEMORY );
      return VDS_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pFolder, 0, sizeof(psaFolder) );
   pFolder->object.type = PSA_FOLDER;
   pFolder->object.pSession = pSession;

   if ( ! pFolder->object.pSession->terminated ) {

      errcode = psaCommonObjOpen( &pFolder->object,
                                   VDS_FOLDER,
                                   PSA_READ_WRITE,
                                   folderName,
                                   nameLengthInBytes );
      if ( errcode == VDS_OK ) *objectHandle = (VDS_HANDLE) pFolder;
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdsFolderStatus( VDS_HANDLE     objectHandle,
                     vdsObjStatus * pStatus )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = VDS_OK;
   psnSessionContext * pContext;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return VDS_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return VDS_WRONG_TYPE_HANDLE;
   }
   
   pContext = &pFolder->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, VDS_NULL_POINTER );
      return VDS_NULL_POINTER;
   }
   
   if ( ! pFolder->object.pSession->terminated ) {

      if ( psaCommonLock( &pFolder->object ) ) {

         pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;
      
         if ( psnLock(&pVDSFolder->memObject, pContext) ) {

            psnMemObjectStatus( &pVDSFolder->memObject, pStatus );

            psnFolderMyStatus( pVDSFolder, pStatus );

            psnUnlock( &pVDSFolder->memObject, pContext );
         }
         else {
            errcode = VDS_OBJECT_CANNOT_GET_LOCK;
         }
         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = VDS_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = VDS_SESSION_IS_TERMINATED;
   }
      
   if ( errcode != VDS_OK ) {
      pscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

