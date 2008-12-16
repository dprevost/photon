/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/psoQueue>
#include <photon/psoQueue.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>
#include <photon/psoDefinition>

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

void Queue::Definition( ObjDefinition & definition )
{
   int rc;
   psoBasicObjectDef def;
   psoFieldDefinition * fields;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Definition", PSO_NULL_HANDLE );
   }
   
   memset( &def, 0, sizeof(psoBasicObjectDef) );
   rc = psoQueueDefinition( m_objectHandle, &def, 0, NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Definition" );
   }
   fields = (psoFieldDefinition *) 
      calloc(sizeof(psoFieldDefinition) * def.numFields, 1);
   if ( fields == NULL ) {
      throw pso::Exception( "Queue::Definition", PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   rc = psoQueueDefinition( m_objectHandle, &def, def.numFields, fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Definition" );
   }
   
   // We catch and rethrow the exception to avoid a memory leak
   try {
      definition.Reset( def, fields );
   }
   catch( pso::Exception exc ) {
      free( fields );
      throw exc;
   }
   
   free( fields );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Queue::Definition( psoBasicObjectDef & definition,
                        psoUint32             numFields,
                        psoFieldDefinition  * fields )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Definition", PSO_NULL_HANDLE );
   }

   rc = psoQueueDefinition( m_objectHandle, 
                           &definition,
                           numFields,
                           fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Queue::Definition" );
   }
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

void Queue::Open( const std::string & queueName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Open", PSO_NULL_HANDLE );
   }
   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "Queue::Open", PSO_ALREADY_OPEN );
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
                  uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Queue::Open", PSO_NULL_HANDLE );
   }
   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "Queue::Open", PSO_ALREADY_OPEN );
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
                      length );
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

