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
#include <photon/photon>
#include <photon/psoFolder.h>
#include <photon/psoErrors.h>

using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

Folder::Folder( Session & session, const std::string & folderName )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::Folder", PSO_NULL_HANDLE );
   }

   rc = psoFolderOpen( m_sessionHandle,
                       folderName.c_str(),
                       folderName.length(),
                       &m_objectHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::Folder" );
   }
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

void Folder::CreateFolder( const std::string & objectName )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateFolder", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateFolder( m_objectHandle,
                               objectName.c_str(),
                               objectName.length() );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateFolder" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::CreateObject( const std::string   & objectName,
                           psoObjectDefinition & definition,
                           DataDefinition      & dataDefinition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateObject( m_objectHandle,
                               objectName.c_str(),
                               objectName.length(),
                               &definition,
                               dataDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void Folder::CreateObject( const std::string   & objectName,
                           psoObjectDefinition & definition,
                           KeyDefinition       & keyDefinition,
                           DataDefinition      & dataDefinition )
{
   int rc;
   
   if ( m_objectHandle == NULL || m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::CreateObject", PSO_NULL_HANDLE );
   }

   rc = psoFolderCreateKeyedObject( m_objectHandle,
                                    objectName.c_str(),
                                    objectName.length(),
                                    &definition,
                                    keyDefinition.m_definitionHandle,
                                    dataDefinition.m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::CreateObject" );
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

void Folder::GetDefinition( const std::string   & objectName,
                             psoObjectDefinition & definition )
{
   int rc;
   PSO_HANDLE dataDefHandle = NULL, keyDefHandle = NULL;

   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::GetDefinition", PSO_NULL_HANDLE );
   }
   
   rc = psoFolderGetDefinition( m_objectHandle,
                                objectName.c_str(),
                                objectName.length(),
                                &definition,
                                &keyDefHandle,
                                &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::GetDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition * 
Folder::GetDataDefinition( const std::string & objectName )
{
   int rc;
   PSO_HANDLE dataDefHandle = NULL, keyDefHandle = NULL;
   psoObjectDefinition definition;
   DataDefinition * pDefinition;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::GetDataDefinition", PSO_NULL_HANDLE );
   }
   
   rc = psoFolderGetDefinition( m_sessionHandle,
                                objectName.c_str(),
                                objectName.length(),
                                &definition,
                                &keyDefHandle,
                                &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::GetDataDefinition" );
   }
   
   pDefinition = new DataDefinition( m_sessionHandle, dataDefHandle );
   
   return pDefinition;
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinition *
Folder::GetKeyDefinition( const std::string & objectName )
{
   int rc;
   PSO_HANDLE dataDefHandle = NULL, keyDefHandle = NULL;
   psoObjectDefinition definition;
   KeyDefinition * pDefinition;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "Folder::GetKeyDefinition", PSO_NULL_HANDLE );
   }
   
   rc = psoFolderGetDefinition( m_objectHandle,
                                objectName.c_str(),
                                objectName.length(),
                                &definition,
                                &keyDefHandle,
                                &dataDefHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "Folder::GetKeyDefinition" );
   }
   
   pDefinition = new KeyDefinition( m_sessionHandle, keyDefHandle );
   
   return pDefinition;
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

