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
#include <photon/psoFolder.h>
#include "API/Folder.h"
#include "API/Session.h"
#include <photon/psoErrors.h>
#include "API/CommonObject.h"
#include "API/DataDefinition.h"
#include "API/KeyDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 * 
 * This file contains the source code for some functions of the C API. 
 * For documentation for these functions, please see the distributed
 * header files.
 * 
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderClose( PSO_HANDLE objectHandle )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( ! pFolder->object.pSession->terminated ) {

     pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

      /* Reinitialize the iterator, if needed */
      if ( pFolder->iterator.pHashItem != NULL ) {
         ok = psonFolderRelease( pMemFolder,
                                 &pFolder->iterator,
                                 &pFolder->object.pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ok ) {
            memset( &pFolder->iterator, 0, sizeof(psonFolderItem) );
         }
         else {
            errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         }
      }

      if ( errcode == PSO_OK ) {
         errcode = psoaCommonObjClose( &pFolder->object );
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
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateFolder( PSO_HANDLE            objectHandle,
                           const char          * objectName,
                           uint32_t              nameLengthInBytes )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psoaSession * pSession;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pSession->terminated ) {
      pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

      ok = psonFolderCreateFolder( pMemFolder,
                                   objectName,
                                   nameLengthInBytes,
                                   &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateObject( PSO_HANDLE            objectHandle,
                           const char          * objectName,
                           uint32_t              nameLengthInBytes,
                           psoObjectDefinition * pDefinition,
                           PSO_HANDLE            dataDefHandle )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psoaSession * pSession;
   psonDataDefinition * pMemDataDefinition = NULL;
   psoaDataDefinition * pDataDefinition;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( pDefinition->type <= PSO_FOLDER || pDefinition->type >= PSO_HASH_MAP ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_OBJECT_TYPE );
      return PSO_WRONG_OBJECT_TYPE;
   }

   if ( pDefinition->type != PSO_FOLDER && dataDefHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataDefHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   pDataDefinition = (psoaDataDefinition *)dataDefHandle;
      
   if ( pDataDefinition->definitionType != PSOA_DEF_DATA ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
      return PSO_WRONG_TYPE_HANDLE;
   }
   pMemDataDefinition = pDataDefinition->pMemDefinition;

   if ( ! pSession->terminated ) {
      pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

      ok = psonFolderCreateObject( pMemFolder,
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition,
                                   NULL,
                                   pMemDataDefinition,
                                   &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderCreateKeyedObject( PSO_HANDLE            objectHandle,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                PSO_HANDLE            keyDefHandle,
                                PSO_HANDLE            dataDefHandle )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   bool ok = true;
   psoaSession * pSession;
   psonDataDefinition * pMemDataDefinition = NULL;
   psonKeyDefinition  * pMemKeyDefinition = NULL;
   psoaDataDefinition * pDataDefinition;
   psoaKeyDefinition  * pKeyDefinition;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( pDefinition->type < PSO_HASH_MAP || pDefinition->type > PSO_FAST_MAP ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_OBJECT_TYPE );
      return PSO_WRONG_OBJECT_TYPE;
   }

   if ( keyDefHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   pKeyDefinition = (psoaKeyDefinition *)keyDefHandle;      
   if ( pKeyDefinition->definitionType != PSOA_DEF_KEY ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
      return PSO_WRONG_TYPE_HANDLE;
   }
   pMemKeyDefinition = pKeyDefinition->pMemDefinition;
   
   if ( dataDefHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   pDataDefinition = (psoaDataDefinition *)dataDefHandle;
   if ( pDataDefinition->definitionType != PSOA_DEF_DATA ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_TYPE_HANDLE );
      return PSO_WRONG_TYPE_HANDLE;
   }
   pMemDataDefinition = pDataDefinition->pMemDefinition;

   if ( ! pSession->terminated ) {
      pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

      ok = psonFolderCreateObject( pMemFolder,
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition,
                                   pMemKeyDefinition,
                                   pMemDataDefinition,
                                   &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderDestroyObject( PSO_HANDLE   objectHandle,
                            const char * objectName,
                            uint32_t     nameLengthInBytes )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok = true;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( ! pSession->terminated ) {
      pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

      ok = psonFolderDestroyObject( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   return errcode;
}
    
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetDefinition( PSO_HANDLE            objectHandle,
                            const char          * objectName,
                            uint32_t              nameLengthInBytes,
                            psoObjectDefinition * pDefinition,
                            PSO_HANDLE          * keyDefHandle,
                            PSO_HANDLE          * dataDefHandle )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   psoaSession * pSession;
   int errcode = PSO_OK;
   bool ok = true;
   psoaDataDefinition * pDataDefinition = NULL;
   psoaKeyDefinition  * pKeyDefinition = NULL;

   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   pSession = pFolder->object.pSession;

   if ( objectName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }

   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   if ( pDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( keyDefHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( dataDefHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   pDataDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDataDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   pKeyDefinition = malloc( sizeof(psoaKeyDefinition) );
   if ( pKeyDefinition == NULL ) {
      free( pDataDefinition );
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }

   if ( ! pSession->terminated ) {
      pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

      ok = psonFolderGetDefinition( pMemFolder,
                                    objectName,
                                    nameLengthInBytes,
                                    pDefinition,
                                    &pKeyDefinition->pMemDefinition,
                                    &pDataDefinition->pMemDefinition,
                                    &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         *dataDefHandle = (PSO_HANDLE) pDataDefinition;
         *keyDefHandle = (PSO_HANDLE) pKeyDefinition;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }
   
   if ( errcode != PSO_OK ) {
      if ( pDataDefinition ) free(pDataDefinition);
      if ( pKeyDefinition) free(pKeyDefinition);
   }
   
   return errcode;
}   

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetFirst( PSO_HANDLE       objectHandle,
                       psoFolderEntry * pEntry )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   psonObjectDescriptor * pDescriptor;
   bool ok;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
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
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   /* Reinitialize the iterator, if needed */
   if ( pFolder->iterator.pHashItem != NULL ) {
      ok = psonFolderRelease( pMemFolder,
                              &pFolder->iterator,
                              &pFolder->object.pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ok ) {
         memset( &pFolder->iterator, 0, sizeof(psonFolderItem) );
      }
      else {
         goto error_handler;
      }
   }

   ok = psonFolderGetFirst( pMemFolder,
                            &pFolder->iterator,
                            &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;

   memset( pEntry, 0, sizeof( psoFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         psonObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   } 
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderGetNext( PSO_HANDLE       objectHandle,
                      psoFolderEntry * pEntry )
{
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   psonObjectDescriptor * pDescriptor;
   bool ok;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
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
   
   pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;

   ok = psonFolderGetNext( pMemFolder,
                           &pFolder->iterator,
                           &pFolder->object.pSession->context );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) goto error_handler;
   
   memset( pEntry, 0, sizeof( psoFolderEntry ) );
   GET_PTR( pDescriptor, pFolder->iterator.pHashItem->dataOffset, 
                         psonObjectDescriptor );
   pEntry->type = pDescriptor->apiType;
   pEntry->status = pFolder->iterator.status;
   pEntry->nameLengthInBytes = pDescriptor->nameLengthInBytes;
   memcpy( pEntry->name, pDescriptor->originalName, pDescriptor->nameLengthInBytes );

   return PSO_OK;

error_handler:
   if ( errcode != PSO_OK ) {
      psocSetError( &pFolder->object.pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   else {
      errcode = psocGetLastError( &pFolder->object.pSession->context.errorHandler );
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoFolderOpen( PSO_HANDLE   sessionHandle,
                   const char * folderName,
                   uint32_t     nameLengthInBytes,
                   PSO_HANDLE * objectHandle )
{
   psoaSession * pSession;
   psoaFolder * pFolder = NULL;
   int errcode;
   
   if ( objectHandle == NULL ) return PSO_NULL_HANDLE;
   *objectHandle = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   if ( folderName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_OBJECT_NAME );
      return PSO_INVALID_OBJECT_NAME;
   }
   
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   pFolder = (psoaFolder *) malloc(sizeof(psoaFolder));
   if (  pFolder == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   memset( pFolder, 0, sizeof(psoaFolder) );
   pFolder->object.type = PSOA_FOLDER;
   pFolder->object.pSession = pSession;

   if ( ! pFolder->object.pSession->terminated ) {

      errcode = psoaCommonObjOpen( &pFolder->object,
                                   PSO_FOLDER,
                                   PSOA_READ_WRITE,
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
   psoaFolder * pFolder;
   psonFolder * pMemFolder;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   
   pFolder = (psoaFolder *) objectHandle;
   if ( pFolder == NULL ) return PSO_NULL_HANDLE;
   
   if ( pFolder->object.type != PSOA_FOLDER ) {
      return PSO_WRONG_TYPE_HANDLE;
   }
   
   pContext = &pFolder->object.pSession->context;

   if ( pStatus == NULL ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   if ( ! pFolder->object.pSession->terminated ) {

      pMemFolder = (psonFolder *) pFolder->object.pMyMemObject;
      
      if ( psonLock(&pMemFolder->memObject, pContext) ) {

         psonMemObjectStatus( &pMemFolder->memObject, pStatus );

         psonFolderMyStatus( pMemFolder, pStatus );
         pStatus->type = PSO_FOLDER;
            
         psonUnlock( &pMemFolder->memObject, pContext );
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

