/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/psoFastMapEditor>
#include <photon/psoFastMap.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMapEditor::FastMapEditor( Session &session )
   : FastMap( session )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FastMapEditor::~FastMapEditor()
{
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
                          dataLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Open( const std::string & hashMapName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Open", PSO_NULL_HANDLE );
   }

   rc = psoFastMapEdit( m_sessionHandle,
                        hashMapName.c_str(),
                        hashMapName.length(),
                        &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FastMapEditor::Open( const char * hashMapName,
                          uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "FastMapEditor::Open", PSO_NULL_HANDLE );
   }

   rc = psoFastMapEdit( m_sessionHandle,
                        hashMapName,
                        nameLengthInBytes,
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
                           dataLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "FastMapEditor::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

