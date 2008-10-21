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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#include <photon/psoLifo>
#include <photon/psoLifo.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>
#include <photon/psoDefinition>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Lifo::Lifo( Session &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Lifo::~Lifo()
{
   if ( m_objectHandle != NULL ) {
      psoLifoClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Lifo::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Close", PSO_NULL_HANDLE );
   }

   rc = psoLifoClose( m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Close" );
   }
   
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Lifo::Definition( ObjDefinition & definition )
{
   int rc;
   psoObjectDefinition * def = NULL;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Definition", PSO_NULL_HANDLE );
   }

   rc = psoLifoDefinition( m_objectHandle, &def );
   
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Definition" );
   }
   
   // We catch and rethrow the exception to avoid a memory leak
   try {
      definition.Reset( *def );
   }
   catch( pso::Exception exc ) {
      free( def );
      throw exc;
   }
   
   free( def );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Lifo::GetFirst( void   * buffer,
                    size_t   bufferLength,
                    size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::GetFirst", PSO_NULL_HANDLE );
   }

   rc = psoLifoGetFirst( m_objectHandle,
                         buffer,
                         bufferLength,
                         &returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw pso::Exception( m_sessionHandle, "Lifo::GetFirst" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Lifo::GetNext( void   * buffer,
                   size_t   bufferLength,
                   size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoLifoGetNext( m_objectHandle,
                        buffer,
                        bufferLength,
                        &returnedLength );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw pso::Exception( m_sessionHandle, "Lifo::GetNext" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Lifo::Open( const std::string & queueName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Open", PSO_NULL_HANDLE );
   }

   rc = psoLifoOpen( m_sessionHandle,
                     queueName.c_str(),
                     queueName.length(),
                     &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Lifo::Open( const char * queueName,
                 size_t       nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Open", PSO_NULL_HANDLE );
   }

   rc = psoLifoOpen( m_sessionHandle,
                     queueName,
                     nameLengthInBytes,
                     &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Lifo::Pop( void   * buffer,
               size_t   bufferLength,
               size_t & returnedLength )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Pop", PSO_NULL_HANDLE );
   }

   rc = psoLifoPop( m_objectHandle,
                    buffer,
                    bufferLength,
                    &returnedLength );
   if ( rc != 0 && rc != PSO_IS_EMPTY && rc != PSO_ITEM_IS_IN_USE ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Pop" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Lifo::Push( const void * pItem, 
                 size_t       length )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Push", PSO_NULL_HANDLE );
   }

   rc = psoLifoPush( m_objectHandle, 
                     pItem, 
                     length );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Push" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Lifo::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Lifo::Status", PSO_NULL_HANDLE );
   }

   rc = psoLifoStatus( m_objectHandle,
                       &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Lifo::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

