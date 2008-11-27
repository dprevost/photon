/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include <photon/photon>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

ObjDefinition::ObjDefinition()
   : pDefinition  ( NULL ),
     currentField ( 0 ),
     keyAdded     ( false )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

ObjDefinition::ObjDefinition( uint32_t numberOfFields, enum psoObjectType type )
   : pDefinition  ( NULL ),
     currentField ( 0 ),
     keyAdded     ( false )
{
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "ObjDefinition::ObjDefinition",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "ObjDefinition::ObjectType",
                            PSO_WRONG_OBJECT_TYPE );
   }
   
   // using calloc - being lazy...
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      numberOfFields * sizeof(psoFieldDefinition);
   pDefinition = (psoObjectDefinition *)calloc( len, 1 );
   if ( pDefinition == NULL ) {
      throw pso::Exception( "ObjDefinition::ObjDefinition",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   pDefinition->numFields = numberOfFields;
   pDefinition->type = type;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

ObjDefinition::~ObjDefinition()
{
   if ( pDefinition ) free( pDefinition );
   pDefinition = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void ObjDefinition::AddField( std::string  & name,
                              psoFieldType   type,
                              uint32_t       length,
                              uint32_t       minLength,
                              uint32_t       maxLength,
                              uint32_t       precision,
                              uint32_t       scale )
{
   AddField( name.c_str(),
             name.length(),
             type,
             length,
             minLength,
             maxLength,
             precision,
             scale );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void ObjDefinition::AddField( const char   * name,
                              uint32_t       nameLength,
                              psoFieldType   type,
                              uint32_t       length,
                              uint32_t       minLength,
                              uint32_t       maxLength,
                              uint32_t       precision,
                              uint32_t       scale )
{
   if ( pDefinition == NULL ) {
      throw pso::Exception( "ObjDefinition::AddField", PSO_NULL_POINTER );
   }

   if ( currentField >= pDefinition->numFields ) {
      throw pso::Exception( "ObjDefinition::AddField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "ObjDefinition::AddField",
                            PSO_INVALID_FIELD_NAME );
   }
   memcpy( pDefinition->fields[currentField].name, name, nameLength );
   
   switch ( type ) {
   case PSO_INTEGER:
      if ( length != 1 && length != 2 && length != 4 && length != 8 ) {
      throw pso::Exception( "ObjDefinition::AddField",
                            PSO_INVALID_FIELD_LENGTH_INT );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].length = length;
      currentField++;
      break;

   case PSO_BINARY:
   case PSO_STRING:
      if ( length == 0 ) {
         throw pso::Exception( "ObjDefinition::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].length = length;
      currentField++;
      break;

   case PSO_VAR_BINARY:
   case PSO_VAR_STRING:
      if ( currentField != pDefinition->numFields-1 ) {
         throw pso::Exception( "ObjDefinition::AddField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( maxLength != 0 && maxLength < minLength ) {
         throw pso::Exception( "ObjDefinition::AddField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].minLength = minLength;
      pDefinition->fields[currentField].maxLength = maxLength;
      currentField++;
      break;

   case PSO_BOOLEAN:
      pDefinition->fields[currentField].type = type;
      currentField++;
      break;

   case PSO_DECIMAL:
      if ( precision == 0 || precision > PSO_FIELD_MAX_PRECISION ) {
         throw pso::Exception( "ObjDefinition::AddField",
                               PSO_INVALID_PRECISION );
      }
      if ( scale > precision ) {
         throw pso::Exception( "ObjDefinition::AddField",
                               PSO_INVALID_SCALE );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].precision = precision;
      pDefinition->fields[currentField].scale = scale;
      currentField++;
      break;

   default:
      throw pso::Exception( "ObjDefinition::AddField",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void ObjDefinition::AddKey( psoKeyType type,
                            uint32_t   length,
                            uint32_t   minLength,
                            uint32_t   maxLength )
{
   if ( pDefinition == NULL ) {
      throw pso::Exception( "ObjDefinition::AddKey", PSO_NULL_POINTER );
   }

   if ( keyAdded ) {
      throw pso::Exception( "ObjDefinition::AddKey",
                            PSO_INVALID_KEY_DEF );
   }

   switch ( type ) {
   case PSO_KEY_INTEGER:
      if ( length != 1 && length != 2 && length != 4 && length != 8 ) {
         throw pso::Exception( "ObjDefinition::AddKey",
                               PSO_INVALID_KEY_DEF );
      }
      pDefinition->key.type = type;
      pDefinition->key.length = length;
      keyAdded = true;
      break;

   case PSO_KEY_BINARY:
   case PSO_KEY_STRING:
      if ( length == 0 ) {
         throw pso::Exception( "ObjDefinition::AddKey",
                               PSO_INVALID_KEY_DEF );
      }
      pDefinition->key.type = type;
      pDefinition->key.length = length;
      keyAdded = true;
      break;

   case PSO_KEY_VAR_BINARY:
   case PSO_KEY_VAR_STRING:
      if ( maxLength != 0 && maxLength < minLength ) {
         throw pso::Exception( "ObjDefinition::AddKey",
                               PSO_INVALID_KEY_DEF );
      }
      pDefinition->key.type = type;
      pDefinition->key.minLength = minLength;
      pDefinition->key.maxLength = maxLength;
      keyAdded = true;
      break;

   default:
      throw pso::Exception( "ObjDefinition::AddKey",
                            PSO_INVALID_KEY_DEF );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void ObjDefinition::ObjectType( enum psoObjectType type )
{
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "ObjDefinition::ObjectType",
                            PSO_WRONG_OBJECT_TYPE );
   }

   if ( pDefinition == NULL ) {
      throw pso::Exception( "ObjDefinition::ObjectType", PSO_NULL_POINTER );
   }
   pDefinition->type = type;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

enum psoObjectType ObjDefinition::ObjectType()
{
   if ( pDefinition == NULL ) {
      throw pso::Exception( "ObjDefinition::ObjectType", PSO_NULL_POINTER );
   }

   return pDefinition->type;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void ObjDefinition::Reset( uint32_t numberOfFields, enum psoObjectType type )
{
   psoObjectDefinition * tmp;
   
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "ObjDefinition::Reset",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "ObjDefinition::Reset",
                            PSO_WRONG_OBJECT_TYPE );
   }
   currentField = numberOfFields;
   
   // using calloc - being lazy...
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      numberOfFields * sizeof(psoFieldDefinition);
   tmp = (psoObjectDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw pso::Exception( "ObjDefinition::Reset",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   if ( pDefinition != NULL ) free( pDefinition );
   pDefinition = tmp;
   
   pDefinition->numFields = numberOfFields;
   pDefinition->type = type;
   currentField = 0;
   keyAdded = false;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void ObjDefinition::Reset( psoObjectDefinition & inputDef )
{
   psoObjectDefinition * tmp;
   
   if ( inputDef.numFields == 0 || inputDef.numFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "ObjDefinition::Reset",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( inputDef.type < PSO_FOLDER || inputDef.type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "ObjDefinition::Reset",
                            PSO_WRONG_OBJECT_TYPE );
   }
   
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      inputDef.numFields * sizeof(psoFieldDefinition);
   tmp = (psoObjectDefinition *)malloc( len );
   if ( tmp == NULL ) {
      throw pso::Exception( "ObjDefinition::Reset",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   if ( pDefinition != NULL ) free( pDefinition );
   pDefinition = tmp;
   memcpy( pDefinition, &inputDef, len );
   
   currentField = inputDef.numFields;
   keyAdded = true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const psoObjectDefinition & ObjDefinition::GetDef()
{
   return * pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

