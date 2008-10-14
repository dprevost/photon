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
#include <photon/psoSession>
#include <photon/psoSession.h>
#include <photon/psoErrors.h>
#include "API/Session.h"
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoSession::psoSession()
   : m_sessionHandle ( NULL )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoSession::~psoSession()
{
   psoExitSession( m_sessionHandle );
   m_sessionHandle = NULL;
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::Commit()
{
   int rc = psoCommit( m_sessionHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::Commit" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::CreateObject( const std::string         & objectName,
                               const psoObjectDefinition & definition )
{
   int rc = psoCreateObject( m_sessionHandle,
                             objectName.c_str(),
                             objectName.length(),
                             (psoObjectDefinition*)&definition );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::CreateObject" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::CreateObject( const char                * objectName,
                               size_t                      nameLengthInBytes,
                               const psoObjectDefinition & definition )
{
   int rc = psoCreateObject( m_sessionHandle,
                             objectName,
                             nameLengthInBytes,
                             (psoObjectDefinition*)&definition );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::CreateObject" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::DestroyObject( const std::string & objectName )
{
   int rc = psoDestroyObject( m_sessionHandle,
                              objectName.c_str(),
                              objectName.length() );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::DestroyObject( const char * objectName,
                                size_t       nameLengthInBytes )
{
   int rc = psoDestroyObject( m_sessionHandle,
                              objectName,
                              nameLengthInBytes );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

std::string & 
psoSession::ErrorMsg( std::string & message )
{
   char msg[1024];
   int rc = psoErrorMsg( m_sessionHandle,
                         msg,
                         1024 );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::ErrorMsg" );
   }
   message = msg;
   return message;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::ErrorMsg( char   * message,
                           size_t   msgLengthInBytes )
{
   // Should be rewritten to use stl strings!!!
   int rc = psoErrorMsg( m_sessionHandle,
                         message,
                         msgLengthInBytes );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::ErrorMsg" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::GetDefinition( const std::string    & objectName,
                                psoObjectDefinition ** ppDefinition )
{
   int rc = psoGetDefinition( m_sessionHandle,
                              objectName.c_str(),
                              objectName.length(),
                              ppDefinition );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::GetDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::GetDefinition( const char           * objectName,
                                size_t                 nameLengthInBytes,
                                psoObjectDefinition ** ppDefinition )
{
   int rc = psoGetDefinition( m_sessionHandle,
                              objectName,
                              nameLengthInBytes,
                              ppDefinition );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::GetDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::GetInfo( psoInfo & info )
{
   int rc = psoGetInfo( m_sessionHandle, &info );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::GetInfo" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::GetStatus( const std::string & objectName,
                            psoObjStatus      & status )
{
   int rc = psoGetStatus( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length(),
                          &status );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::GetStatus" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::GetStatus( const char   * objectName,
                            size_t         nameLengthInBytes,
                            psoObjStatus & status )
{
   int rc = psoGetStatus( m_sessionHandle,
                          objectName,
                          nameLengthInBytes,
                          &status );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::GetStatus" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::Init()
{
   int rc = psoInitSession( &m_sessionHandle );

   if ( rc != 0 ) {
      throw psoException( rc, NULL, "psoSession::Init" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoSession::LastError()
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
      throw psoException( rc, m_sessionHandle, "psoSession::ErrorMsg" );
   }
   
   return lastErr;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoSession::Rollback()
{
   int rc = psoRollback( m_sessionHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoSession::Rollback" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

