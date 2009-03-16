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

#include "Nucleus/Folder.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/HashMap.h"
#include "Nucleus/FastMap.h"
#include "Nucleus/Queue.h"
#include "Nucleus/MemoryHeader.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The next 7 functions should only be used by the API, to create, destroy,
 * open or close a memory object.
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderCloseObject( psonFolderItem     * pFolderItem,
                               psonSessionContext * pContext )
{
   psonFolder   * parentFolder;
   psonTreeNode * pNode;
   psonTxStatus * txItemStatus, * txFolderStatus;
   psonObjectDescriptor * pDesc;
   
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );

   GET_PTR( pDesc, pFolderItem->pHashItem->dataOffset, psonObjectDescriptor );
   GET_PTR( pNode, pDesc->nodeOffset, psonTreeNode);
   
   /* Special case, the top folder */
   if ( pNode->myParentOffset == PSON_NULL_OFFSET ) return true;

   GET_PTR( parentFolder, pNode->myParentOffset, psonFolder );
   GET_PTR( txFolderStatus, parentFolder->nodeObject.txStatusOffset, psonTxStatus );
   
   if ( psonLock( &parentFolder->memObject, pContext ) ) {
      GET_PTR( txItemStatus, pNode->txStatusOffset, psonTxStatus );
      txItemStatus->parentCounter--;
      txFolderStatus->usageCounter--;
      
      /* 
       * if parentCounter is equal to zero, the object is not open. Since 
       * we hold the lock on the folder, no session can therefore open it
       * or use it in an iteration. We can remove it without problems if
       * a remove was indeed committed.
       */
      if ( (txItemStatus->parentCounter == 0) && 
         (txItemStatus->usageCounter == 0) ) {
         if ( txItemStatus->status & PSON_TXS_DESTROYED_COMMITTED ||
            txItemStatus->status & PSON_TXS_EDIT_COMMITTED ) {
            /* Time to really delete the object and the record! */
            psonFolderRemoveObject( parentFolder,
                                    pFolderItem->pHashItem,
                                    pContext );
         }
      }
      psonUnlock( &parentFolder->memObject, pContext );

      return true;
   }
   
   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_ENGINE_BUSY );
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderCreateObject( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                psoObjectDefinition * pDefinition,
                                const unsigned char * pKeyDef,
                                uint32_t              keyDefLength,
                                const unsigned char * pDataDef,
                                uint32_t              dataDefLength,
                                psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   bool ok;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pDefinition->type > 0 && 
                      pDefinition->type < PSO_LAST_OBJECT_TYPE );
   if ( pDefinition->type != PSO_FOLDER ) {
      PSO_PRE_CONDITION( pDataDef  != NULL );
   }
   
   strLength = nameLengthInBytes;

   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char*)malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderInsertObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      ok = psonFolderInsertObject( pFolder,
                                   &(lowerName[first]),
                                   &(name[first]),
                                   strLength, 
                                   pDefinition,
                                   pKeyDef,
                                   keyDefLength,
                                   pDataDef,
                                   dataDefLength,
                                   1, /* numBlocks, */
                                   0, /* expectedNumOfChilds, */
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderDestroyObject( psonFolder         * pFolder,
                                 const char         * objectName,
                                 uint32_t             nameLengthInBytes,
                                 psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
      if ( strLength == 0 ) {
         errcode = PSO_INVALID_OBJECT_NAME;
         goto error_handler;
      }
   }

   /*
    * There is no psonUnlock here - the recursive nature of the 
    * function psonFolderDeleteObject() means that it will release 
    * the lock as soon as it can, after locking the
    * next folder in the chain if needed. 
    */
   if ( psonLock( &pFolder->memObject, pContext ) ) {
      ok = psonFolderDeleteObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength,
                                   pContext );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_ENGINE_BUSY;
      goto error_handler;
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderEditObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psonFolderItem     * pFolderItem,
                              psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   bool ok;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   strLength = nameLengthInBytes;
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* 
       * Opening the top folder (special case). No lock is needed here
       * since all we do is to retrieve the pointer (and we do not 
       * count access since the object is undeletable).
       */
      if ( objectType != PSO_FOLDER ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((psonMemoryHeader *) g_pBaseAddr)->topHashItem;
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderEditObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderEditObject( pFolder,
                                    &(lowerName[first]), 
                                    strLength, 
                                    objectType,
                                    pFolderItem,
                                    pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderGetDef( psonFolder          * pFolder,
                          const char          * objectName,
                          uint32_t              nameLengthInBytes,
                          psoObjectDefinition * pDefinition,
                          unsigned char      ** ppKeyDef,
                          uint32_t            * pKeyDefLength,
                          unsigned char      ** ppDataDef,
                          uint32_t            * pDataDefLength,
                          psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   bool ok;
   uint32_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder       != NULL );
   PSO_PRE_CONDITION( objectName    != NULL );
   PSO_PRE_CONDITION( pDefinition   != NULL );
   PSO_PRE_CONDITION( ppDataDef != NULL );
   PSO_PRE_CONDITION( pContext      != NULL );

   *ppDataDef = NULL;
   
   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* Getting the status of the top folder (special case). */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         pDefinition->type = PSO_FOLDER;
         
         psonUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetDefinition() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetDefinition( pFolder,
                                       &(lowerName[first]), 
                                       strLength, 
                                       pDefinition,
                                       ppKeyDef,
                                       pKeyDefLength,
                                       ppDataDef,
                                       pDataDefLength,
                                       pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderGetDefLength( psonFolder          * pFolder,
                                const char          * objectName,
                                uint32_t              nameLengthInBytes,
                                uint32_t            * pKeyDefLength,
                                uint32_t            * pDataDefLength,
                                psonSessionContext  * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   bool ok;
   uint32_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder        != NULL );
   PSO_PRE_CONDITION( objectName     != NULL );
   PSO_PRE_CONDITION( pKeyDefLength  != NULL );
   PSO_PRE_CONDITION( pDataDefLength != NULL );
   PSO_PRE_CONDITION( pContext       != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   *pKeyDefLength  = 0;
   *pDataDefLength = 0;
   
   if ( strLength == 0 ) {
      /* There is nothing to do left for this special case (top folder). */
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetDefinition() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetDefLength( pFolder,
                                      &(lowerName[first]), 
                                      strLength, 
                                      pKeyDefLength,
                                      pDataDefLength,
                                      pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderGetStatus( psonFolder         * pFolder,
                             const char         * objectName,
                             uint32_t             nameLengthInBytes,
                             psoObjStatus       * pStatus,
                             psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   bool ok;
   uint32_t first = 0;

   const char * name = objectName;
   char * lowerName = NULL;

   PSO_PRE_CONDITION( pFolder    != NULL );
   PSO_PRE_CONDITION( pStatus    != NULL );
   PSO_PRE_CONDITION( objectName != NULL );
   PSO_PRE_CONDITION( pContext   != NULL );

   strLength = nameLengthInBytes;
   
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowecase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* Getting the status of the top folder (special case). */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         psonMemObjectStatus( &pFolder->memObject, pStatus );
         pStatus->type = PSO_FOLDER;
         
         psonFolderMyStatus( pFolder, pStatus );

         psonUnlock( &pFolder->memObject, pContext );
      }
      else {
         errcode = PSO_OBJECT_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetStatus() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetStatus( pFolder,
                                   &(lowerName[first]), 
                                   strLength, 
                                   pStatus,
                                   pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonTopFolderOpenObject( psonFolder         * pFolder,
                              const char         * objectName,
                              uint32_t             nameLengthInBytes,
                              enum psoObjectType   objectType, 
                              psonFolderItem     * pFolderItem,
                              psonSessionContext * pContext )
{
   psoErrors errcode = PSO_OK;
   uint32_t strLength, i;
   uint32_t first = 0;
   const char * name = objectName;
   char * lowerName = NULL;
   bool ok;

   PSO_PRE_CONDITION( pFolder     != NULL );
   PSO_PRE_CONDITION( pFolderItem != NULL );
   PSO_PRE_CONDITION( objectName  != NULL );
   PSO_PRE_CONDITION( pContext    != NULL );
   PSO_PRE_CONDITION( objectType > 0 && objectType < PSO_LAST_OBJECT_TYPE );

   strLength = nameLengthInBytes;
   if ( strLength > PSO_MAX_FULL_NAME_LENGTH ) {
      errcode = PSO_OBJECT_NAME_TOO_LONG;
      goto error_handler;
   }
   if ( strLength == 0 ) {
      errcode = PSO_INVALID_OBJECT_NAME;
      goto error_handler;
   }

   lowerName = (char *) malloc( (strLength+1)*sizeof(char) );
   if ( lowerName == NULL ) {
      errcode = PSO_NOT_ENOUGH_HEAP_MEMORY;
      goto error_handler;
   }

   /* lowercase the string */
   for ( i = 0; i < strLength; ++i ) {
      lowerName[i] = (char) tolower( name[i] );
   }
   
   /* strip the first char if a separator */
   if ( name[0] == '/' || name[0] == '\\' ) {
      first = 1;
      --strLength;
   }

   if ( strLength == 0 ) {
      /* 
       * Opening the top folder (special case). No lock is needed here
       * since all we do is to retrieve the pointer (and we do not 
       * count access since the object is undeletable).
       */
      if ( objectType != PSO_FOLDER ) {
         errcode = PSO_WRONG_OBJECT_TYPE;
         goto error_handler;
      }
      pFolderItem->pHashItem = &((psonMemoryHeader *) g_pBaseAddr)->topHashItem;
   }
   else {
      /*
       * There is no psonUnlock here - the recursive nature of the 
       * function psonFolderGetObject() means that it will release 
       * the lock as soon as it can, after locking the
       * next folder in the chain if needed. 
       */
      if ( psonLock( &pFolder->memObject, pContext ) ) {
         ok = psonFolderGetObject( pFolder,
                                   &(lowerName[first]), 
                                   strLength, 
                                   objectType,
                                   pFolderItem,
                                   pContext );
         PSO_POST_CONDITION( ok == true || ok == false );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_ENGINE_BUSY;
         goto error_handler;
      }
   }
   
   free( lowerName );
   
   return true;

error_handler:

   if ( lowerName != NULL ) free( lowerName );

   /*
    * On failure, errcode would be non-zero, unless the failure occurs in
    * some other function which already called psocSetError. 
    */
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
   }
   
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

