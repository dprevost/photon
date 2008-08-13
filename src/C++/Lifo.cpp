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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <vdsf/vdsLifo>
#include <vdsf/vdsLifo.h>
#include <vdsf/vdsSession>
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsLifo::vdsLifo( vdsSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsLifo::~vdsLifo()
{
   if ( m_objectHandle != NULL ) {
      vdsLifoClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsLifo::Close()
{
   int rc = vdsLifoClose( m_objectHandle );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Close" );
   }
   
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsLifo::Definition( vdsObjectDefinition ** definition )
{
   int rc = vdsLifoDefinition( m_objectHandle, definition );
   
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsLifo::GetFirst( void   * buffer,
                       size_t   bufferLength,
                       size_t * returnedLength )
{
   int rc = vdsLifoGetFirst( m_objectHandle,
                             buffer,
                             bufferLength,
                             returnedLength );
   if ( rc != 0 && rc != VDS_IS_EMPTY ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::GetFirst" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsLifo::GetNext( void   * buffer,
                      size_t   bufferLength,
                      size_t * returnedLength )
{
   int rc = vdsLifoGetNext( m_objectHandle,
                            buffer,
                            bufferLength,
                            returnedLength );
   if ( rc != 0 && rc != VDS_REACHED_THE_END ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsLifo::Open( const std::string & queueName )
{
   int rc = vdsLifoOpen( m_sessionHandle,
                         queueName.c_str(),
                         queueName.length(),
                         &m_objectHandle );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsLifo::Open( const char * queueName,
                    size_t       nameLengthInBytes )
{
   int rc = vdsLifoOpen( m_sessionHandle,
                         queueName,
                         nameLengthInBytes,
                         &m_objectHandle );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsLifo::Pop( void   * buffer,
                  size_t   bufferLength,
                  size_t * returnedLength )
{
   int rc = vdsLifoPop( m_objectHandle,
                        buffer,
                        bufferLength,
                        returnedLength );
   if ( rc != 0 && rc != VDS_IS_EMPTY && rc != VDS_ITEM_IS_IN_USE ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Pop" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsLifo::Push( const void * pItem, 
                    size_t       length )
{
   int rc = vdsLifoPush( m_objectHandle, 
                         pItem, 
                         length );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsLifo::Status( vdsObjStatus * pStatus )
{
   int rc = vdsLifoStatus( m_objectHandle,
                           pStatus );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsLifo::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

