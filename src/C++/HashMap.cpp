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
#include <vdsf/vdsHashMap>
#include <vdsf/vdsHashMap.h>
#include <vdsf/vdsSession>
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsHashMap::vdsHashMap( vdsSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsHashMap::~vdsHashMap()
{
   if ( m_objectHandle != NULL ) {
      vdsHashMapClose( m_objectHandle );
   }
   m_sessionHandle = m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Close()
{
   int rc = vdsHashMapClose( m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Delete( const void * key,
                         size_t       keyLength )
{
   int rc = vdsHashMapDelete( m_objectHandle,
                              key,
                              keyLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Delete" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Get( const void * key,
                      size_t       keyLength,
                      void       * buffer,
                      size_t       bufferLength,
                      size_t     * returnedLength )
{
   int rc = vdsHashMapGet( m_objectHandle,
                           key,
                           keyLength,
                           buffer,
                           bufferLength,
                           returnedLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Get" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsHashMap::GetFirst( void       * key,
                          size_t       keyLength,
                          void       * buffer,
                          size_t       bufferLength,
                          size_t     * retKeyLength,
                          size_t     * retDataLength )
{
   int rc = vdsHashMapGetFirst( m_objectHandle,
                                key,
                                keyLength,
                                buffer,
                                bufferLength,
                                retKeyLength,
                                retDataLength );
   if ( rc != 0 && rc != VDS_IS_EMPTY ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsHashMap::GetNext( void       * key,
                         size_t       keyLength,
                         void       * buffer,
                         size_t       bufferLength,
                         size_t     * retKeyLength,
                         size_t     * retDataLength )
{
   int rc = vdsHashMapGetNext( m_objectHandle,
                               key,
                               keyLength,
                               buffer,
                               bufferLength,
                               retKeyLength,
                               retDataLength );
   if ( rc != 0 && rc != VDS_REACHED_THE_END ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Insert( const void * key,
                         size_t       keyLength,
                         const void * data,
                         size_t       dataLength )
{
   int rc = vdsHashMapInsert( m_objectHandle,
                              key,
                              keyLength,
                              data,
                              dataLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Insert" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Open( const std::string & hashMapName )
{
   int rc = vdsHashMapOpen( m_sessionHandle,
                            hashMapName.c_str(),
                            hashMapName.length(),
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Open( const char * hashMapName,
                       size_t       nameLengthInBytes )
{
   int rc = vdsHashMapOpen( m_sessionHandle,
                            hashMapName,
                            nameLengthInBytes,
                            &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Replace( const void * key,
                          size_t       keyLength,
                          const void * data,
                          size_t       dataLength )
{
   int rc = vdsHashMapReplace( m_objectHandle,
                               key,
                               keyLength,
                               data,
                               dataLength );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Replace" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsHashMap::Status( vdsObjStatus * pStatus )
{
   int rc = vdsHashMapStatus( m_objectHandle, pStatus );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsHashMap::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

