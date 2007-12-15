/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <vdsf/vdsQueue>
#include <vdsf/vdsQueue.h>
#include <vdsf/vdsSession>
#include <vdsf/vdsErrors.h>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsQueue::vdsQueue( vdsSession &session )
   : m_objectHandle ( NULL )
{
   if ( session.m_sessionHandle == NULL )
      throw( VDS_NULL_HANDLE );
   m_sessionHandle = session.m_sessionHandle;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsQueue::~vdsQueue()
{
   if ( m_objectHandle != NULL )
      vdsQueueClose( m_objectHandle );
   m_sessionHandle = m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsQueue::Close()
{
   int rc = vdsQueueClose( m_objectHandle );

   if ( rc != 0 ) throw( rc );
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsQueue::GetFirst( void   * buffer,
                        size_t   bufferLength,
                        size_t * returnedLength )
{
   int rc = vdsQueueGetFirst( m_objectHandle,
                              buffer,
                              bufferLength,
                              returnedLength );
   if ( rc != 0 && rc != VDS_IS_EMPTY ) throw( rc );
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsQueue::GetNext( void   * buffer,
                       size_t   bufferLength,
                       size_t * returnedLength )
{
   int rc = vdsQueueGetNext( m_objectHandle,
                             buffer,
                             bufferLength,
                             returnedLength );
   if ( rc != 0 && rc != VDS_REACHED_THE_END ) throw( rc );
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsQueue::Open( const std::string & queueName )
{
   int rc = vdsQueueOpen( m_sessionHandle,
                          queueName.c_str(),
                          queueName.length(),
                          &m_objectHandle );
   if ( rc != 0 ) throw( rc );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsQueue::Open( const char * queueName,
                     size_t       nameLengthInBytes )
{
   int rc = vdsQueueOpen( m_sessionHandle,
                          queueName,
                          nameLengthInBytes,
                          &m_objectHandle );
   if ( rc != 0 ) throw( rc );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsQueue::Pop( void   * buffer,
                   size_t   bufferLength,
                   size_t * returnedLength )
{
   int rc = vdsQueuePop( m_objectHandle,
                         buffer,
                         bufferLength,
                         returnedLength );
   if ( rc != 0 && rc != VDS_IS_EMPTY ) throw( rc );
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsQueue::Push( const void * pItem, 
                     size_t       length )
{
   int rc = vdsQueuePush( m_objectHandle, 
                          pItem, 
                          length );
   if ( rc != 0 ) throw( rc );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsQueue::Status( vdsObjStatus * pStatus )
{
   int rc = vdsQueueStatus( m_objectHandle,
                            pStatus );
   if ( rc != 0 ) throw( rc );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
