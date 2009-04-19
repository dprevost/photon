/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/photon>
#include <photon/Queue.h>
#include "API/Queue.h"
#include "API/Session.h"

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Queue::Queue()
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( NULL )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Queue::Queue( Session & session, const std::string & queueName )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Queue", PSO_NULL_HANDLE );
   }

   rc = psoQueueOpen( m_sessionHandle,
                      queueName.c_str(),
                      queueName.length(),
                      &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Queue" );
   }
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

DataDefinition * Queue::GetDataDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle;
   DataDefinition * pDefinition;

   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::GetDataDefinition", PSO_NULL_HANDLE );
   }

   rc = psoQueueDefinition( m_objectHandle,
                            &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::GetDataDefinition" );
   }
   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Queue::GetFirst( void     * buffer,
                     uint32_t   bufferLength,
                     uint32_t & returnedLength )
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

int Queue::GetNext( void     * buffer,
                    uint32_t   bufferLength,
                    uint32_t & returnedLength )
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

DataDefinition * Queue::GetRecordDefinition()
{
   int rc;
   PSO_HANDLE dataDefHandle;
   DataDefinition * pDefinition;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::GetRecordDefinition", PSO_NULL_HANDLE );
   }

   rc = psoQueueRecordDefinition( m_objectHandle,
                                  &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::GetRecordDefinition" );
   }

   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Open( Session & session, const std::string & queueName )
{
   int rc;
   
   if ( session.m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Open", PSO_NULL_HANDLE );
   }

   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "Queue::Open", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;

   rc = psoQueueOpen( m_sessionHandle,
                      queueName.c_str(),
                      queueName.length(),
                      &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Queue::Pop( void     * buffer,
                uint32_t   bufferLength,
                uint32_t & returnedLength )
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
                  uint32_t     length )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Push", PSO_NULL_HANDLE );
   }

   rc = psoQueuePush( m_objectHandle, 
                      pItem, 
                      length,
                      NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Push( const void           * pItem, 
                  uint32_t               length,
                  const DataDefinition & dataDefinition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Push", PSO_NULL_HANDLE );
   }

   rc = psoQueuePush( m_objectHandle, 
                      pItem, 
                      length,
                      dataDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::PushNow( const void * pItem, 
                     uint32_t     length )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::PushNow", PSO_NULL_HANDLE );
   }

   rc = psoQueuePushNow( m_objectHandle, 
                         pItem, 
                         length,
                         NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::PushNow" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::PushNow( const void           * pItem, 
                     uint32_t               length,
                     const DataDefinition & dataDefinition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::PushNow", PSO_NULL_HANDLE );
   }

   rc = psoQueuePushNow( m_objectHandle, 
                         pItem, 
                         length,
                         dataDefinition.m_definitionHandle );
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

