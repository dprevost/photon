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
#include <photon/photon>
#include <photon/FastMap.h>
#include "API/FastMap.h"
#include "API/Session.h"

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMap::FastMap()
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( NULL )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMap::FastMap( Session &session, const std::string & hashMapName )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::FastMap", PSO_NULL_HANDLE );
   }

   rc = psoFastMapOpen( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::FastMap" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMap::~FastMap()
{
   if ( m_objectHandle != NULL ) {
      psoFastMapClose( m_objectHandle );
   }
   
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMap::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Close", PSO_NULL_HANDLE );
   }
   
   rc = psoFastMapClose( m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMap::Get( const void * key,
                   uint32_t     keyLength,
                   void       * buffer,
                   uint32_t     bufferLength,
                   uint32_t   & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Get", PSO_NULL_HANDLE );
   }

   rc = psoFastMapGet( m_objectHandle,
                       key,
                       keyLength,
                       buffer,
                       bufferLength,
                       &returnedLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
#if 0
FieldDefinition * FastMap::GetFieldDefinition()
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   FieldDefinition * pFieldDef = NULL;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetFieldDefinition", PSO_NULL_HANDLE );
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
      throw pso::Exception( m_sessionHandle, "FastMap::GetFieldDefinition" );
   }   
   
   return pFieldDef;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition * FastMap::GetDataDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle, keyDefHandle;
   DataDefinition * pDefinition;

   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetDataDefinition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDefinition( m_objectHandle,
                              &keyDefHandle,
                              &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::GetDataDefinition" );
   }
   
   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
KeyDefinition * FastMap::GetKeyDefinition()
{
   psoaFastMap * pHashMap;
   psonFastMap * pMemHashMap;
   int errcode = PSO_OK;
   psonSessionContext * pContext;
   KeyDefinition * pKeyDef = NULL;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetKeyDefinition", PSO_NULL_HANDLE );
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
      throw pso::Exception( m_sessionHandle, "FastMap::GetKeyDefinition" );
   }   
   
   return pKeyDef;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int FastMap::GetFirst( void       * key,
                       uint32_t     keyLength,
                       void       * buffer,
                       uint32_t     bufferLength,
                       uint32_t   & retKeyLength,
                       uint32_t   & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetFirst", PSO_NULL_HANDLE );
   }
   
   rc = psoFastMapGetFirst( m_objectHandle,
                            key,
                            keyLength,
                            buffer,
                            bufferLength,
                            &retKeyLength,
                            &retDataLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw pso::Exception( m_sessionHandle, "FastMap::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition * FastMap::GetKeyDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle, keyDefHandle;
   KeyDefinition * pDefinition;

   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetKeyDefinition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDefinition( m_objectHandle,
                              &keyDefHandle,
                              &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::GetKeyDefinition" );
   }
   
   pDefinition = new KeyDefinition( m_sessionHandle, keyDefHandle );

   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int FastMap::GetNext( void       * key,
                      uint32_t     keyLength,
                      void       * buffer,
                      uint32_t     bufferLength,
                      uint32_t   & retKeyLength,
                      uint32_t   & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoFastMapGetNext( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           &retKeyLength,
                           &retDataLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw pso::Exception( m_sessionHandle, "FastMap::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition * FastMap::GetRecordDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle;
   DataDefinition * pDefinition;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::GetRecordDefinition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapRecordDefinition( m_objectHandle,
                                    &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::GetRecordDefinition" );
   }

   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMap::Open( Session &session, const std::string & hashMapName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Open", PSO_NULL_HANDLE );
   }

   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "FastMap::Open", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;

   rc = psoFastMapOpen( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMap::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Status", PSO_NULL_HANDLE );
   }

   rc = psoFastMapStatus( m_objectHandle, &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

