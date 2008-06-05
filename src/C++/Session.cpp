/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <vdsf/vdsSession>
#include <vdsf/vdsSession.h>
#include <vdsf/vdsErrors.h>
#include "API/Session.h"
#include <vdsf/vdsException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsSession::vdsSession()
   : m_sessionHandle ( NULL )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsSession::~vdsSession()
{
   vdsExitSession( m_sessionHandle );
   m_sessionHandle = NULL;
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::Commit()
{
   int rc = vdsCommit( m_sessionHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::Commit" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::CreateObject( const std::string   & objectName,
                               vdsObjectDefinition * pDefinition )
{
   int rc = vdsCreateObject( m_sessionHandle,
                             objectName.c_str(),
                             objectName.length(),
                             pDefinition );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::CreateObject" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::CreateObject( const char          * objectName,
                               size_t                nameLengthInBytes,
                               vdsObjectDefinition * pDefinition )
{
   int rc = vdsCreateObject( m_sessionHandle,
                             objectName,
                             nameLengthInBytes,
                             pDefinition );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::CreateObject" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::DestroyObject( const std::string & objectName )
{
   int rc = vdsDestroyObject( m_sessionHandle,
                              objectName.c_str(),
                              objectName.length() );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::DestroyObject( const char * objectName,
                                size_t       nameLengthInBytes )
{
   int rc = vdsDestroyObject( m_sessionHandle,
                              objectName,
                              nameLengthInBytes );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

std::string & 
vdsSession::ErrorMsg( std::string & message )
{
   char msg[1024];
   int rc = vdsErrorMsg( m_sessionHandle,
                         msg,
                         1024 );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::ErrorMsg" );
   }
   message = msg;
   return message;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::ErrorMsg( char   * message,
                           size_t   msgLengthInBytes )
{
   // Should be rewritten to use stl strings!!!
   int rc = vdsErrorMsg( m_sessionHandle,
                         message,
                         msgLengthInBytes );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::ErrorMsg" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::GetInfo( vdsInfo * pInfo )
{
   int rc = vdsGetInfo( m_sessionHandle, pInfo );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::GetInfo" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::GetStatus( const std::string & objectName,
                            vdsObjStatus      * pStatus )
{
   int rc = vdsGetStatus( m_sessionHandle,
                          objectName.c_str(),
                          objectName.length(),
                          pStatus );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::GetStatus" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::GetStatus( const char   * objectName,
                            size_t         nameLengthInBytes,
                            vdsObjStatus * pStatus )
{
   int rc = vdsGetStatus( m_sessionHandle,
                          objectName,
                          nameLengthInBytes,
                          pStatus );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::GetStatus" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::Init()
{
   int rc = vdsInitSession( &m_sessionHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, NULL, "vdsSession::Init" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsSession::LastError()
{
   vdsaSession* pSession;
   int rc = 0;
   int lastErr = 0;
   
   pSession = (vdsaSession*) m_sessionHandle;
   if ( pSession == NULL ) return VDS_NULL_HANDLE;
   
   if ( pSession->type != VDSA_SESSION ) return VDS_WRONG_TYPE_HANDLE;

   if ( vdsaSessionLock( pSession ) == 0 ) {
      if ( ! pSession->terminated ) {
         lastErr = vdscGetLastError( &pSession->context.errorHandler );
      }
      else {
         rc = VDS_SESSION_IS_TERMINATED;
      }
      vdsaSessionUnlock( pSession );
   }
   else {
      rc = VDS_SESSION_CANNOT_GET_LOCK;
   }
   
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::ErrorMsg" );
   }
   
   return lastErr;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsSession::Rollback()
{
   int rc = vdsRollback( m_sessionHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsSession::Rollback" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

