/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <vdsf/vdsFastMap>
#include <vdsf/vdsFastMap.h>
#include <vdsf/vdsSession>
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsFastMap::vdsFastMap( vdsSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsFastMap::~vdsFastMap()
{
   if ( m_objectHandle != NULL ) {
      vdsFastMapClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMap::Close()
{
   int rc = vdsFastMapClose( m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMap::Definition( vdsObjectDefinition ** definition )
{
   int rc = vdsFastMapDefinition( m_objectHandle, definition );
   
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMap::Get( const void * key,
                      size_t       keyLength,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     * returnedLength )
{
   int rc = vdsFastMapGet( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           returnedLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsFastMap::GetFirst( void       * key,
                          size_t       keyLength,
                          void       * buffer,
                          size_t       bufferLength,
                          size_t     * retKeyLength,
                          size_t     * retDataLength )
{
   int rc = vdsFastMapGetFirst( m_objectHandle,
                                key,
                                keyLength,
                                buffer,
                                bufferLength,
                                retKeyLength,
                                retDataLength );
   if ( rc != 0 && rc != VDS_IS_EMPTY ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsFastMap::GetNext( void       * key,
                         size_t       keyLength,
                         void       * buffer,
                         size_t       bufferLength,
                         size_t     * retKeyLength,
                         size_t     * retDataLength )
{
   int rc = vdsFastMapGetNext( m_objectHandle,
                               key,
                               keyLength,
                               buffer,
                               bufferLength,
                               retKeyLength,
                               retDataLength );
   if ( rc != 0 && rc != VDS_REACHED_THE_END ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMap::Open( const std::string & hashMapName )
{
   int rc = vdsFastMapOpen( m_sessionHandle,
                            hashMapName.c_str(),
                            hashMapName.length(),
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMap::Open( const char * hashMapName,
                       size_t       nameLengthInBytes )
{
   int rc = vdsFastMapOpen( m_sessionHandle,
                            hashMapName,
                            nameLengthInBytes,
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFastMap::Status( vdsObjStatus * pStatus )
{
   int rc = vdsFastMapStatus( m_objectHandle, pStatus );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFastMap::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

