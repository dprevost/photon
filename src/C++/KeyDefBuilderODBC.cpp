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
#include <photon/KeyDefBuilderODBC>
#include <sstream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefBuilderODBC::KeyDefBuilderODBC( uint32_t numKeyFields,
                                      bool     simple /* = true */ )
   : keys         ( NULL ),
     numKeys      ( numKeyFields ),
     currentKey   ( 0 ),
     simpleDef    ( simple )
{
   if ( numKeyFields == 0 || numKeyFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefBuilderODBC::KeyDefBuilderODBC",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   // using calloc - being lazy...
   size_t len = numKeyFields * sizeof(psoKeyFieldDefinition);
   keys = (psoKeyFieldDefinition *)calloc( len, 1 );
   if ( keys == NULL ) {
      throw pso::Exception( "KeyDefBuilderODBC::KeyDefBuilderODBC",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefBuilderODBC::~KeyDefBuilderODBC()
{
   if ( keys ) free( keys );
   keys = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefBuilderODBC::AddKeyField( std::string & name,
                                     psoKeyType    type,
                                     uint32_t      length )
{
   AddKeyField( name.c_str(),
                name.length(),
                type,
                length );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefBuilderODBC::AddKeyField( const char * name,
                                     uint32_t     nameLength,
                                     psoKeyType   type,
                                     uint32_t     length )
{
   if ( keys == NULL ) {
      throw pso::Exception( "KeyDefBuilderODBC::AddKeyField", PSO_NULL_POINTER );
   }

   if ( currentKey >= numKeys ) {
      throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( name == NULL ) {
      throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                            PSO_NULL_POINTER );
   }

   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                            PSO_INVALID_FIELD_NAME );
   }
   memcpy( keys[currentKey].name, name, nameLength );
   
   
   switch ( type ) {
   case PSO_KEY_INTEGER:
   case PSO_KEY_BIGINT:
   case PSO_KEY_DATE:
   case PSO_KEY_TIME:
   case PSO_KEY_TIMESTAMP:
      keys[currentKey].type = type;
      keys[currentKey].length = 0;
      currentKey++;
      break;

   case PSO_KEY_BINARY:
   case PSO_KEY_CHAR:
      if ( length == 0 ) {
         throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      keys[currentKey].type = type;
      keys[currentKey].length = length;
      currentKey++;
      break;

   case PSO_KEY_VARBINARY:
   case PSO_KEY_VARCHAR:
      if ( simpleDef && currentKey != numKeys-1 ) {
         throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      keys[currentKey].type = type;
      keys[currentKey].length = length;
      currentKey++;
      break;

   case PSO_KEY_LONGVARBINARY:
   case PSO_KEY_LONGVARCHAR:
      if ( simpleDef && currentKey != numKeys-1 ) {
         throw pso::Exception( "KeyDefBuilderODBC::AddKeyField",
                               PSO_INVALID_FIELD_TYPE );
      }
      keys[currentKey].type = type;
      keys[currentKey].length = 0;
      currentKey++;
      break;

   default:
      throw pso::Exception( "KeyDefBuilderODBC::AddKeyLength",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * KeyDefBuilderODBC::GetDefinition()
{
   if ( keys == NULL ) {
      throw pso::Exception( "KeyDefBuilderODBC::GetDefinition", PSO_NULL_POINTER );
   }

   if ( currentKey != numKeys ) {
      throw pso::Exception( "KeyDefBuilderODBC::GetDefinition",
                            PSO_INVALID_NUM_FIELDS );
   }

   return (unsigned char *)keys;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* Returns the length, in bytes, of the definition. */
uint32_t KeyDefBuilderODBC::GetDefLength()
{
   if ( keys == NULL ) {
      throw pso::Exception( "KeyDefBuilderODBC::GetDefinition", PSO_NULL_POINTER );
   }

   if ( currentKey != numKeys ) {
      throw pso::Exception( "KeyDefBuilderODBC::GetDefinition",
                            PSO_INVALID_NUM_FIELDS );
   }

   return numKeys * sizeof(psoKeyFieldDefinition);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

