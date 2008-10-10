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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include <photon/psoFastMapEditor>
#include <photon/psoFastMap.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoFastMapEditor::psoFastMapEditor( psoSession &session )
   : psoFastMap( session )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoFastMapEditor::~psoFastMapEditor()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMapEditor::Delete( const void * key,
                         size_t       keyLength )
{
   int rc = psoFastMapDelete( m_objectHandle,
                              key,
                              keyLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFastMapEditor::Delete" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMapEditor::Empty()
{
   int rc = psoFastMapEmpty( m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFastMapEditor::Empty" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMapEditor::Insert( const void * key,
                               size_t       keyLength,
                               const void * data,
                               size_t       dataLength )
{
   int rc = psoFastMapInsert( m_objectHandle,
                              key,
                              keyLength,
                              data,
                              dataLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFastMapEditor::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMapEditor::Open( const std::string & hashMapName )
{
   int rc = psoFastMapEdit( m_sessionHandle,
                            hashMapName.c_str(),
                            hashMapName.length(),
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFastMapEditor::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMapEditor::Open( const char * hashMapName,
                             size_t       nameLengthInBytes )
{
   int rc = psoFastMapEdit( m_sessionHandle,
                            hashMapName,
                            nameLengthInBytes,
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFastMapEditor::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFastMapEditor::Replace( const void * key,
                                size_t       keyLength,
                                const void * data,
                                size_t       dataLength )
{
   int rc = psoFastMapReplace( m_objectHandle,
                               key,
                               keyLength,
                               data,
                               dataLength );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFastMapEditor::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

