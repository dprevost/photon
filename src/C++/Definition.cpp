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
      throw psoException( PSO_INVALID_NUM_FIELDS, 
                          NULL, 
                          "psoDefinition::psoDefinition" );
   }
   
   // using calloc - being lazy...
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      numberOfFields * sizeof(psoFieldDefinition);
   pDefinition = (psoObjectDefinition *)calloc( len, 1 );
   if ( pDefinition == NULL ) {
      throw psoException( PSO_NOT_ENOUGH_HEAP_MEMORY, 
                          NULL, 
                          "psoDefinition::psoDefinition" );
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
      throw psoException( PSO_INVALID_KEY_DEF, 
                          NULL, 
                          "psoDefinition::AddField" );

}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::AddKey( psoKeyType type,
                            size_t     length,
                            size_t     minLength,
                            size_t     maxLength )
{
   if ( keyAdded ) {
      throw psoException( PSO_INVALID_KEY_DEF, 
                          NULL, 
                          "psoDefinition::AddKey" );
   }

   switch ( type ) {
   case PSO_KEY_INTEGER:
      if ( length == 1 || length == 2 || length == 4 || length == 8 ) {
         pDefinition->key.type = type;
         pDefinition->key.length = length;
         keyAdded = true;
         break;
      }
      throw psoException( PSO_INVALID_KEY_DEF, 
                          NULL, 
                          "psoDefinition::AddKey" );
   case PSO_KEY_BINARY:
   case PSO_KEY_STRING:
      if ( length > 0 ) {
         pDefinition->key.type = type;
         pDefinition->key.length = length;
         keyAdded = true;
         break;
      }
      throw psoException( PSO_INVALID_KEY_DEF, 
                          NULL, 
                          "psoDefinition::AddKey" );
   case PSO_KEY_VAR_BINARY:
   case PSO_KEY_VAR_STRING:
      if ( maxLength == 0 || maxLength >= minLength ) {
         pDefinition->key.type = type;
         pDefinition->key.minLength = minLength;
         pDefinition->key.maxLength = maxLength;
         keyAdded = true;
         break;
      }
      throw psoException( PSO_INVALID_KEY_DEF, 
                          NULL, 
                          "psoDefinition::AddKey" );
   default:
      throw psoException( PSO_INVALID_KEY_DEF, 
                          NULL, 
                          "psoDefinition::AddKey" );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::ObjectType( psoObjectType type )
{
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw psoException( PSO_WRONG_OBJECT_TYPE, 
                          NULL, 
                          "psoDefinition::ObjectType" );
   }
   
   pDefinition->type = type;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoDefinition::Reset( size_t numberOfFields )
{
   psoObjectDefinition * tmp;
   
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw psoException( PSO_INVALID_NUM_FIELDS, 
                          NULL, 
                          "psoDefinition::psoDefinition" );
   }
   currentField = numberOfFields;
   
   // using calloc - being lazy...
   size_t len = offsetof( psoObjectDefinition, fields ) + 
      numberOfFields * sizeof(psoFieldDefinition);
   tmp = (psoObjectDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw psoException( PSO_NOT_ENOUGH_HEAP_MEMORY, 
                          NULL, 
                          "psoDefinition::Reset" );
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
