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
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::Close", PSO_NULL_HANDLE );
   }

   rc = psoFolderClose( m_objectHandle );
   m_objectHandle = NULL;
   
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::Close" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObject( const std::string   & objectName,
                              psoObjectDefinition & definition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObject( m_objectHandle,
                               objectName.c_str(),
                               objectName.length(),
                               &definition );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObject( const char          * objectName,
                              size_t                nameLengthInBytes,
                              psoObjectDefinition & definition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObject( m_objectHandle,
                               objectName,
                               nameLengthInBytes,
                               &definition );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObjectXML( const std::string & xmlBuffer )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::CreateObjectXML", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObjectXML( m_objectHandle,
                                  xmlBuffer.c_str(),
                                  xmlBuffer.length() );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::CreateObjectXML( const char * xmlBuffer,
                                 size_t       lengthInBytes )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::CreateObjectXML", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObjectXML( m_objectHandle,
                                  xmlBuffer,
                                  lengthInBytes );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::DestroyObject( const std::string & objectName )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::DestroyObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderDestroyObject( m_objectHandle,
                                objectName.c_str(),
                                objectName.length() );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::DestroyObject( const char * objectName,
                               size_t       nameLengthInBytes )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::DestroyObject", PSO_NULL_HANDLE );
   }
   rc = psoFolderDestroyObject( m_objectHandle,
                                objectName,
                                nameLengthInBytes );

   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/*
 * Iterate through the folder - no data items are removed from the 
 * folder by this function.*/
int psoFolder::GetFirst( psoFolderEntry & entry )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::GetFirst", PSO_NULL_HANDLE );
   }

   rc = psoFolderGetFirst( m_objectHandle, &entry );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw psoException( m_sessionHandle, "psoFolder::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int psoFolder::GetNext( psoFolderEntry & entry )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoFolderGetNext( m_objectHandle, &entry );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw psoException( m_sessionHandle, "psoFolder::GetNext" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Open( const std::string & folderName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::Open", PSO_NULL_HANDLE );
   }

   rc = psoFolderOpen( m_sessionHandle,
                       folderName.c_str(),
                       folderName.length(),
                       &m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Open( const char * folderName,
                      size_t       nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::Open", PSO_NULL_HANDLE );
   }

   rc = psoFolderOpen( m_sessionHandle,
                       folderName,
                       nameLengthInBytes,
                       &m_objectHandle );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::Open" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoFolder::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw psoException( "psoFolder::Status", PSO_NULL_HANDLE );
   }

   rc = psoFolderStatus( m_objectHandle, &status );
   if ( rc != 0 ) {
      throw psoException( m_sessionHandle, "psoFolder::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

