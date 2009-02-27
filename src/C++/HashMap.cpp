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
#include <photon/psoHashMap>
#include <photon/psoHashMap.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>
#include <photon/psoDefinition>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

HashMap::HashMap( Session &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

HashMap::~HashMap()
{
   if ( m_objectHandle != NULL ) {
      psoHashMapClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Close", PSO_NULL_HANDLE );
   }

   rc = psoHashMapClose( m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
void HashMap::Definition( ObjDefinition & definition )
{
   int rc;
   psoObjectDefinition def;
   psoKeyDefinition key;
   psoFieldDefinition * fields;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Definition", PSO_NULL_HANDLE );
   }
   
   memset( &def, 0, sizeof(psoObjectDefinition) );
   memset( &key, 0, sizeof(psoKeyDefinition) );
   rc = psoHashMapDefinition( m_objectHandle, &def, &key, 0, NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Definition" );
   }
   fields = (psoFieldDefinition *) 
      calloc(sizeof(psoFieldDefinition) * def.numFields, 1);
   if ( fields == NULL ) {
      throw pso::Exception( "HashMap::Definition", PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   rc = psoHashMapDefinition( m_objectHandle, &def, &key, def.numFields, fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Definition" );
   }
   
   // We catch and rethrow the exception to avoid a memory leak
   try {
      definition.Reset( def, &key, fields );
   }
   catch( pso::Exception exc ) {
      free( fields );
      throw exc;
   }
   
   free( fields );
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Definition( psoObjectDefinition & definition,
                          unsigned char       * key,
                          psoUint32             keyLength,
                          unsigned char       * fields,
                          psoUint32             fieldsLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Definition", PSO_NULL_HANDLE );
   }

   rc = psoHashMapDefinition( m_objectHandle, 
                              &definition,
                              key,
                              keyLength,
                              fields,
                              fieldsLength );   
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Delete( const void * key,
                      uint32_t     keyLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Delete", PSO_NULL_HANDLE );
   }

   rc = psoHashMapDelete( m_objectHandle,
                          key,
                          keyLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Delete" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Get( const void * key,
                   uint32_t     keyLength,
                   void       * buffer,
                   uint32_t     bufferLength,
                   uint32_t   & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Get", PSO_NULL_HANDLE );
   }

   rc = psoHashMapGet( m_objectHandle,
                       key,
                       keyLength,
                       buffer,
                       bufferLength,
                       &returnedLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int HashMap::GetFirst( void     * key,
                       uint32_t   keyLength,
                       void     * buffer,
                       uint32_t   bufferLength,
                       uint32_t & retKeyLength,
                       uint32_t & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::GetFirst", PSO_NULL_HANDLE );
   }

   rc = psoHashMapGetFirst( m_objectHandle,
                            key,
                            keyLength,
                            buffer,
                            bufferLength,
                            &retKeyLength,
                            &retDataLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw pso::Exception( m_sessionHandle, "HashMap::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int HashMap::GetNext( void     * key,
                      uint32_t   keyLength,
                      void     * buffer,
                      uint32_t   bufferLength,
                      uint32_t & retKeyLength,
                      uint32_t & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoHashMapGetNext( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           &retKeyLength,
                           &retDataLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw pso::Exception( m_sessionHandle, "HashMap::GetNext" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Insert( const void * key,
                      uint32_t     keyLength,
                      const void * data,
                      uint32_t     dataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Insert", PSO_NULL_HANDLE );
   }

   rc = psoHashMapInsert( m_objectHandle,
                          key,
                          keyLength,
                          data,
                          dataLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Open( const std::string & hashMapName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Open", PSO_NULL_HANDLE );
   }
   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "HashMap::Open", PSO_ALREADY_OPEN );
   }

   rc = psoHashMapOpen( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Open( const char * hashMapName,
                    uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Open", PSO_NULL_HANDLE );
   }
   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "HashMap::Open", PSO_ALREADY_OPEN );
   }

   rc = psoHashMapOpen( m_sessionHandle,
                        hashMapName,
                        nameLengthInBytes,
                        &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Replace( const void * key,
                       uint32_t     keyLength,
                       const void * data,
                       uint32_t     dataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Replace", PSO_NULL_HANDLE );
   }

   rc = psoHashMapReplace( m_objectHandle,
                           key,
                           keyLength,
                           data,
                           dataLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void HashMap::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "HashMap::Status", PSO_NULL_HANDLE );
   }

   rc = psoHashMapStatus( m_objectHandle, &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "HashMap::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

