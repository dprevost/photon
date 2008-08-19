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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/psoQueue>
#include <photon/psoQueue.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoQueue::psoQueue( psoSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoQueue::~psoQueue()
{
   if ( m_objectHandle != NULL ) {
      psoQueueClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::Close()
{
   int rc = psoQueueClose( m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Close" );
   }
   
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::Definition( psoObjectDefinition ** definition )
{
   int rc = psoQueueDefinition( m_objectHandle, definition );
   
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoQueue::GetFirst( void   * buffer,
                        size_t   bufferLength,
                        size_t * returnedLength )
{
   int rc = psoQueueGetFirst( m_objectHandle,
                              buffer,
                              bufferLength,
                              returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::GetFirst" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoQueue::GetNext( void   * buffer,
                       size_t   bufferLength,
                       size_t * returnedLength )
{
   int rc = psoQueueGetNext( m_objectHandle,
                             buffer,
                             bufferLength,
                             returnedLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::Open( const std::string & queueName )
{
   int rc = psoQueueOpen( m_sessionHandle,
                          queueName.c_str(),
                          queueName.length(),
                          &m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::Open( const char * queueName,
                     size_t       nameLengthInBytes )
{
   int rc = psoQueueOpen( m_sessionHandle,
                          queueName,
                          nameLengthInBytes,
                          &m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoQueue::Pop( void   * buffer,
                   size_t   bufferLength,
                   size_t * returnedLength )
{
   int rc = psoQueuePop( m_objectHandle,
                         buffer,
                         bufferLength,
                         returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY && rc != PSO_ITEM_IS_IN_USE ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Pop" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::Push( const void * pItem, 
                     size_t       length )
{
   int rc = psoQueuePush( m_objectHandle, 
                          pItem, 
                          length );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::PushNow( const void * pItem, 
                        size_t       length )
{
   int rc = psoQueuePushNow( m_objectHandle, 
                             pItem, 
                             length );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::PushNow" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoQueue::Status( psoObjStatus * pStatus )
{
   int rc = psoQueueStatus( m_objectHandle,
                            pStatus );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoQueue::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

