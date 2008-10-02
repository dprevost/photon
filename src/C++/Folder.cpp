/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include <photon/psoFolder>
#include <photon/psoFolder.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoFolder::psoFolder( psoSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoFolder::~psoFolder()
{
   if ( m_objectHandle != NULL ) {
      psoFolderClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Close()
{
   int rc = psoFolderClose( m_objectHandle );
   m_objectHandle = NULL;
   
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::Close" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObject( const std::string   & objectName,
                              psoObjectDefinition * pDefinition )
{
   int rc = psoFolderCreateObject( m_objectHandle,
                                   objectName.c_str(),
                                   objectName.length(),
                                   pDefinition );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObject( const char    *       objectName,
                              size_t                nameLengthInBytes,
                              psoObjectDefinition * pDefinition )
{
   int rc = psoFolderCreateObject( m_objectHandle,
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObjectXML( const std::string & xmlBuffer )
{
   int rc = psoFolderCreateObjectXML( m_objectHandle,
                                      xmlBuffer.c_str(),
                                      xmlBuffer.length() );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObjectXML( const char * xmlBuffer,
                                 size_t       lengthInBytes )
{
   int rc = psoFolderCreateObjectXML( m_objectHandle,
                                      xmlBuffer,
                                      lengthInBytes );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::DestroyObject( const std::string & objectName )
{
   int rc = psoFolderDestroyObject( m_objectHandle,
                                    objectName.c_str(),
                                    objectName.length() );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::DestroyObject( const char * objectName,
                               size_t       nameLengthInBytes )
{
   int rc = psoFolderDestroyObject( m_objectHandle,
                                    objectName,
                                    nameLengthInBytes );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/*
 * Iterate through the folder - no data items are removed from the 
 * folder by this function.*/
int psoFolder::GetFirst( psoFolderEntry * pEntry )
{
   int rc = psoFolderGetFirst( m_objectHandle, pEntry );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::GetFirst" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoFolder::GetNext( psoFolderEntry * pEntry )
{
   int rc = psoFolderGetNext( m_objectHandle, pEntry );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Open( const std::string & folderName )
{
   int rc = psoFolderOpen( m_sessionHandle,
                           folderName.c_str(),
                           folderName.length(),
                           &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Open( const char * folderName,
                      size_t       nameLengthInBytes )
{
   int rc = psoFolderOpen( m_sessionHandle,
                           folderName,
                           nameLengthInBytes,
                           &m_objectHandle );

   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::Open" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Status( psoObjStatus * pStatus )
{
   int rc = psoFolderStatus( m_objectHandle, pStatus );
   if ( rc != 0 ) {
      throw psoException( rc, m_sessionHandle, "psoFolder::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

