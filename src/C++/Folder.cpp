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
#include <photon/psoFolder>
#include <photon/psoFolder.h>
#include <photon/psoSession>
#include <photon/psoErrors.h>
#include <photon/psoException>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Folder::Folder( Session &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Folder::~Folder()
{
   if ( m_objectHandle != NULL ) {
      psoFolderClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::Close()
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::Close", PSO_NULL_HANDLE );
   }

   rc = psoFolderClose( m_objectHandle );
   m_objectHandle = NULL;
   
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::Close" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::CreateObject( const std::string   & objectName,
                           psoObjectDefinition & definition,
                           psoKeyDefinition    * pKey,
                           psoFieldDefinition  * fields )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObject( m_objectHandle,
                               objectName.c_str(),
                               objectName.length(),
                               &definition,
                               pKey,
                               fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::CreateObject( const char          * objectName,
                           uint32_t              nameLengthInBytes,
                           psoObjectDefinition & definition,
                           psoKeyDefinition    * pKey,
                           psoFieldDefinition  * fields )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObject( m_objectHandle,
                               objectName,
                               nameLengthInBytes,
                               &definition,
                               pKey,
                               fields );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::CreateObjectXML( const std::string & xmlBuffer )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateObjectXML", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObjectXML( m_objectHandle,
                                  xmlBuffer.c_str(),
                                  xmlBuffer.length() );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::CreateObjectXML( const char * xmlBuffer,
                              uint32_t     lengthInBytes )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateObjectXML", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObjectXML( m_objectHandle,
                                  xmlBuffer,
                                  lengthInBytes );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::DestroyObject( const std::string & objectName )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::DestroyObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderDestroyObject( m_objectHandle,
                                objectName.c_str(),
                                objectName.length() );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::DestroyObject( const char * objectName,
                            uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::DestroyObject", PSO_NULL_HANDLE );
   }
   rc = psoFolderDestroyObject( m_objectHandle,
                                objectName,
                                nameLengthInBytes );

   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/*
 * Iterate through the folder - no data items are removed from the 
 * folder by this function.*/
int Folder::GetFirst( psoFolderEntry & entry )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::GetFirst", PSO_NULL_HANDLE );
   }

   rc = psoFolderGetFirst( m_objectHandle, &entry );
   if ( rc != 0 && rc != PSO_IS_EMPTY ) {
      throw pso::Exception( m_sessionHandle, "Folder::GetFirst" );
   }
   
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int Folder::GetNext( psoFolderEntry & entry )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::GetNext", PSO_NULL_HANDLE );
   }

   rc = psoFolderGetNext( m_objectHandle, &entry );
   if ( rc != 0 && rc != PSO_REACHED_THE_END ) {
      throw pso::Exception( m_sessionHandle, "Folder::GetNext" );
   }

   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::Open( const std::string & folderName )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::Open", PSO_NULL_HANDLE );
   }
   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "Folder::Open", PSO_ALREADY_OPEN );
   }

   rc = psoFolderOpen( m_sessionHandle,
                       folderName.c_str(),
                       folderName.length(),
                       &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::Open( const char * folderName,
                   uint32_t     nameLengthInBytes )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::Open", PSO_NULL_HANDLE );
   }
   if ( m_objectHandle != NULL ) {
      throw pso::Exception( "Folder::Open", PSO_ALREADY_OPEN );
   }

   rc = psoFolderOpen( m_sessionHandle,
                       folderName,
                       nameLengthInBytes,
                       &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::Open" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::Status( psoObjStatus & status )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::Status", PSO_NULL_HANDLE );
   }

   rc = psoFolderStatus( m_objectHandle, &status );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

