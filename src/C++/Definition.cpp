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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoDefinition::psoDefinition( size_t numberOfFields )
   : pDefinition  ( NULL ),
     currentField ( 0 ),
     keyAdded     ( false )
{
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw psoException( "psoDefinition::psoDefinition",
                          PSO_INVALID_NUM_FIELDS );
   }
   
   // using calloc - being lazy...
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      numberOfFields * sizeof(psoFieldDefinition);
   pDefinition = (psoObjectDefinition *)calloc( len, 1 );
   if ( pDefinition == NULL ) {
      throw psoException( "psoDefinition::psoDefinition",
                          PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   pDefinition->numFields = numberOfFields;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoDefinition::~psoDefinition()
{
   if ( pDefinition ) free( pDefinition );
   pDefinition = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::AddField( std::string  & name,
                              psoFieldType   type,
                              size_t         length,
                              size_t         minLength,
                              size_t         maxLength,
                              size_t         precision,
                              size_t         scale )
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

void psoDefinition::AddField( const char   * name,
                              size_t         nameLength,
                              psoFieldType   type,
                              size_t         length,
                              size_t         minLength,
                              size_t         maxLength,
                              size_t         precision,
                              size_t         scale )
{
   if ( currentField >= pDefinition->numFields ) {
      throw psoException( "psoDefinition::AddField",
                          PSO_INVALID_NUM_FIELDS );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw psoException( "psoDefinition::AddField",
                          PSO_INVALID_FIELD_NAME );
   }
   memcpy( pDefinition->fields[currentField].name, name, nameLength );
   
   switch ( type ) {
   case PSO_INTEGER:
      if ( length != 1 && length != 2 && length != 4 && length != 8 ) {
      throw psoException( "psoDefinition::AddField",
                          PSO_INVALID_FIELD_LENGTH_INT );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].length = length;
      currentField++;
      break;

   case PSO_BINARY:
   case PSO_STRING:
      if ( length == 0 ) {
         throw psoException( "psoDefinition::AddField",
                             PSO_INVALID_FIELD_LENGTH );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].length = length;
      currentField++;
      break;

   case PSO_VAR_BINARY:
   case PSO_VAR_STRING:
      if ( currentField != pDefinition->numFields-1 ) {
         throw psoException( "psoDefinition::AddField",
                             PSO_INVALID_FIELD_TYPE );
      }
      if ( maxLength != 0 && maxLength < minLength ) {
         throw psoException( "psoDefinition::AddField",
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
         throw psoException( "psoDefinition::AddField",
                             PSO_INVALID_PRECISION );
      }
      if ( scale > precision ) {
         throw psoException( "psoDefinition::AddField",
                             PSO_INVALID_SCALE );
      }
      pDefinition->fields[currentField].type = type;
      pDefinition->fields[currentField].precision = precision;
      pDefinition->fields[currentField].scale = scale;
      currentField++;
      break;

   default:
      throw psoException( "psoDefinition::AddField",
                          PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::AddKey( psoKeyType type,
                            size_t     length,
                            size_t     minLength,
                            size_t     maxLength )
{
   if ( keyAdded ) {
      throw psoException( "psoDefinition::AddKey",
                          PSO_INVALID_KEY_DEF );
   }

   switch ( type ) {
   case PSO_KEY_INTEGER:
      if ( length != 1 && length != 2 && length != 4 && length != 8 ) {
         throw psoException( "psoDefinition::AddKey",
                             PSO_INVALID_KEY_DEF );
      }
      pDefinition->key.type = type;
      pDefinition->key.length = length;
      keyAdded = true;
      break;

   case PSO_KEY_BINARY:
   case PSO_KEY_STRING:
      if ( length == 0 ) {
         throw psoException( "psoDefinition::AddKey",
                             PSO_INVALID_KEY_DEF );
      }
      pDefinition->key.type = type;
      pDefinition->key.length = length;
      keyAdded = true;
      break;

   case PSO_KEY_VAR_BINARY:
   case PSO_KEY_VAR_STRING:
      if ( maxLength != 0 && maxLength < minLength ) {
         throw psoException( "psoDefinition::AddKey",
                             PSO_INVALID_KEY_DEF );
      }
      pDefinition->key.type = type;
      pDefinition->key.minLength = minLength;
      pDefinition->key.maxLength = maxLength;
      keyAdded = true;
      break;

   default:
      throw psoException( "psoDefinition::AddKey",
                          PSO_INVALID_KEY_DEF );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::ObjectType( psoObjectType type )
{
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw psoException( "psoDefinition::ObjectType",
                          PSO_WRONG_OBJECT_TYPE );
   }
   
   pDefinition->type = type;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::Reset( size_t numberOfFields )
{
   psoObjectDefinition * tmp;
   
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw psoException( "psoDefinition::psoDefinition",
                          PSO_INVALID_NUM_FIELDS );
   }
   currentField = numberOfFields;
   
   // using calloc - being lazy...
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      numberOfFields * sizeof(psoFieldDefinition);
   tmp = (psoObjectDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw psoException( "psoDefinition::Reset",
                          PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   free( pDefinition );
   pDefinition = tmp;
   
   pDefinition->numFields = numberOfFields;
   currentField = 0;
   keyAdded = false;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const psoObjectDefinition & psoDefinition::GetDef()
{
   return * pDefinition;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
