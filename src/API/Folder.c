/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include <photon/psoFolder.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 * 
 * This file contains the source code for some functions of the C API. 
 * For documentation for these functions, please see the distributed
 * header files.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderClose( PSO_HANDLE objectHandle )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = PSO_OK;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
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
               errcode = PSO_OBJECT_CANNOT_GET_LOCK;
            }
         }

         if ( errcode == PSO_OK ) {
            errcode = psaCommonObjClose( &pFolder->object );
         }
         psaCommonUnlock( &pFolder->object );
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
       * Memory might still be around even after it is released, so we make 
       * sure future access with the handle fails by setting the type wrong!
       */
      pFolder->object.type = 0; 
      free( pFolder );
   }
   else {
      pscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateObject( PSO_HANDLE            objectHandle,
                           const char          * objectName,
                           size_t                nameLengthInBytes,
                           psoObjectDefinition * pDefinition )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psaSession* pSession;

   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   errcode = psaValidateDefinition( pDefinition );
   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
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
         PSO_POST_CONDITION( ok == true || ok == false );
         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateObjectXML( PSO_HANDLE   objectHandle,
                              const char * xmlBuffer,
                              size_t       lengthInBytes )
{
   psoObjectDefinition * pDefinition = NULL;
   int errcode = PSO_OK;
   char * objectName = NULL;
   size_t nameLengthInBytes = 0;
   
   if ( xmlBuffer == NULL ) return PSO_NULL_POINTER;
   if ( lengthInBytes == 0 ) return PSO_INVALID_LENGTH;
   
   errcode = psaXmlToDefinition( xmlBuffer,
                                  lengthInBytes,
                                  &pDefinition,
                                  &objectName,
                                  &nameLengthInBytes );
   if ( errcode == PSO_OK ) {
      errcode = psoFolderCreateObject( objectHandle,
                                       objectName,
                                       nameLengthInBytes,
                                       pDefinition );
   }
   
   if ( pDefinition != NULL ) free(pDefinition);
   if ( objectName != NULL )  free(objectName);
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderDestroyObject( PSO_HANDLE   objectHandle,
                            const char * objectName,
                            size_t       nameLengthInBytes )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = PSO_OK;
   psaSession* pSession;
   bool ok = true;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pSession->terminated ) {
      if ( psaCommonLock( &pFolder->object ) ) {
         pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

         ok = psnFolderDestroyObject( pVDSFolder,
                                       objectName,
                                       nameLengthInBytes,
                                       &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );

         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = pscGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetFirst( PSO_HANDLE       objectHandle,
                       psoFolderEntry * pEntry )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = PSO_OK;
   psnObjectDescriptor * pDescriptor;
   bool ok;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( pEntry == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   if ( ! psaCommonLock( &pFolder->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }
   
   pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

   /* Reinitialize the iterator, if needed */
   if ( pFolder->iterator.pHashItem != NULL ) {
      ok = psnFolderRelease( pVDSFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
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
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;

   memset( pEntry, 0, sizeof( psoFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         psnObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );

   psaCommonUnlock( &pFolder->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pFolder->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = pscGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetNext( PSO_HANDLE       objectHandle,
                      psoFolderEntry * pEntry )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = PSO_OK;
   psnObjectDescriptor * pDescriptor;
   bool ok;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( pEntry == NULL ) {
      errcode = PSO_NULL_POINTER;
      goto error_handler;
   }

   if ( pFolder->iterator.pHashItem == NULL ) {
      errcode = PSO_INVALID_ITERATOR;
      goto error_handler;
   }

   if ( pFolder->object.pSession->terminated ) {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }
   
   if ( ! psaCommonLock( &pFolder->object ) ) {
      errcode = PSO_SESSION_CANNOT_GET_LOCK;
      goto error_handler;
   }

   pVDSFolder = (psnFolder *) pFolder->object.pMyVdsObject;

   ok = psnFolderGetNext( pVDSFolder,
                           &pFolder->iterator,
                           &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler_unlock;
   
   memset( pEntry, 0, sizeof( psoFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         psnObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );

   psaCommonUnlock( &pFolder->object );

   return PSO_OK;

error_handler_unlock:
   psaCommonUnlock( &pFolder->object );

error_handler:
   if ( errcode != PSO_OK ) {
      pscSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = pscGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderOpen( PSO_HANDLE   sessionHandle,
                   const char * folderName,
                   size_t       nameLengthInBytes,
                   PSO_HANDLE * objectHandle )
{
   psaSession * pSession;
   psaFolder * pFolder = NULL;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSA_SESSION ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( folderName == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pFolder = (psaFolder *) malloc(sizeof(psaFolder));
   if (  pFolder == NULL ) {
      pscSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pFolder, 0, sizeof(psaFolder) );
   pFolder->object.type = PSA_FOLDER;
   pFolder->object.pSession = pSession;

   if ( ! pFolder->object.pSession->terminated ) {

      errcode = psaCommonObjOpen( &pFolder->object,
                                   PSO_FOLDER,
                                   PSA_READ_WRITE,
                                   folderName,
                                   nameLengthInBytes );
      if ( errcode == PSO_OK ) *objectHandle = (PSO_HANDLE) pFolder;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderStatus( PSO_HANDLE     objectHandle,
                     psoObjStatus * pStatus )
{
   psaFolder * pFolder;
   psnFolder * pVDSFolder;
   int errcode = PSO_OK;
   psnSessionContext * pContext;
   
   pFolder = (psaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pContext = &pFolder->object.pSession->context;

   if ( pStatus == NULL ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
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
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
         psaCommonUnlock( &pFolder->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
      
   if ( errcode != PSO_OK ) {
      pscSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

