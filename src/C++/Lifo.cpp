/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/psoLifo>
#include <photon/psoLifo.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoLifo::psoLifo( psoSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoLifo::~psoLifo()
{
   if ( m_objectHandle != NULL ) {
      psoLifoClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoLifo::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Close", PSO_NULL_HANDLE );
   }

   rc = psoLifoClose( m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoLifo::Close" );
   }
   
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoLifo::Definition( psoObjectDefinition ** definition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Definition", PSO_NULL_HANDLE );
   }

   rc = psoLifoDefinition( m_objectHandle, definition );
   
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoLifo::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoLifo::GetFirst( void   * buffer,
                       size_t   bufferLength,
                       size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::GetFirst", PSO_NULL_HANDLE );
   }

   rc = psoLifoGetFirst( m_objectHandle,
                         buffer,
                         bufferLength,
                         &returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw psoException( m_sessionHandle, "psoLifo::GetFirst" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoLifo::GetNext( void   * buffer,
                      size_t   bufferLength,
                      size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoLifoGetNext( m_objectHandle,
                        buffer,
                        bufferLength,
                        &returnedLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw psoException( m_sessionHandle, "psoLifo::GetNext" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoLifo::Open( const std::string & queueName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Open", PSO_NULL_HANDLE );
   }

   rc = psoLifoOpen( m_sessionHandle,
                     queueName.c_str(),
                     queueName.length(),
                     &m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoLifo::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoLifo::Open( const char * queueName,
                    size_t       nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Open", PSO_NULL_HANDLE );
   }

   rc = psoLifoOpen( m_sessionHandle,
                     queueName,
                     nameLengthInBytes,
                     &m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoLifo::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoLifo::Pop( void   * buffer,
                  size_t   bufferLength,
                  size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Pop", PSO_NULL_HANDLE );
   }

   rc = psoLifoPop( m_objectHandle,
                    buffer,
                    bufferLength,
                    &returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY && rc != PSO_ITEM_IS_IN_USE ) {
      throw psoException( m_sessionHandle, "psoLifo::Pop" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoLifo::Push( const void * pItem, 
                    size_t       length )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Push", PSO_NULL_HANDLE );
   }

   rc = psoLifoPush( m_objectHandle, 
                     pItem, 
                     length );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoLifo::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoLifo::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoLifo::Status", PSO_NULL_HANDLE );
   }

   rc = psoLifoStatus( m_objectHandle,
                       &status );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoLifo::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

