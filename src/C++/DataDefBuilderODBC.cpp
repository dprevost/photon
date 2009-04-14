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
#include <photon/DataDefBuilderODBC>
#include <sstream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefBuilderODBC::DataDefBuilderODBC( uint32_t numFieldFields,
                                        bool     simple /* = true */ )
   : fields       ( NULL ),
     numFields    ( numFieldFields ),
     currentField ( 0 ),
     simpleDef    ( simple )
{
   if ( numFieldFields == 0 || numFieldFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "DataDefBuilderODBC::DataDefBuilderODBC",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   // using calloc - being lazy...
   size_t len = numFieldFields * sizeof(psoFieldDefinition);
   fields = (psoFieldDefinition *)calloc( len, 1 );
   if ( fields == NULL ) {
      throw pso::Exception( "DataDefBuilderODBC::DataDefBuilderODBC",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefBuilderODBC::~DataDefBuilderODBC()
{
   if ( fields ) free( fields );
   fields = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void DataDefBuilderODBC::AddField( std::string  & name,
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

void DataDefBuilderODBC::AddField( const char * name,
                                    uint32_t     nameLength,
                                    psoFieldType type,
                                    uint32_t     length,
                                    uint32_t     precision,
                                    uint32_t     scale )
{
   if ( fields == NULL ) {
      throw pso::Exception( "DataDefBuilderODBC::AddField", PSO_NULL_POINTER );
   }

   if ( currentField >= numFields ) {
      throw pso::Exception( "DataDefBuilderODBC::AddField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( name == NULL ) {
      throw pso::Exception( "DataDefBuilderODBC::AddField",
                            PSO_NULL_POINTER );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "DataDefBuilderODBC::AddField",
                            PSO_INVALID_FIELD_NAME );
   }
   memcpy( fields[currentField].name, name, nameLength );
   
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
      fields[currentField].type = type;
      fields[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_BINARY:
   case PSO_CHAR:
      if ( length == 0 ) {
         throw pso::Exception( "DataDefBuilderODBC::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      fields[currentField].type = type;
      fields[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_VARBINARY:
   case PSO_VARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         throw pso::Exception( "DataDefBuilderODBC::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "DataDefBuilderODBC::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      fields[currentField].type = type;
      fields[currentField].vals.length = length;
      currentField++;
      break;

   case PSO_LONGVARBINARY:
   case PSO_LONGVARCHAR:
      if ( simpleDef && currentField != numFields-1 ) {
         throw pso::Exception( "DataDefBuilderODBC::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      fields[currentField].type = type;
      fields[currentField].vals.length = 0;
      currentField++;
      break;

   case PSO_NUMERIC:
      if ( precision == 0 || precision > PSO_FIELD_MAX_PRECISION ) {
         throw pso::Exception( "DataDefBuilderODBC::AddField",
                               PSO_INVALID_PRECISION );
      }
      if ( scale > precision ) {
         throw pso::Exception( "DataDefBuilderODBC::AddField",
                               PSO_INVALID_SCALE );
      }
      fields[currentField].type = type;
      fields[currentField].vals.decimal.precision = precision;
      fields[currentField].vals.decimal.scale = scale;
      currentField++;
      break;

   default:
      throw pso::Exception( "DataDefBuilderODBC::AddField",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * DataDefBuilderODBC::GetDefinition()
{
   if ( fields == NULL ) {
      throw pso::Exception( "DataDefBuilderODBC::GetDefinition", PSO_NULL_POINTER );
   }

   if ( currentField != numFields ) {
      throw pso::Exception( "DataDefBuilderODBC::GetDefinition",
                            PSO_INVALID_NUM_FIELDS );
   }

   return (const unsigned char *)fields;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoUint32 DataDefBuilderODBC::GetDefLength()
{
   if ( fields == NULL ) {
      throw pso::Exception( "DataDefBuilderODBC::GetDefLength", PSO_NULL_POINTER );
   }

   if ( currentField != numFields ) {
      throw pso::Exception( "DataDefBuilderODBC::GetDefLength",
                            PSO_INVALID_NUM_FIELDS );
   }

   return sizeof(psoFieldDefinition)*numFields;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

