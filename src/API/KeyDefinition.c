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
#include "API/KeyDefinition.h"
#include <photon/KeyDefinition.h>
#include "API/Session.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoKeyDefClose( PSO_HANDLE definitionHandle )
{
   psoaKeyDefinition * pDefinition;

   pDefinition = (psoaKeyDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_KEY ) return PSO_WRONG_TYPE_HANDLE;

   /*
    * Memory might still be around even after it is released, so we make 
    * sure future access with the handle fails by setting the type wrong!
    */
   pDefinition->definitionType = 0; 
   free( pDefinition );
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoKeyDefCreate( PSO_HANDLE               sessionHandle,
                      const char             * definitionName,
                      psoUint32                nameLengthInBytes,
                      enum psoDefinitionType   type,
                      const unsigned char    * dataDef,
                      psoUint32                dataDefLength,
                      PSO_HANDLE             * definitionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok = true;
   psonKeyDefinition * pMemDefinition = NULL;
   uint32_t recLength;
   psoaKeyDefinition * pDefinition = NULL;
   psonHashTxItem * pHashItem = NULL;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;

   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( definitionName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   if ( type <= PSO_DEF_FIRST_TYPE || type >= PSO_DEF_LAST_TYPE ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_WRONG_OBJECT_TYPE );
      return PSO_WRONG_OBJECT_TYPE;
   }
   if ( dataDef == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( dataDefLength == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   if ( definitionHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   /* We need to serialize the inputs to insert the record in the hash map */
   recLength = offsetof( psonKeyDefinition, definition ) + dataDefLength;
   pMemDefinition = malloc( recLength );
   if ( pMemDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   pMemDefinition->type = type;
   pMemDefinition->definitionLength = dataDefLength;
   memcpy( pMemDefinition->definition, dataDef, dataDefLength );

   /* Create our proxy object */
   pDefinition = malloc( sizeof(psoaKeyDefinition) );
   if ( pDefinition == NULL ) {
      free( pMemDefinition );
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( ! pSession->terminated ) {
      ok = psonHashMapInsert( pSession->pKeyDefMap,
                              definitionName,
                              nameLengthInBytes,
                              pMemDefinition,
                              recLength,
                              NULL,
                              &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
         
      if ( ok ) {
         ok = psonHashMapGet( pSession->pKeyDefMap,
                              definitionName,
                              nameLengthInBytes,
                              &pHashItem,
                              (uint32_t) -1,
                              &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
      }
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   free( pMemDefinition );
   pDefinition->pSession = pSession;
   pDefinition->definitionType = PSOA_DEF_KEY;
   GET_PTR( pMemDefinition, pHashItem->dataOffset, psonKeyDefinition );
   pDefinition->pMemDefinition = pMemDefinition;
   pDefinition->name = (char *)pHashItem->key;
   pDefinition->nameLength = pHashItem->keyLength;
   
   *definitionHandle = (PSO_HANDLE) pDefinition;

   return PSO_OK;
   
error_handler:

   if ( pMemDefinition ) free( pMemDefinition );
   free( pDefinition );
   
   if ( errcode != 0 ) {
      psocSetError( &pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoKeyDefGet( PSO_HANDLE               definitionHandle,
                   enum psoDefinitionType * type,
                   unsigned char          * dataDef,
                   psoUint32                dataDefLength )
{
   psoaKeyDefinition * pDefinition;

   pDefinition = (psoaKeyDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_KEY ) return PSO_WRONG_TYPE_HANDLE;

   if ( type == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( dataDef == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( dataDefLength < pDefinition->pMemDefinition->definitionLength ) {
      psocSetError( &pDefinition->pSession->context.errorHandler,
                    g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   
   *type = pDefinition->pMemDefinition->type;
   memcpy( dataDef, pDefinition->pMemDefinition->definition,
      pDefinition->pMemDefinition->definitionLength );
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoKeyDefGetLength( PSO_HANDLE   definitionHandle,
                         psoUint32  * dataDefLength )
{
   psoaKeyDefinition * pDefinition;

   pDefinition = (psoaKeyDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_KEY ) return PSO_WRONG_TYPE_HANDLE;

   if ( dataDefLength == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   *dataDefLength = pDefinition->pMemDefinition->definitionLength;
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoKeyDefOpen( PSO_HANDLE   sessionHandle,
                    const char * definitionName,
                    psoUint32    nameLengthInBytes,
                    PSO_HANDLE * definitionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok = true;
   psonKeyDefinition * pMemDefinition = NULL;
   psoaKeyDefinition * pDefinition = NULL;
   psonHashTxItem * pHashItem;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;

   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( definitionName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }
   if ( definitionHandle == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   /* Create our proxy object */
   pDefinition = malloc( sizeof(psoaKeyDefinition) );
   if ( pDefinition == NULL ) {
      free( pMemDefinition );
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( ! pSession->terminated ) {
      ok = psonHashMapGet( pSession->pKeyDefMap,
                           definitionName,
                           nameLengthInBytes,
                           &pHashItem,
                           (uint32_t) -1,
                           &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
      if ( ! ok ) goto error_handler;
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pDefinition->pSession = pSession;
   pDefinition->definitionType = PSOA_DEF_KEY;
   GET_PTR( pMemDefinition, pHashItem->dataOffset, psonKeyDefinition );
   pDefinition->pMemDefinition = pMemDefinition;
   pDefinition->name = (char *)pHashItem->key;
   pDefinition->nameLength = pHashItem->keyLength;
   
   *definitionHandle = (PSO_HANDLE) pDefinition;

   return PSO_OK;
   
error_handler:

   free( pDefinition );
   
   if ( errcode != 0 ) {
      psocSetError( &pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function is not included in the published API.
 *
 * This function can be dangerous. Handles to key definition are 
 * not counted for performance reasons -> this might destroy a
 * definition which is used by someone else...
 */
PHOTON_API_EXPORT
int psoaKeyDefDestroy( PSO_HANDLE   sessionHandle,
                       const char * definitionName,
                       psoUint32    nameLengthInBytes )
{
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok = true;

   pSession = (psoaSession*) sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;

   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( definitionName == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( nameLengthInBytes == 0 ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_INVALID_LENGTH );
      return PSO_INVALID_LENGTH;
   }

   if ( ! pSession->terminated ) {
      ok = psonHashMapDelete( pSession->pKeyDefMap,
                              definitionName,
                              nameLengthInBytes,
                              &pSession->context );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }

   if ( errcode != 0 ) {
      psocSetError( &pSession->context.errorHandler, 
         g_psoErrorHandle, errcode );
   }
   if ( ! ok ) {
      errcode = psocGetLastError( &pSession->context.errorHandler );
   }

   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoaKeyDefGetDef( PSO_HANDLE                definitionHandle,
                      char                   ** name,
                      psoUint32               * nameLength,
                      enum psoDefinitionType  * type,
                      unsigned char          ** keyDef,
                      psoUint32               * keyDefLength )
{
   psoaKeyDefinition * pDefinition;

   pDefinition = (psoaKeyDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_KEY ) return PSO_WRONG_TYPE_HANDLE;

   if ( name == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( nameLength == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( type == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   if ( keyDef == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   if ( keyDefLength == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }
   
   *type = pDefinition->pMemDefinition->type;
   *keyDef = pDefinition->pMemDefinition->definition,
   *keyDefLength = pDefinition->pMemDefinition->definitionLength;
   *name = pDefinition->name;
   *nameLength = pDefinition->nameLength;
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoaGetKeyOffsets( psoKeyFieldDefinition * pDefinition,
                        int                numKeys,
                        uint32_t         * pOffsets )
{
   int i;
   uint32_t minLength = 0;

   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pOffsets    != NULL );
   PSO_PRE_CONDITION( numKeys > 0 );
   
   /*
    * The first field is special - the alignment offset is always zero
    * since we just started.
    */
   pOffsets[0] = 0;

   switch( pDefinition[0].type ) {

   case PSO_KEY_CHAR:
   case PSO_KEY_BINARY:
      minLength = pDefinition[0].length;
      break;
      
   case PSO_KEY_VARCHAR:
   case PSO_KEY_LONGVARCHAR:
   case PSO_KEY_VARBINARY:
   case PSO_KEY_LONGVARBINARY:
      /* A single field - a single offset ! */
      break;

   case PSO_KEY_INTEGER:
      minLength = 4;
      break;
      
   case PSO_KEY_BIGINT:
      minLength = 8;
      break;
   
   case PSO_KEY_DATE:
      minLength = sizeof(struct psoDateStruct);
      break;

   case PSO_KEY_TIME:
      minLength = sizeof(struct psoTimeStruct);
      break;

   case PSO_KEY_TIMESTAMP:
      minLength = sizeof(struct psoTimeStampStruct);
      break;
   }
   
   for ( i = 1; i < numKeys; ++i ) {

      switch( pDefinition[i].type ) {

      case PSO_KEY_CHAR:
      case PSO_KEY_BINARY:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;
         minLength += pDefinition[i].length;
         break;
         
      case PSO_KEY_VARCHAR:
      case PSO_KEY_LONGVARCHAR:
      case PSO_KEY_VARBINARY:
      case PSO_KEY_LONGVARBINARY:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;
         break;
      
      case PSO_KEY_INTEGER:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT32 + 1)*PSOC_ALIGNMENT_INT32;
         pOffsets[i] = minLength;
         minLength += 4;
         break;
         
      case PSO_KEY_BIGINT:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT64 + 1)*PSOC_ALIGNMENT_INT64;
         pOffsets[i] = minLength;
         minLength += 8;
         break;

      case PSO_KEY_DATE:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoDateStruct);
         break;

      case PSO_KEY_TIME:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoTimeStruct);
         break;

      case PSO_KEY_TIMESTAMP:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoTimeStampStruct);
         break;

      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

