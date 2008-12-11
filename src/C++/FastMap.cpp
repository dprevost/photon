/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/psoFastMap>
#include <photon/psoFastMap.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>
#include <photon/psoDefinition>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMap::FastMap( Session &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
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


void FastMap::Definition( ObjDefinition & definition )
{
   int rc;
   psoObjectDefinition def;
   psoFieldDefinition * fields;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Definition", PSO_NULL_HANDLE );
   }
   
   memset( &def, 0, sizeof(psoObjectDefinition) );
   rc = psoFastMapDefinition( m_objectHandle, &def, 0, NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Definition" );
   }
   fields = (psoFieldDefinition *) 
      calloc(sizeof(psoFieldDefinition) * def.numFields, 1);
   if ( fields == NULL ) {
      throw pso::Exception( "FastMap::Definition", PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   rc = psoFastMapDefinition( m_objectHandle, &def, def.numFields, fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Definition" );
   }
   
   // We catch and rethrow the exception to avoid a memory leak
   try {
      definition.Reset( def, fields );
   }
   catch( pso::Exception exc ) {
      free( fields );
      throw exc;
   }
   
   free( fields );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMap::Definition( psoObjectDefinition & definition,
                          psoUint32             numFields,
                          psoFieldDefinition  * fields )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Definition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDefinition( m_objectHandle, &definition, numFields, fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Definition" );
   }
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

void FastMap::Open( const std::string & hashMapName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Open", PSO_NULL_HANDLE );
   }

   rc = psoFastMapOpen( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMap::Open( const char * hashMapName,
                    uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMap::Open", PSO_NULL_HANDLE );
   }

   rc = psoFastMapOpen( m_sessionHandle,
                        hashMapName,
                        nameLengthInBytes,
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

