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
#include "API/DataDefinition.h"
#include <photon/DataDefinition.h>
#include "API/Session.h"
#include "Nucleus/HashMap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoDataDefClose( PSO_HANDLE definitionHandle )
{
   psoaDataDefinition * pDefinition;
   int errcode = PSO_OK;
   
   pDefinition = (psoaDataDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_DATA ) return PSO_WRONG_TYPE_HANDLE;

   if ( ! pDefinition->pSession->terminated ) {
      if ( psoaSessionLock( pDefinition->pSession ) ) {
         
         if ( pDefinition->ppApiObject != NULL ) {
            *pDefinition->ppApiObject = NULL;
         }
         /*
          * Memory might still be around even after it is released, so we 
          * make sure future access with the handle fails by setting the 
          * type wrong.
          */
         pDefinition->definitionType = 0; 
         free( pDefinition );
         
         psoaSessionUnlock(pDefinition->pSession);
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   return errcode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoDataDefCreate( PSO_HANDLE               sessionHandle,
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
   psonDataDefinition * pMemDefinition = NULL;
   uint32_t recLength;
   psoaDataDefinition * pDefinition = NULL;
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
   recLength = offsetof( psonDataDefinition, definition ) + dataDefLength;
   pMemDefinition = malloc( recLength );
   if ( pMemDefinition == NULL ) {
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   pMemDefinition->type = type;
   pMemDefinition->definitionLength = dataDefLength;
   memcpy( pMemDefinition->definition, dataDef, dataDefLength );

   /* Create our proxy object */
   pDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDefinition == NULL ) {
      free( pMemDefinition );
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( ! pSession->terminated ) {
      if ( psoaSessionLock( pSession ) ) {
         ok = psonHashMapInsert( pSession->pDataDefMap,
                                 definitionName,
                                 nameLengthInBytes,
                                 pMemDefinition,
                                 recLength,
                                 NULL,
                                 &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         
         if ( ok ) {
            ok = psonHashMapGet( pSession->pDataDefMap,
                                 definitionName,
                                 nameLengthInBytes,
                                 &pHashItem,
                                 (uint32_t) -1,
                                 &pSession->context );
            PSO_POST_CONDITION( ok == true || ok == false );
         }
         psoaSessionUnlock( pSession );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   free( pMemDefinition );
   pDefinition->pSession = pSession;
   pDefinition->definitionType = PSOA_DEF_DATA;
   GET_PTR( pMemDefinition, pHashItem->dataOffset, psonDataDefinition );
   pDefinition->pMemDefinition = pMemDefinition;
   pDefinition->ppApiObject = NULL;
   
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

int psoDataDefGet( PSO_HANDLE               definitionHandle,
                   enum psoDefinitionType * type,
                   unsigned char          * dataDef,
                   psoUint32                dataDefLength )
{
   psoaDataDefinition * pDefinition;

   pDefinition = (psoaDataDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_DATA ) return PSO_WRONG_TYPE_HANDLE;

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

int psoDataDefGetLength( PSO_HANDLE   definitionHandle,
                         psoUint32  * dataDefLength )
{
   psoaDataDefinition * pDefinition;

   pDefinition = (psoaDataDefinition *) definitionHandle;
   if ( pDefinition == NULL ) return PSO_NULL_HANDLE;
   
   if ( pDefinition->definitionType != PSOA_DEF_DATA ) return PSO_WRONG_TYPE_HANDLE;

   if ( dataDefLength == NULL ) {
      psocSetError( &pDefinition->pSession->context.errorHandler, g_psoErrorHandle, PSO_NULL_POINTER );
      return PSO_NULL_POINTER;
   }

   *dataDefLength = pDefinition->pMemDefinition->definitionLength;
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int psoDataDefOpen( PSO_HANDLE   sessionHandle,
                    const char * definitionName,
                    psoUint32    nameLengthInBytes,
                    PSO_HANDLE * definitionHandle )
{
   int errcode = PSO_OK;
   psoaSession* pSession;
   bool ok = true;
   psonDataDefinition * pMemDefinition = NULL;
   psoaDataDefinition * pDefinition = NULL;
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
   pDefinition = malloc( sizeof(psoaDataDefinition) );
   if ( pDefinition == NULL ) {
      free( pMemDefinition );
      psocSetError( &pSession->context.errorHandler, g_psoErrorHandle, PSO_NOT_ENOUGH_HEAP_MEMORY );
      return PSO_NOT_ENOUGH_HEAP_MEMORY;
   }
   
   if ( ! pSession->terminated ) {
      if ( psoaSessionLock( pSession ) ) {
         ok = psonHashMapGet( pSession->pDataDefMap,
                              definitionName,
                              nameLengthInBytes,
                              &pHashItem,
                              (uint32_t) -1,
                              &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         psoaSessionUnlock( pSession );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

   pDefinition->pSession = pSession;
   pDefinition->definitionType = PSOA_DEF_DATA;
   GET_PTR( pMemDefinition, pHashItem->dataOffset, psonDataDefinition );
   pDefinition->pMemDefinition = pMemDefinition;
   
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

void psoaGetFieldOffsets( psoFieldDefinition * pDefinition,
                          int                  numFields,
                          uint32_t           * pOffsets )
{
   int i;
   uint32_t minLength = 0;

   PSO_PRE_CONDITION( pDefinition != NULL );
   PSO_PRE_CONDITION( pOffsets    != NULL );
   PSO_PRE_CONDITION( numFields > 0 );
   
   /*
    * The first field is special - the alignment offset is always zero
    * since we just started.
    */
   pOffsets[0] = 0;

   switch( pDefinition[0].type ) {

   case PSO_CHAR:
   case PSO_BINARY:
      minLength = pDefinition[0].vals.length;
      break;
      
   case PSO_VARCHAR:
   case PSO_LONGVARCHAR:
   case PSO_VARBINARY:
   case PSO_LONGVARBINARY:
      /* A single field - a single offset ! */
      break;

   case PSO_TINYINT:
      minLength = 1;
      break;

   case PSO_SMALLINT:
      minLength = 4;
      break;

   case PSO_INTEGER:
      minLength = 4;
      break;
      
   case PSO_BIGINT:
      minLength = 8;
      break;
   
   case PSO_REAL:
      minLength = 4;
      break;
   
   case PSO_DOUBLE:
      minLength = 8;
      break;
   
   case PSO_NUMERIC:
      minLength = sizeof(struct psoNumericStruct);
      break;
   
   case PSO_DATE:
      minLength = sizeof(struct psoDateStruct);
      break;

   case PSO_TIME:
      minLength = sizeof(struct psoTimeStruct);
      break;

   case PSO_TIMESTAMP:
      minLength = sizeof(struct psoTimeStampStruct);
      break;
   }
   
   for ( i = 1; i < numFields; ++i ) {

      switch( pDefinition[i].type ) {

      case PSO_CHAR:
      case PSO_BINARY:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;
         minLength += pDefinition[i].vals.length;
         break;
         
      case PSO_VARCHAR:
      case PSO_LONGVARCHAR:
      case PSO_VARBINARY:
      case PSO_LONGVARBINARY:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;
         break;
      
      case PSO_TINYINT:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_CHAR + 1)*PSOC_ALIGNMENT_CHAR;
         pOffsets[i] = minLength;
         minLength += 1;
         break;
      
      case PSO_SMALLINT:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT16 + 1)*PSOC_ALIGNMENT_INT16;
         pOffsets[i] = minLength;
         minLength += 2;
         break;

      case PSO_INTEGER:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT32 + 1)*PSOC_ALIGNMENT_INT32;
         pOffsets[i] = minLength;
         minLength += 4;
         break;
         
      case PSO_BIGINT:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT64 + 1)*PSOC_ALIGNMENT_INT64;
         pOffsets[i] = minLength;
         minLength += 8;
         break;

      case PSO_REAL:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT32 + 1)*PSOC_ALIGNMENT_INT32;
         pOffsets[i] = minLength;
         minLength += 4;
         break;

      case PSO_DOUBLE:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_INT64 + 1)*PSOC_ALIGNMENT_INT64;
         pOffsets[i] = minLength;
         minLength += 8;
         break;

      case PSO_NUMERIC:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoNumericStruct);
         break;

      case PSO_DATE:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoDateStruct);
         break;

      case PSO_TIME:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoTimeStruct);
         break;

      case PSO_TIMESTAMP:
         minLength = ((minLength-1)/PSOC_ALIGNMENT_STRUCT + 1)*PSOC_ALIGNMENT_STRUCT;
         pOffsets[i] = minLength;
         minLength += sizeof(struct psoTimeStampStruct);
         break;

      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This function is not included in the published API.
 *
 * This function can be dangerous. Handles to data definition are 
 * not counted for performance reasons -> this might destroy a
 * definition which is used by someone else...
 */
PHOTON_API_EXPORT
int psoaDataDefDestroy( PSO_HANDLE   sessionHandle,
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
      if ( psoaSessionLock( pSession ) ) {
         ok = psonHashMapDelete( pSession->pDataDefMap,
                                 definitionName,
                                 nameLengthInBytes,
                                 &pSession->context );
         PSO_POST_CONDITION( ok == true || ok == false );
         psoaSessionUnlock( pSession );
         if ( ! ok ) goto error_handler;
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
         goto error_handler;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
      goto error_handler;
   }

error_handler:

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


