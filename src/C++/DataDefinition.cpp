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
#include <photon/DataDefinition.h>
#include "API/DataDefinition.h"
#include <sstream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition::DataDefinition()
   : m_definitionHandle ( NULL ),
     m_sessionHandle    ( NULL ),
     m_defType          ( PSO_DEF_USER_DEFINED ),
     m_dataDef          ( NULL ),
     m_dataDefLength    ( 0 ),
     m_currentLength    ( 0 )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition::DataDefinition( Session                & session,
                                const std::string        name,
                                enum psoDefinitionType   type,
                                const unsigned char    * dataDef,
                                psoUint32                dataDefLength )
   : m_definitionHandle ( NULL ),
     m_sessionHandle    ( session.m_sessionHandle ),
     m_defType          ( type ),
     m_dataDef          ( dataDef ),
     m_dataDefLength    ( dataDefLength ),
     m_currentLength    ( 0 )
{
   int rc;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::DataDefinition", PSO_NULL_HANDLE );
   }

   rc = psoDataDefCreate( m_sessionHandle,
                          name.c_str(),
                          name.length(),
                          type,
                          dataDef,
                          dataDefLength,
                          &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::DataDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition::DataDefinition( Session & session, const std::string name )
   : m_definitionHandle ( NULL ),
     m_sessionHandle    ( session.m_sessionHandle ),
     m_currentLength    ( 0 )
{
   int rc;
   char * dummyName;
   uint32_t length;
   
   if ( m_sessionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::DataDefinition", PSO_NULL_HANDLE );
   }

   rc = psoDataDefOpen( m_sessionHandle,
                        name.c_str(),
                        name.length(),
                        &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::DataDefinition" );
   }
   
   rc = psoaDataDefGetDef( m_definitionHandle,
                           &dummyName,
                           &length,
                           &m_defType,
                           (unsigned char **)&m_dataDef,
                           &m_dataDefLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::DataDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition::DataDefinition( PSO_HANDLE sessionHandle,
                                PSO_HANDLE definitionHandle )
   : m_definitionHandle ( definitionHandle ),
     m_sessionHandle    ( sessionHandle ),
     m_currentLength    ( 0 )
{
   int rc;
   char * dummyName;
   uint32_t length;

   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::DataDefinition", PSO_NULL_HANDLE );
   }

   rc = psoaDataDefGetDef( m_definitionHandle, 
                           &dummyName,
                           &length,
                           &m_defType,
                           (unsigned char **)&m_dataDef,
                           &m_dataDefLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::DataDefinition" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefinition::~DataDefinition()
{
   if ( m_definitionHandle != NULL ) {
      psoDataDefClose( m_definitionHandle );
   }
   m_sessionHandle = m_definitionHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
   
void DataDefinition::Close()
{
   int rc;

   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::Close", PSO_NULL_HANDLE );
   }

   rc = psoDataDefClose( m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::Close" );
   }
   m_sessionHandle = m_definitionHandle = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void DataDefinition::Create( Session                & session,
                             const std::string        name,
                             enum psoDefinitionType   type,
                             const unsigned char    * dataDef,
                             psoUint32                dataDefLength )
{
   int rc;
   
   if ( session.m_sessionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::Create", PSO_NULL_HANDLE );
   }
   
   if ( m_definitionHandle != NULL ) {
      throw pso::Exception( "DataDefinition::Create", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;
   m_defType = type;
   m_dataDef = dataDef;
   m_dataDefLength = dataDefLength;
   m_currentLength = 0;

   rc = psoDataDefCreate( m_sessionHandle,
                          name.c_str(),
                          name.length(),
                          type,
                          dataDef,
                          dataDefLength,
                          &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::Create" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void DataDefinition::GetDefinition( unsigned char * buffer,
                                    psoUint32       bufferLength )
{
   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::GetDefinition", PSO_NULL_HANDLE );
   }

   if ( buffer == NULL ) {
      throw pso::Exception( "DataDefinition::GetDefinition", PSO_NULL_POINTER );
   }
   
   if ( bufferLength < m_dataDefLength ) {
      throw pso::Exception( "DataDefinition::GetDefinition", PSO_INVALID_LENGTH );
   }
   
   memcpy( buffer, m_dataDef, m_dataDefLength );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

uint32_t DataDefinition::GetLength()
{
   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::GetLength", PSO_NULL_HANDLE );
   }

   return m_dataDefLength;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string DataDefinition::GetNext()
{
   string s;
   uint32_t i;

   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::GetNext", PSO_NULL_HANDLE );
   }

   if ( m_dataDef == NULL ) {
      throw pso::Exception( "DataDefinition::GetNext", PSO_NULL_POINTER );
   }

   if ( m_currentLength >= m_dataDefLength ) {
      m_currentLength = 0;
      return s;
   }
   
   if ( m_defType == PSO_DEF_USER_DEFINED ) {
      
      for ( i = m_currentLength; i < m_dataDefLength; ++i ) {
         if ( m_dataDef[i] == 0 ) {
            m_currentLength = i + 1;
            break;
         }
         s += m_dataDef[i];
      }
   }
   else if ( m_defType == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
      
      return GetNextODBC();
   }

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string DataDefinition::GetNextODBC() 
{
   stringstream stream;
   string s;
   char name [PSO_MAX_FIELD_LENGTH+1];
   uint32_t currentField = m_currentLength / sizeof(psoFieldDefinition);
   psoFieldDefinition * field = (psoFieldDefinition *)m_dataDef;
   
   stream << "Name: ";   
   if ( field[currentField].name[PSO_MAX_FIELD_LENGTH-1] == '\0' ) {
      stream << field[currentField].name;
   }
   else {
      memcpy( name, field[currentField].name, PSO_MAX_FIELD_LENGTH );
      name[PSO_MAX_FIELD_LENGTH] = '\0';
      stream << name;
   }
      
   stream << ", Type: ";
   switch ( field[currentField].type ) {

   case PSO_TINYINT:
      stream << "TinyInt";
      break;
   case PSO_SMALLINT:
      stream << "SmallInt";
      break;
   case PSO_INTEGER:
      stream << "Integer";
      break;
   case PSO_BIGINT:
      stream << "BigInt";
      break;
   case PSO_REAL:
      stream << "Real";
      break;
   case PSO_DOUBLE:
      stream << "Double";
      break;
   case PSO_DATE:
      stream << "Date";
      break;
   case PSO_TIME:
      stream << "Time";
      break;
   case PSO_TIMESTAMP:
      stream << "TimeStamp";
      break;

   case PSO_BINARY:
      stream << "Binary, Length: ";
      stream << field[currentField].vals.length;
      break;
   case PSO_CHAR:
      stream << "Char, Length: ";
      stream << field[currentField].vals.length;
      break;

   case PSO_VARBINARY:
      stream << "VarBinary, Length: ";
      stream << field[currentField].vals.length;
      break;

   case PSO_VARCHAR:
      stream << "VarChar, Length: ";
      stream << field[currentField].vals.length;
      break;

   case PSO_LONGVARBINARY:
      stream << "LongVarBinary";
      break;
   case PSO_LONGVARCHAR:
      stream << "LongVarChar";
      break;

   case PSO_NUMERIC:
      stream << "Numeric, Precision = ";
      stream << field[currentField].vals.decimal.precision;
      stream << ", Scale = ";
      stream << field[currentField].vals.decimal.scale;
      break;

   default:
      throw pso::Exception( "DataDefinition::GetNextODBC",
                            PSO_INVALID_FIELD_TYPE );
   }

   m_currentLength += sizeof(psoFieldDefinition);
   s = stream.str();
   
   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

enum psoDefinitionType DataDefinition::GetType()
{
   if ( m_sessionHandle == NULL || m_definitionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::GetType", PSO_NULL_HANDLE );
   }

   return m_defType;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void DataDefinition::Open( Session & session, const std::string name )
{
   int rc;
   char * dummyName;
   uint32_t length;
   
   if ( session.m_sessionHandle == NULL ) {
      throw pso::Exception( "DataDefinition::Open", PSO_NULL_HANDLE );
   }

   if ( m_definitionHandle != NULL ) {
      throw pso::Exception( "DataDefinition::Open", PSO_ALREADY_OPEN );
   }

   m_sessionHandle = session.m_sessionHandle;

   rc = psoDataDefOpen( m_sessionHandle,
                        name.c_str(),
                        name.length(),
                        &m_definitionHandle );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::Open" );
   }
   
   rc = psoaDataDefGetDef( m_definitionHandle, 
                           &dummyName,
                           &length,
                           &m_defType,
                           (unsigned char **)&m_dataDef,
                           &m_dataDefLength );
   if ( rc != 0 ) {
      throw pso::Exception( m_sessionHandle, "DataDefinition::Open" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

