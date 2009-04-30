/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/KeyDefinition.h>
#include "API/KeyDefinition.h"
#include <sstream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition::KeyDefinition()
   : m_definitionHandle ( NULL ),
     m_sessionHandle    ( NULL ),
     m_defType          ( PSO_DEF_USER_DEFINED ),
     m_keyDef           ( NULL ),
     m_keyDefLength     ( 0 ),
     m_currentLength    ( 0 )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition::KeyDefinition( Session                & session,
                              const std::string        name,
                              enum psoDefinitionType   type,
                              const unsigned char    * keyDef,
                              psoUint32                keyDefLength )
   : m_definitionHandle ( NULL ),
     m_sessionHandle    ( session.m_sessionHandle ),
     m_defType          ( type ),
     m_keyDef           ( keyDef ),
     m_keyDefLength     ( keyDefLength ),
     m_currentLength    ( 0 )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "keyDefinition::KeyDefinition", PSO_NULL_HANDLE );
   }

   rc = psoKeyDefCreate( m_sessionHandle,
                         name.c_str(),
                         name.length(),
                         type,
                         keyDef,
                         keyDefLength,
                         &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::KeyDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition::KeyDefinition( Session & session, const std::string name )
   : m_definitionHandle ( NULL ),
     m_sessionHandle    ( session.m_sessionHandle ),
     m_currentLength    ( 0 )
{
   int rc;
   char * dummyName;
   uint32_t length;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::KeyDefinition", PSO_NULL_HANDLE );
   }

   rc = psoKeyDefOpen( m_sessionHandle,
                       name.c_str(),
                       name.length(),
                       &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::KeyDefinition" );
   }
   
   rc = psoaKeyDefGetDef( m_definitionHandle, 
                          &dummyName,
                          &length,
                          &m_defType,
                          (unsigned char **)&m_keyDef,
                          &m_keyDefLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::KeyDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition::KeyDefinition( PSO_HANDLE sessionHandle,
                              PSO_HANDLE definitionHandle )
   : m_definitionHandle ( definitionHandle ),
     m_sessionHandle    ( sessionHandle ),
     m_currentLength    ( 0 )
{
   int rc;
   char * dummyName;
   uint32_t length;

   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::KeyDefinition", PSO_NULL_HANDLE );
   }

   rc = psoaKeyDefGetDef( m_definitionHandle, 
                          &dummyName,
                          &length,
                          &m_defType,
                          (unsigned char **)&m_keyDef,
                          &m_keyDefLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::KeyDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition::~KeyDefinition()
{
   if ( m_definitionHandle != NULL ) {
      psoKeyDefClose( m_definitionHandle );
   }
   m_sessionHandle = m_definitionHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinition::Close()
{
   int rc;

   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::Close", PSO_NULL_HANDLE );
   }

   rc = psoKeyDefClose( m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::Close" );
   }
   m_sessionHandle = m_definitionHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinition::Create( Session                & session,
                            const std::string        name,
                            enum psoDefinitionType   type,
                            const unsigned char    * keyDef,
                            psoUint32                keyDefLength )
{
   int rc;
   
   if ( session.m_sessionHandle == NULL ) {
      throw pso::Exception( "keyDefinition::Create", PSO_NULL_HANDLE );
   }

   if ( m_definitionHandle != NULL ) {
      throw pso::Exception( "KeyDefinition::Create", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;
   m_defType = type;
   m_keyDef  = keyDef;
   m_keyDefLength  = keyDefLength;
   m_currentLength = 0;

   rc = psoKeyDefCreate( m_sessionHandle,
                         name.c_str(),
                         name.length(),
                         type,
                         keyDef,
                         keyDefLength,
                         &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::Create" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinition::GetDefinition( unsigned char * buffer,
                                   psoUint32       bufferLength)
{
   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::GetDefinition", PSO_NULL_HANDLE );
   }

   if ( bufferLength < m_keyDefLength ) {
      throw pso::Exception( "KeyDefinition::GetDefinition", PSO_INVALID_LENGTH );
   }
   
   memcpy( buffer, m_keyDef, m_keyDefLength );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoUint32 KeyDefinition::GetLength()
{
   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::GetLength", PSO_NULL_HANDLE );
   }

   return m_keyDefLength;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string KeyDefinition::GetNext()
{
   string s;
   uint32_t i;

   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::GetNext", PSO_NULL_HANDLE );
   }

   if ( m_keyDef == NULL ) {
      throw pso::Exception( "KeyDefinition::GetNext", PSO_NULL_POINTER );
   }

   if ( m_currentLength >= m_keyDefLength ) {
      m_currentLength = 0;
      return s;
   }
   
   if ( m_defType == PSO_DEF_USER_DEFINED ) {
      
      for ( i = m_currentLength; i < m_keyDefLength; ++i ) {
         if ( m_keyDef[i] == 0 ) {
            m_currentLength = i + 1;
            break;
         }
         s += m_keyDef[i];
      }
   }
   else if ( m_defType == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
      
//      return GetNextODBC();
   }

   return s;
}


// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

enum psoDefinitionType KeyDefinition::GetType()
{
   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::GetType", PSO_NULL_HANDLE );
   }

   return m_defType;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinition::Open( Session & session, const std::string name )
{
   int rc;
   char * dummyName;
   uint32_t length;
   
   if ( session.m_sessionHandle == NULL ) {
      throw pso::Exception( "KeyDefinition::Open", PSO_NULL_HANDLE );
   }

   if ( m_definitionHandle != NULL ) {
      throw pso::Exception( "KeyDefinition::Open", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;
   
   rc = psoKeyDefOpen( m_sessionHandle,
                       name.c_str(),
                       name.length(),
                       &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::Open" );
   }
   
   rc = psoaKeyDefGetDef( m_definitionHandle, 
                          &dummyName,
                          &length,
                          &m_defType,
                          (unsigned char **)&m_keyDef,
                          &m_keyDefLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "KeyDefinition::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

