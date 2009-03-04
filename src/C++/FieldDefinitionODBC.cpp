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
     numFields    ( 0 ),
     currentField ( 0 ),
     simpleDef    ( true )
{
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
#if 0
void FieldDefinitionODBC::AddField( const char   * name,
                                     uint32_t       nameLength,
                                     psoFieldType   type,
                                     uint32_t       length,
                                     uint32_t       precision,
                                     uint32_t       scale )
{
   if ( fields == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField", PSO_NULL_POINTER );
   }

   if ( currentField >= numFields ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_FIELD_NAME );
   }
   memcpy( fields[currentField].name, name, nameLength );
   
   switch ( type ) {
   case PSO_INTEGER:
   case PSO_TINYINT:
   case PSO_SMALLINT:
   case PSO_BIGINT:
   case PSO_REAL:
   case PSO_DOUBLE:
   case PSO_DATE:
   case PSO_TIME:
   case PSO_TIMESTAMP:
      fields[currentField].type = type;
      fields[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_BINARY:
   case PSO_CHAR:
      if ( length == 0 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      fields[currentField].type = type;
      fields[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_VARBINARY:
   case PSO_VARCHAR:
      if ( currentField != numFields-1 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      fields[currentField].type = type;
      fields[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_LONGVARBINARY:
   case PSO_LONGVARCHAR:
      if ( currentField != numFields-1 ) {
         throw pso::Exception( "FieldDefinitionODBC::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      fields[currentField].type = type;
      fields[currentField].vals.length = 0;
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
      fields[currentField].type = type;
      fields[currentField].vals.decimal.precision = precision;
      fields[currentField].vals.decimal.scale = scale;
      currentField++;
      break;

   default:
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_FIELD_TYPE );
   }
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FieldDefinitionODBC::AddField( const char * name,
                                    uint32_t     nameLength,
                                    psoFieldType type,
                                    uint32_t     length,
                                    uint32_t     precision,
                                    uint32_t     scale )
{
   if ( field == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField", PSO_NULL_POINTER );
   }

   if ( currentField >= numFields ) {
      throw pso::Exception( "FieldDefinitionODBC::AddField",
                            PSO_INVALID_NUM_FIELDS );
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
      if ( currentField != numFields-1 ) {
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
      if ( currentField != numFields-1 ) {
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

