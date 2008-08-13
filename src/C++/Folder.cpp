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
#include <vdsf/vdsFolder>
#include <vdsf/vdsFolder.h>
#include <vdsf/vdsSession>
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsException>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsFolder::vdsFolder( vdsSession &session )
   : m_objectHandle  ( NULL ),
     m_sessionHandle ( session.m_sessionHandle )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsFolder::~vdsFolder()
{
   if ( m_objectHandle != NULL ) {
      vdsFolderClose( m_objectHandle );
   }
   m_objectHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::Close()
{
   int rc = vdsFolderClose( m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::Close" );
   }
   m_objectHandle = NULL;   
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::CreateObject( const std::string   & objectName,
                              vdsObjectDefinition * pDefinition )
{
   int rc = vdsFolderCreateObject( m_objectHandle,
                                   objectName.c_str(),
                                   objectName.length(),
                                   pDefinition );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::CreateObject( const char    *       objectName,
                              size_t                nameLengthInBytes,
                              vdsObjectDefinition * pDefinition )
{
   int rc = vdsFolderCreateObject( m_objectHandle,
                                   objectName,
                                   nameLengthInBytes,
                                   pDefinition );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::CreateObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::CreateObjectXML( const std::string & xmlBuffer )
{
   int rc = vdsFolderCreateObjectXML( m_objectHandle,
                                      xmlBuffer.c_str(),
                                      xmlBuffer.length() );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::CreateObjectXML( const char * xmlBuffer,
                                 size_t       lengthInBytes )
{
   int rc = vdsFolderCreateObjectXML( m_objectHandle,
                                      xmlBuffer,
                                      lengthInBytes );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::CreateObjectXML" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::DestroyObject( const std::string & objectName )
{
   int rc = vdsFolderDestroyObject( m_objectHandle,
                                    objectName.c_str(),
                                    objectName.length() );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::DestroyObject( const char * objectName,
                               size_t       nameLengthInBytes )
{
   int rc = vdsFolderDestroyObject( m_objectHandle,
                                    objectName,
                                    nameLengthInBytes );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::DestroyObject" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/*
 * Iterate through the folder - no data items are removed from the 
 * folder by this function.*/
int vdsFolder::GetFirst( vdsFolderEntry * pEntry )
{
   int rc = vdsFolderGetFirst( m_objectHandle, pEntry );
   if ( rc != 0 && rc != VDS_IS_EMPTY ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::GetFirst" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdsFolder::GetNext( vdsFolderEntry * pEntry )
{
   int rc = vdsFolderGetNext( m_objectHandle, pEntry );
   if ( rc != 0 && rc != VDS_REACHED_THE_END ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::GetNext" );
   }
   return rc;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::Open( const std::string & folderName )
{
   int rc = vdsFolderOpen( m_sessionHandle,
                           folderName.c_str(),
                           folderName.length(),
                           &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::Open( const char * folderName,
                      size_t       nameLengthInBytes )
{
   int rc = vdsFolderOpen( m_sessionHandle,
                           folderName,
                           nameLengthInBytes,
                           &m_objectHandle );

   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::Open" );
   }
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdsFolder::Status( vdsObjStatus * pStatus )
{
   int rc = vdsFolderStatus( m_objectHandle, pStatus );
   if ( rc != 0 ) {
      throw vdsException( rc, m_sessionHandle, "vdsFolder::Status" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

