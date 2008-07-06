/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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
#include <vdsf/vdsFastMapEditor>
#include <vdsf/vdsFastMap.h>
#include <vdsf/vdsSession>
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsFastMapEditor::vdsFastMapEditor( vdsSession &session )
   : vdsFastMap( session )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsFastMapEditor::~vdsFastMapEditor()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMapEditor::Delete( const void * key,
                         size_t       keyLength )
{
   int rc = vdsFastMapDelete( m_objectHandle,
                              key,
                              keyLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMapEditor::Delete" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMapEditor::Insert( const void * key,
                               size_t       keyLength,
                               const void * data,
                               size_t       dataLength )
{
   int rc = vdsFastMapInsert( m_objectHandle,
                              key,
                              keyLength,
                              data,
                              dataLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMapEditor::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMapEditor::Open( const std::string & hashMapName )
{
   int rc = vdsFastMapEdit( m_sessionHandle,
                            hashMapName.c_str(),
                            hashMapName.length(),
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMapEditor::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMapEditor::Open( const char * hashMapName,
                             size_t       nameLengthInBytes )
{
   int rc = vdsFastMapEdit( m_sessionHandle,
                            hashMapName,
                            nameLengthInBytes,
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMapEditor::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMapEditor::Replace( const void * key,
                                size_t       keyLength,
                                const void * data,
                                size_t       dataLength )
{
   int rc = vdsFastMapReplace( m_objectHandle,
                               key,
                               keyLength,
                               data,
                               dataLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMapEditor::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

