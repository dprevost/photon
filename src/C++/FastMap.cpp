/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/psoFastMap>
#include <photon/psoFastMap.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoFastMap::psoFastMap( psoSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoFastMap::~psoFastMap()
{
   if ( m_objectHandle != NULL ) {
      psoFastMapClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMap::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::Close", PSO_NULL_HANDLE );
   }
   
   rc = psoFastMapClose( m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFastMap::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMap::Definition( psoObjectDefinition ** definition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::Definition", PSO_NULL_HANDLE );
   }

   rc = psoFastMapDefinition( m_objectHandle, definition );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFastMap::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMap::Get( const void * key,
                      size_t       keyLength,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::Get", PSO_NULL_HANDLE );
   }

   rc = psoFastMapGet( m_objectHandle,
                       key,
                       keyLength,
                       buffer,
                       bufferLength,
                       &returnedLength );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFastMap::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoFastMap::GetFirst( void       * key,
                          size_t       keyLength,
                          void       * buffer,
                          size_t       bufferLength,
                          size_t     & retKeyLength,
                          size_t     & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::GetFirst", PSO_NULL_HANDLE );
   }
   
   rc = psoFastMapGetFirst( m_objectHandle,
                            key,
                            keyLength,
                            buffer,
                            bufferLength,
                            &retKeyLength,
                            &retDataLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw psoException( m_sessionHandle, "psoFastMap::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoFastMap::GetNext( void       * key,
                         size_t       keyLength,
                         void       * buffer,
                         size_t       bufferLength,
                         size_t     & retKeyLength,
                         size_t     & retDataLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoFastMapGetNext( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           &retKeyLength,
                           &retDataLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw psoException( m_sessionHandle, "psoFastMap::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMap::Open( const std::string & hashMapName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::Open", PSO_NULL_HANDLE );
   }

   rc = psoFastMapOpen( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFastMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMap::Open( const char * hashMapName,
                       size_t       nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::Open", PSO_NULL_HANDLE );
   }

   rc = psoFastMapOpen( m_sessionHandle,
                        hashMapName,
                        nameLengthInBytes,
                        &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFastMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMap::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFastMap::Status", PSO_NULL_HANDLE );
   }

   rc = psoFastMapStatus( m_objectHandle, &status );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFastMap::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

