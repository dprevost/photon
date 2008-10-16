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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/psoQueue>
#include <photon/psoQueue.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Queue::Queue( Session & session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Queue::~Queue()
{
   if ( m_objectHandle != NULL ) {
      psoQueueClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Close", PSO_NULL_HANDLE );
   }

   rc = psoQueueClose( m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Close" );
   }
   
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Definition( psoObjectDefinition ** definition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Definition", PSO_NULL_HANDLE );
   }

   rc = psoQueueDefinition( m_objectHandle, definition );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Definition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Queue::GetFirst( void   * buffer,
                     size_t   bufferLength,
                     size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::GetFirst", PSO_NULL_HANDLE );
   }

   rc = psoQueueGetFirst( m_objectHandle,
                          buffer,
                          bufferLength,
                          &returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw pso::Exception( m_sessionHandle, "Queue::GetFirst" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Queue::GetNext( void   * buffer,
                    size_t   bufferLength,
                    size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoQueueGetNext( m_objectHandle,
                         buffer,
                         bufferLength,
                         &returnedLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw pso::Exception( m_sessionHandle, "Queue::GetNext" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Open( const std::string & queueName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Open", PSO_NULL_HANDLE );
   }

   rc = psoQueueOpen( m_sessionHandle,
                      queueName.c_str(),
                      queueName.length(),
                      &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Open( const char * queueName,
                  size_t       nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Open", PSO_NULL_HANDLE );
   }

   rc = psoQueueOpen( m_sessionHandle,
                      queueName,
                      nameLengthInBytes,
                      &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Queue::Pop( void   * buffer,
                size_t   bufferLength,
                size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Pop", PSO_NULL_HANDLE );
   }

   rc = psoQueuePop( m_objectHandle,
                     buffer,
                     bufferLength,
                     &returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY && rc != PSO_ITEM_IS_IN_USE ) {
      throw pso::Exception( m_sessionHandle, "Queue::Pop" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Push( const void * pItem, 
                  size_t       length )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Push", PSO_NULL_HANDLE );
   }

   rc = psoQueuePush( m_objectHandle, 
                      pItem, 
                      length );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::PushNow( const void * pItem, 
                     size_t       length )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::PushNow", PSO_NULL_HANDLE );
   }

   rc = psoQueuePushNow( m_objectHandle, 
                         pItem, 
                         length );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::PushNow" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Status", PSO_NULL_HANDLE );
   }

   rc = psoQueueStatus( m_objectHandle,
                            &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

