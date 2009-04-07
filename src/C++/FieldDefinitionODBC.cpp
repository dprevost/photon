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
#include <sstream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionODBC::FieldDefinitionODBC( unsigned char * serialFieldDef,
                                          uint32_t        fieldDefLen )
   : FieldDefinition( true ),
     field        ( NULL ),
     currentField ( 0 ),
     simpleDef    ( true )
{
   if ( serialFieldDef == NULL ) {
      throw pso::Exception( "FieldDefinitionODBC::FieldDefinitionODBC", 
                            PSO_NULL_POINTER );
   }
   if ( fieldDefLen == 0 ) {
      throw pso::Exception( "FieldDefinitionODBC::FieldDefinitionODBC", 
                            PSO_INVALID_LENGTH );
   }
   if ( (fieldDefLen%sizeof(psoFieldDefinition)) != 0 ) {
      throw pso::Exception( "FieldDefinitionODBC::FieldDefinitionODBC", 
                            PSO_INVALID_LENGTH );
   }

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
   : FieldDefinition( false ),
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

