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
#include <photon/psoSession>
#include <photon/psoSession.h>
#include <photon/psoErrors.h>
#include "API/Session.h"
#include <photon/psoException>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Session::Session()
   : m_sessionHandle ( NULL )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Session::~Session()
{
   psoExitSession( m_sessionHandle );
   m_sessionHandle = NULL;
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::Commit()
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::Commit", PSO_NULL_HANDLE );
   }

   rc = psoCommit( m_sessionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::Commit" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateObject( const std::string   & objectName,
                            ObjDefinition       & definition )
{
   int rc;

      if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoCreateObject( m_sessionHandle,
                         objectName.c_str(),
                         objectName.length(),
                         (psoObjectDefinition *)&(definition.GetDef()),
                         (psoKeyDefinition *)   &(definition.GetKey()),
                         (psoFieldDefinition *) definition.GetFields() );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateObject( const std::string         & objectName,
                            const psoObjectDefinition & definition,
                            const psoKeyDefinition    * pKey,
                            psoFieldDefinition        * fields )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoCreateObject( m_sessionHandle,
                         objectName.c_str(),
                         objectName.length(),
                         (psoObjectDefinition *) &definition,
                         (psoKeyDefinition *)    pKey,
                         fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateObject" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateObject( const char                * objectName,
                            uint32_t                    nameLengthInBytes,
                            const psoObjectDefinition & definition,
                            const psoKeyDefinition    * pKey,
                            psoFieldDefinition        * fields )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoCreateObject( m_sessionHandle,
                         objectName,
                         nameLengthInBytes,
                         (psoObjectDefinition *) &definition,
                         (psoKeyDefinition *)    pKey,
                         fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateObject" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::DestroyObject( const std::string & objectName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::DestroyObject", PSO_NULL_HANDLE );
   }

   rc = psoDestroyObject( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length() );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::DestroyObject( const char * objectName,
                             uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::DestroyObject", PSO_NULL_HANDLE );
   }

   rc = psoDestroyObject( m_sessionHandle,
                          objectName,
                          nameLengthInBytes );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

std::string & 
Session::ErrorMsg( std::string & message )
{
   char msg[1024];
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::ErrorMsg", PSO_NULL_HANDLE );
   }

   rc = psoErrorMsg( m_sessionHandle,
                     msg,
                     1024 );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::ErrorMsg" );
   }
   message = msg;

   return message;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::ErrorMsg( char   * message,
                        uint32_t msgLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::ErrorMsg", PSO_NULL_HANDLE );
   }

   rc = psoErrorMsg( m_sessionHandle,
                     message,
                     msgLengthInBytes );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::ErrorMsg" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::GetDefinition( const std::string & objectName,
                             ObjDefinition     & definition )
{
   int rc;
   psoObjectDefinition def;
   psoFieldDefinition * fields;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetDefinition", PSO_NULL_HANDLE );
   }
   
   memset( &def, 0, sizeof(psoObjectDefinition) );
   rc = psoGetDefinition( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length(),
                          &def,
                          0,
                          NULL );
   //psoHashMapDefinition( m_objectHandle, &def, 0, NULL );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetDefinition" );
   }
   fields = (psoFieldDefinition *) 
      calloc(sizeof(psoFieldDefinition) * def.numFields, 1);
   if ( fields == NULL ) {
      throw pso::Exception( "Session::GetDefinition", PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   rc = psoGetDefinition( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length(),
                          &def,
                          def.numFields, 
                          fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetDefinition" );
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

void Session::GetDefinition( const std::string   & objectName,
                             psoObjectDefinition & definition,
                             psoUint32             numFields,
                             psoFieldDefinition  * fields )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetDefinition", PSO_NULL_HANDLE );
   }

   rc = psoGetDefinition( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length(),
                          &definition,
                          numFields,
                          fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::GetDefinition( const char          * objectName,
                             uint32_t              nameLengthInBytes,
                             psoObjectDefinition & definition,
                             psoUint32             numFields,
                             psoFieldDefinition  * fields )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetDefinition", PSO_NULL_HANDLE );
   }

   rc = psoGetDefinition( m_sessionHandle,
                          objectName,
                          nameLengthInBytes,
                          &definition,
                          numFields,
                          fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::GetInfo( psoInfo & info )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetInfo", PSO_NULL_HANDLE );
   }

   rc = psoGetInfo( m_sessionHandle, &info );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetInfo" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::GetStatus( const std::string & objectName,
                         psoObjStatus      & status )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetStatus", PSO_NULL_HANDLE );
   }

   rc = psoGetStatus( m_sessionHandle,
                      objectName.c_str(),
                      objectName.length(),
                      &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetStatus" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::GetStatus( const char   * objectName,
                         uint32_t       nameLengthInBytes,
                         psoObjStatus & status )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetStatus", PSO_NULL_HANDLE );
   }

   rc = psoGetStatus( m_sessionHandle,
                      objectName,
                      nameLengthInBytes,
                      &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetStatus" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::Init()
{
   int rc = psoInitSession( &m_sessionHandle );

   if ( rc != 0 ) {
      throw pso::Exception( "Session::Init", rc );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Session::LastError()
{
   psoaSession* pSession;
   int rc = 0;
   int lastErr = 0;
   
   pSession = (psoaSession *) m_sessionHandle;
   if ( pSession == NULL ) return PSO_NULL_HANDLE;
   
   if ( pSession->type != PSOA_SESSION ) return PSO_WRONG_TYPE_HANDLE;

   if ( psoaSessionLock( pSession ) ) {
      if ( ! pSession->terminated ) {
         lastErr = psocGetLastError( &pSession->context.errorHandler );
      }
      else {
         rc = PSO_SESSION_IS_TERMINATED;
      }
      psoaSessionUnlock( pSession );
   }
   else {
      rc = PSO_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::ErrorMsg" );
   }
   
   return lastErr;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::Rollback()
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::Rollback", PSO_NULL_HANDLE );
   }

   rc = psoRollback( m_sessionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::Rollback" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

