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
#include <photon/FieldDefinitionODBC>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionODBC::FieldDefinitionODBC( unsigned char * serialFieldDef,
                                          uint32_t        fieldDefLen )
   : FieldDefinition( serialFieldDef, fieldDefLen ),
     field        ( NULL ),
     currentField ( 0 ),
     simpleDef    ( true )
{
   numFields = fieldDefLen / sizeof(psoFieldDefinition);

   field = (psoFieldDefinition *) malloc( fieldDefLen );
   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::FieldDefinitionODBC",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   memcpy( field, serialFieldDef, fieldDefLen );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionODBC::FieldDefinitionODBC( uint32_t numFieldFields,
                                          bool     simple /* = true */ )
   : FieldDefinition(),
     field        ( NULL ),
     numFields    ( numFieldFields ),
     currentField ( 0 ),
     simpleDef    ( simple )
{
   if ( numFieldFields == 0 || numFieldFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "FieldDefinitionODBC::FieldDefinitionODBC",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   // using calloc - being lazy...
   size_t len = numFieldFields * sizeof(psoFieldDefinition);
   field = (psoFieldDefinition *)calloc( len, 1 );
   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::FieldDefinitionODBC",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionODBC::~FieldDefinitionODBC()
{
   if ( field ) free( field );
   field = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FieldDefinitionODBC::AddField( std::string  & name,
                                    psoFieldType   type,
                                    uint32_t       length,
                                    uint32_t       precision,
                                    uint32_t       scale )
{
   AddField( name.c_str(),
             name.length(),
             type,
             length,
             precision,
             scale );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FieldDefinitionODBC::AddField( const char * name,
                                    uint32_t     nameLength,
                                    psoFieldType type,
                                    uint32_t     length,
                                    uint32_t     precision,
                                    uint32_t     scale )
{
   if ( readOnly ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_DEF_OPERATION );
   }

   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField", PSO_NULL_POINTER );
   }

   if ( currentField >= numFields ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( name == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_NULL_POINTER );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
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
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      field[currentField].type = type;
      field[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_VARBINARY:
   case PSO_VARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      field[currentField].type = type;
      field[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_LONGVARBINARY:
   case PSO_LONGVARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      field[currentField].type = type;
      field[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_NUMERIC:
      if ( precision == 0 || precision > PSO_FIELD_MAX_PRECISION ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_PRECISION );
      }
      if ( scale > precision ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_SCALE );
      }
      field[currentField].type = type;
      field[currentField].vals.decimal.precision = precision;
      field[currentField].vals.decimal.scale = scale;
      currentField++;
      break;

   default:
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string FieldDefinitionODBC::GetNext() 
{
   string s;
   char name [PSO_MAX_FIELD_LENGTH+1];
   
   if ( ! readOnly ) {
      throw pso::Exception( "FieldDefinitionODBC::GetNext",
                            PSO_INVALID_DEF_OPERATION );
   }
   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::GetNext", PSO_NULL_POINTER );
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
      throw pso::Exception( "FieldDefinitionODBC::GetNext",
                            PSO_INVALID_FIELD_TYPE );
   }

   currentField++;

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
void FieldDefinitionODBC::Reset( uint32_t numberOfFields, enum psoObjectType type )
{
   psoFieldDefinition * tmp;
   
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_WRONG_OBJECT_TYPE );
   }
   currentField = numberOfFields;
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   memset( &field, 0, sizeof(psoFieldDefinition) );
   
   // using calloc - being lazy...
   size_t len = numberOfFields * sizeof(psoFieldDefinition);
   tmp = (psoFieldDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   if ( fields != NULL ) free( fields );
   fields = tmp;
   
   numFields = numberOfFields;
   definition.type = type;
   currentField = 0;
   fieldAdded = false;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
void FieldDefinitionODBC::Reset( psoObjectDefinition & inputDef,
                           psoFieldDefinition    * inputField,
                           psoFieldDefinition  * inputFields )
{
   psoFieldDefinition * tmp;
   
   if ( inputDef.numFields == 0 || inputDef.numFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( inputFields == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_NULL_POINTER );
   }
   if ( inputDef.type < PSO_FOLDER || inputDef.type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_WRONG_OBJECT_TYPE );
   }
   
   size_t len = inputDef.numFields * sizeof(psoFieldDefinition);
   tmp = (psoFieldDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::Reset",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   if ( fields != NULL ) free( fields );
   fields = tmp;

   memcpy( &definition, &inputDef, sizeof(psoObjectDefinition) );
   memcpy( fields, inputFields, len );
   
   currentField = inputDef.numFields;
   
   fieldAdded = false;
   if ( inputField != NULL ) {
      memcpy( &field, inputField, sizeof(psoFieldDefinition) );
      fieldAdded = true;
   }
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
const unsigned char * FieldDefinitionODBC::GetField()
{
   return field;
}

#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

