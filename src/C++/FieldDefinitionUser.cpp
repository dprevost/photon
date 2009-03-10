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
#include <photon/FieldDefinitionUser>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionUser::FieldDefinitionUser( unsigned char * serialFieldDef,
                                          uint32_t        fieldDefLen )
   : FieldDefinition( true ),
     field        ( NULL ),
     numFields    ( 0 ),
     currentField ( 0 )
{
   if ( serialFieldDef == NULL ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser", 
                            PSO_NULL_POINTER );
   }
   if ( fieldDefLen == 0 ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser", 
                            PSO_INVALID_LENGTH );
   }

   field = (unsigned char *)malloc( fieldDefLen );
   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   memcpy( field, serialFieldDef, fieldDefLen );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionUser::FieldDefinitionUser( uint32_t numFieldFields )
   : FieldDefinition( false ),
     field        ( NULL ),
     numFields    ( numFieldFields ),
     currentField ( 0 )
{
   if ( numFieldFields == 0 || numFieldFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser",
                            PSO_INVALID_NUM_FIELDS );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionUser::~FieldDefinitionUser()
{
   if ( field ) free( field );
   field = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FieldDefinitionUser::AddField( std::string fieldDescriptor )
{
   if ( readOnly ) {
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_INVALID_DEF_OPERATION );
   }

   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionUser::AddField", PSO_NULL_POINTER );
   }

   if ( currentField >= numFields ) {
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( name == NULL ) {
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_NULL_POINTER );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_INVALID_FIELD_NAME );
   }
   memcpy( field[currentField].name, name, nameLength );
   
   switch ( type ) {
   case PSO_TINYINT:
   case PSO_SMALLINT:
   case PSO_INTEGER:
   case PSO_BIGINT:
   case PSO_REAL:
   case PSO_DOUBLE:
   case PSO_DATE:
   case PSO_TIME:
   case PSO_TIMESTAMP:
      field[currentField].type = type;
      field[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_BINARY:
   case PSO_CHAR:
      if ( length == 0 ) {
         throw pso::Exception( "FieldDefinitionUser::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      field[currentField].type = type;
      field[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_VARBINARY:
   case PSO_VARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         throw pso::Exception( "FieldDefinitionUser::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "FieldDefinitionUser::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      field[currentField].type = type;
      field[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_LONGVARBINARY:
   case PSO_LONGVARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         throw pso::Exception( "FieldDefinitionUser::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      field[currentField].type = type;
      field[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_NUMERIC:
      if ( precision == 0 || precision > PSO_FIELD_MAX_PRECISION ) {
         throw pso::Exception( "FieldDefinitionUser::AddField",
                               PSO_INVALID_PRECISION );
      }
      if ( scale > precision ) {
         throw pso::Exception( "FieldDefinitionUser::AddField",
                               PSO_INVALID_SCALE );
      }
      field[currentField].type = type;
      field[currentField].vals.decimal.precision = precision;
      field[currentField].vals.decimal.scale = scale;
      currentField++;
      break;

   default:
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string FieldDefinitionUser::GetNext() 
{
   string s;
   char name [PSO_MAX_FIELD_LENGTH+1];
   
   if ( ! readOnly ) {
      throw pso::Exception( "FieldDefinitionUser::GetNext",
                            PSO_INVALID_DEF_OPERATION );
   }
   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionUser::GetNext", PSO_NULL_POINTER );
   }

   if ( currentField >= numFields ) return s;

   s += "Name: ";   
   if ( field[currentField].name[PSO_MAX_FIELD_LENGTH-1] == '\0' ) {
      s += field[currentField].name;
   }
   else {
      memcpy( name, field[currentField].name, PSO_MAX_FIELD_LENGTH );
      name[PSO_MAX_FIELD_LENGTH] = '\0';
      s += name;
   }
      
   s += ", Type: ";
   switch ( field[currentField].type ) {

   case PSO_TINYINT:
      s += "TinyInt";
      break;
   case PSO_SMALLINT:
      s += "SmallInt";
      break;
   case PSO_INTEGER:
      s += "Integer";
      break;
   case PSO_BIGINT:
      s += "BigInt";
      break;
   case PSO_REAL:
      s += "Real";
      break;
   case PSO_DOUBLE:
      s += "Double";
      break;
   case PSO_DATE:
      s += "Date";
      break;
   case PSO_TIME:
      s += "Time";
      break;
   case PSO_TIMESTAMP:
      s += "TimeStamp";
      break;

   case PSO_BINARY:
      s += "Binary, Length: ";
      s += field[currentField].vals.length;
      break;
   case PSO_CHAR:
      s += "Char, Length: ";
      s += field[currentField].vals.length;
      break;

   case PSO_VARBINARY:
      s += "VarBinary, Length: ";
      s += field[currentField].vals.length;
      break;

   case PSO_VARCHAR:
      s += "VarChar, Length: ";
      s += field[currentField].vals.length;
      break;

   case PSO_LONGVARBINARY:
      s += "LongVarBinary";
      break;
   case PSO_LONGVARCHAR:
      s += "LongVarChar";
      break;

   case PSO_NUMERIC:
      s += "Numeric, Precision = ";
      s += field[currentField].vals.decimal.precision;
      s += ", Scale = ";
      s += field[currentField].vals.decimal.scale;
      break;

   default:
      throw pso::Exception( "FieldDefinitionUser::GetNext",
                            PSO_INVALID_FIELD_TYPE );
   }

   currentField++;

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * FieldDefinitionUser::GetDefinition()
{
   return (unsigned char *)field;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* Returns the length, in bytes, of the definition. */
uint32_t FieldDefinitionUser::GetDefLength()
{
   return numFields * sizeof(psoFieldDefinition);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

