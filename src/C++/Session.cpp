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
#include <photon/Session>
#include <photon/Session.h>
#include <photon/psoErrors.h>
#include "API/Session.h"
#include <photon/Exception>
#include <photon/DataDefinition>
#include <photon/KeyDefinition>
#include <photon/DataDefinition.h>
#include <photon/KeyDefinition.h>

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

void Session::CreateFolder( const std::string & objectName )
{
   int rc;

   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateFolder", PSO_NULL_HANDLE );
   }
    
   rc = psoCreateFolder( m_sessionHandle,
                         objectName.c_str(),
                         objectName.length() );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateFolder" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateQueue( const std::string   & objectName,
                           psoObjectDefinition & definition,
                           DataDefinition      & dataDefinition )
{
   int rc;

   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateQueue", PSO_NULL_HANDLE );
   }

   rc = psoCreateQueue( m_sessionHandle,
                        objectName.c_str(),
                        objectName.length(),
                        &definition,
                        dataDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateQueue" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateQueue( const std::string   & objectName,
                           psoObjectDefinition & definition,
                           const std::string   & dataDefName )
{
   int rc;
   PSO_HANDLE dataDefHandle;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateQueue", PSO_NULL_HANDLE );
   }

   rc = psoDataDefOpen( m_sessionHandle,
                        dataDefName.c_str(),
                        dataDefName.length(),
                        &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateQueue" );
   }

   rc = psoCreateQueue( m_sessionHandle,
                        objectName.c_str(),
                        objectName.length(),
                        &definition,
                        dataDefHandle );
   
   psoDataDefClose( dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateQueue" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateMap( const std::string   & objectName,
                         psoObjectDefinition & definition,
                         DataDefinition      & dataDefinition,
                         KeyDefinition       & keyDefinition )
{
   int rc;

   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateMap", PSO_NULL_HANDLE );
   }

   rc = psoCreateMap( m_sessionHandle,
                      objectName.c_str(),
                      objectName.length(),
                      &definition,
                      dataDefinition.m_definitionHandle,
                      keyDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateMap" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Session::CreateMap( const std::string   & objectName,
                         psoObjectDefinition & definition,
                         const std::string   & dataDefName,
                         const std::string   & keyDefName )
{
   int rc;
   PSO_HANDLE keyDefHandle, dataDefHandle;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::CreateMap", PSO_NULL_HANDLE );
   }

   rc = psoKeyDefOpen( m_sessionHandle,
                       keyDefName.c_str(),
                       keyDefName.length(),
                       &keyDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateMap" );
   }

   rc = psoDataDefOpen( m_sessionHandle,
                        dataDefName.c_str(),
                        dataDefName.length(),
                        &dataDefHandle );
   if ( rc != 0 ) {
      psoKeyDefClose( keyDefHandle );
      throw pso::Exception( m_sessionHandle, "Session::CreateMap" );
   }

   rc = psoCreateMap( m_sessionHandle,
                      objectName.c_str(),
                      objectName.length(),
                      &definition,
                      dataDefHandle,
                      keyDefHandle );

   psoKeyDefClose(  keyDefHandle );
   psoDataDefClose( dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::CreateMap" );
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

void Session::GetDefinition( const std::string   & objectName,
                             psoObjectDefinition & definition )
{
   int rc;

   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetDefinition", PSO_NULL_HANDLE );
   }
   
   rc = psoGetDefinition( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length(),
                          &definition );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition * 
Session::GetDataDefinition( const std::string & objectName )
{
   int rc;
   PSO_HANDLE dataDefHandle = NULL;
   DataDefinition * pDefinition;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetDataDefinition", PSO_NULL_HANDLE );
   }
   
   rc = psoGetDataDefinition( m_sessionHandle,
                              objectName.c_str(),
                              objectName.length(),
                              &dataDefHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetDataDefinition" );
   }
   
   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition *
Session::GetKeyDefinition( const std::string & objectName )
{
   int rc;
   PSO_HANDLE keyDefHandle = NULL;
   KeyDefinition * pDefinition;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Session::GetKeyDefinition", PSO_NULL_HANDLE );
   }
   
   rc = psoGetKeyDefinition( m_sessionHandle,
                             objectName.c_str(),
                             objectName.length(),
                             &keyDefHandle );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Session::GetKeyDefinition" );
   }
   
   pDefinition = new KeyDefinition( m_sessionHandle, keyDefHandle );
   
   return pDefinition;
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

   if ( ! pSession->terminated ) {
      lastErr = psocGetLastError( &pSession->context.errorHandler );
   }
   else {
      rc = PSO_SESSION_IS_TERMINATED;
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

