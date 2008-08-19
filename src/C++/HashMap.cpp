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
#include <photon/psoHashMap>
#include <photon/psoHashMap.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoHashMap::psoHashMap( psoSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoHashMap::~psoHashMap()
{
   if ( m_objectHandle != NULL ) {
      psoHashMapClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Close()
{
   int rc = psoHashMapClose( m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Definition( psoObjectDefinition ** definition )
{
   int rc = psoHashMapDefinition( m_objectHandle, definition );
   
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Delete( const void * key,
                         size_t       keyLength )
{
   int rc = psoHashMapDelete( m_objectHandle,
                              key,
                              keyLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Delete" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Get( const void * key,
                      size_t       keyLength,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     * returnedLength )
{
   int rc = psoHashMapGet( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           returnedLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoHashMap::GetFirst( void       * key,
                          size_t       keyLength,
                          void       * buffer,
                          size_t       bufferLength,
                          size_t     * retKeyLength,
                          size_t     * retDataLength )
{
   int rc = psoHashMapGetFirst( m_objectHandle,
                                key,
                                keyLength,
                                buffer,
                                bufferLength,
                                retKeyLength,
                                retDataLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoHashMap::GetNext( void       * key,
                         size_t       keyLength,
                         void       * buffer,
                         size_t       bufferLength,
                         size_t     * retKeyLength,
                         size_t     * retDataLength )
{
   int rc = psoHashMapGetNext( m_objectHandle,
                               key,
                               keyLength,
                               buffer,
                               bufferLength,
                               retKeyLength,
                               retDataLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Insert( const void * key,
                         size_t       keyLength,
                         const void * data,
                         size_t       dataLength )
{
   int rc = psoHashMapInsert( m_objectHandle,
                              key,
                              keyLength,
                              data,
                              dataLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Open( const std::string & hashMapName )
{
   int rc = psoHashMapOpen( m_sessionHandle,
                            hashMapName.c_str(),
                            hashMapName.length(),
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Open( const char * hashMapName,
                       size_t       nameLengthInBytes )
{
   int rc = psoHashMapOpen( m_sessionHandle,
                            hashMapName,
                            nameLengthInBytes,
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Replace( const void * key,
                          size_t       keyLength,
                          const void * data,
                          size_t       dataLength )
{
   int rc = psoHashMapReplace( m_objectHandle,
                               key,
                               keyLength,
                               data,
                               dataLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoHashMap::Status( psoObjStatus * pStatus )
{
   int rc = psoHashMapStatus( m_objectHandle, pStatus );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoHashMap::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

