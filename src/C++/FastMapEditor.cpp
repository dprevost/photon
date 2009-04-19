/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/photon>
#include <photon/FastMap.h>
#include <photon/Session>
#include <photon/psoErrors.h>
#include <photon/Exception>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMapEditor::FastMapEditor()
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( NULL )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMapEditor::FastMapEditor( Session & session, 
                              const std::string & hashMapName )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::FastMapEditor", PSO_NULL_HANDLE );
   }

   rc = psoFastMapEdit( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::FastMapEditor" );
   }

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMapEditor::~FastMapEditor()
{
   if ( m_objectHandle != NULL ) {
      psoFastMapClose( m_objectHandle );
   }
   
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Close", PSO_NULL_HANDLE );
   }
   
   rc = psoFastMapClose( m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Delete( const void * key,
                            uint32_t     keyLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Delete", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDelete( m_objectHandle,
                          key,
                          keyLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Delete" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Empty()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Empty", PSO_NULL_HANDLE );
   }

   rc = psoFastMapEmpty( m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Empty" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Get( const void * key,
                         uint32_t     keyLength,
                         void       * buffer,
                         uint32_t     bufferLength,
                         uint32_t   & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Get", PSO_NULL_HANDLE );
   }

   rc = psoFastMapGet( m_objectHandle,
                       key,
                       keyLength,
                       buffer,
                       bufferLength,
                       &returnedLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#if 0
FieldDefinition * FastMapEditor::GetFieldDefinition()
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   FieldDefinition * pFieldDef = NULL;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetFieldDefinition", PSO_NULL_HANDLE );
   }

   pHashMap = (psoaFastMap *) m_objectHandle;
   pContext = &pHashMap->object.pSession->context;

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
      
         switch( pMemHashMap->fieldDefType ) {
         case PSO_DEF_PHOTON_ODBC_SIMPLE:
            pFieldDef = new FieldDefinitionODBC( pHashMap->fieldsDef, 
                                                 pHashMap->fieldsDefLength );
            break;
         default:
            break;
         }
         
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetFieldDefinition" );
   }   
   
   return pFieldDef;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition * FastMapEditor::GetDataDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle, keyDefHandle;
   DataDefinition * pDefinition;

   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetDataDefinition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDefinition( m_objectHandle,
                              &keyDefHandle,
                              &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetDataDefinition" );
   }
   
   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
KeyDefinition * FastMapEditor::GetKeyDefinition()
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   KeyDefinition * pKeyDef = NULL;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetKeyDefinition", PSO_NULL_HANDLE );
   }

   pHashMap = (psoaFastMap *) m_objectHandle;
   pContext = &pHashMap->object.pSession->context;

   if ( ! pHashMap->object.pSession->terminated ) {
      if ( psoaCommonLock( &pHashMap->object ) ) {
         pMemHashMap = (psonFastMap *) pHashMap->object.pMyMemObject;
      
         switch( pMemHashMap->keyDefType ) {
         case PSO_DEF_PHOTON_ODBC_SIMPLE:
            pKeyDef = new KeyDefinitionODBC( pHashMap->keyDef, 
                                             pHashMap->keyDefLength );
            break;
         default:
            break;
         }
         
         psoaCommonUnlock( &pHashMap->object );
      }
      else {
         errcode = PSO_SESSION_CANNOT_GET_LOCK;
      }
   }
   else {
      errcode = PSO_SESSION_IS_TERMINATED;
   }
   
   if ( errcode != PSO_OK ) {
      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetKeyDefinition" );
   }   
   
   return pKeyDef;
}
#endif
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int FastMapEditor::GetFirst( void     * key,
                             uint32_t   keyLength,
                             void     * buffer,
                             uint32_t   bufferLength,
                             uint32_t & retKeyLength,
                             uint32_t & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetFirst", PSO_NULL_HANDLE );
   }
   
   rc = psoFastMapGetFirst( m_objectHandle,
                            key,
                            keyLength,
                            buffer,
                            bufferLength,
                            &retKeyLength,
                            &retDataLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition * FastMapEditor::GetKeyDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle, keyDefHandle;
   KeyDefinition * pDefinition;

   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetKeyDefinition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDefinition( m_objectHandle,
                              &keyDefHandle,
                              &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetKeyDefinition" );
   }
   
   pDefinition = new KeyDefinition( m_sessionHandle, keyDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int FastMapEditor::GetNext( void     * key,
                            uint32_t   keyLength,
                            void     * buffer,
                            uint32_t   bufferLength,
                            uint32_t & retKeyLength,
                            uint32_t & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoFastMapGetNext( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           &retKeyLength,
                           &retDataLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition * FastMapEditor::GetRecordDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle;
   DataDefinition * pDefinition;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::GetRecordDefinition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapRecordDefinition( m_objectHandle,
                                    &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::GetRecordDefinition" );
   }

   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Insert( const void * key,
                            uint32_t     keyLength,
                            const void * data,
                            uint32_t     dataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Insert", PSO_NULL_HANDLE );
   }

   rc = psoFastMapInsert( m_objectHandle,
                          key,
                          keyLength,
                          data,
                          dataLength,
                          NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Insert( const void           * key,
                            uint32_t               keyLength,
                            const void           * data,
                            uint32_t               dataLength,
                            const DataDefinition & dataDefinition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Insert", PSO_NULL_HANDLE );
   }

   rc = psoFastMapInsert( m_objectHandle,
                          key,
                          keyLength,
                          data,
                          dataLength,
                          dataDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Open( Session & session, 
                          const std::string & hashMapName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Open", PSO_NULL_HANDLE );
   }

   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "FastMapEditor::Open", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;

   rc = psoFastMapEdit( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Replace( const void * key,
                             uint32_t     keyLength,
                             const void * data,
                             uint32_t     dataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Replace", PSO_NULL_HANDLE );
   }

   rc = psoFastMapReplace( m_objectHandle,
                           key,
                           keyLength,
                           data,
                           dataLength,
                           NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Replace( const void           * key,
                             uint32_t               keyLength,
                             const void           * data,
                             uint32_t               dataLength,
                             const DataDefinition & dataDefinition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Replace", PSO_NULL_HANDLE );
   }

   rc = psoFastMapReplace( m_objectHandle,
                           key,
                           keyLength,
                           data,
                           dataLength,
                           dataDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Status", PSO_NULL_HANDLE );
   }

   rc = psoFastMapStatus( m_objectHandle, &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

