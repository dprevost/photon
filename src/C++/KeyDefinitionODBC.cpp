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
#include <photon/KeyDefinitionODBC>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionODBC::KeyDefinitionODBC( unsigned char * serialKeyDef,
                                      uint32_t        keyDefLen )
   : KeyDefinition( serialKeyDef, keyDefLen ),
     key          ( NULL ),
     numKeys      ( 0 ),
     currentKey   ( 0 ),
     simpleDef    ( true )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionODBC::KeyDefinitionODBC( uint32_t numKeyFields,
                                      bool     simple /* = true */ )
   : KeyDefinition(),
     key          ( NULL ),
     numKeys      ( numKeyFields ),
     currentKey   ( 0 ),
     simpleDef    ( simple )
{
   if ( numKeyFields == 0 || numKeyFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefinitionODBC::KeyDefinitionODBC",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   // using calloc - being lazy...
   size_t len = numKeyFields * sizeof(psoKeyDefinition);
   key = (psoKeyDefinition *)calloc( len, 1 );
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::KeyDefinitionODBC",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionODBC::~KeyDefinitionODBC()
{
   if ( key ) free( key );
   key = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinitionODBC::AddKeyField( std::string & name,
                                     psoKeyType    type,
                                     uint32_t      length )
{
   AddKeyField( name.c_str(),
                name.length(),
                type,
                length );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinitionODBC::AddKeyField( const char * name,
                                     uint32_t     nameLength,
                                     psoKeyType   type,
                                     uint32_t     length )
{
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField", PSO_NULL_POINTER );
   }

   if ( currentKey >= numKeys ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                            PSO_INVALID_FIELD_NAME );
   }
   memcpy( key[currentKey].name, name, nameLength );
   
   
   switch ( type ) {
   case PSO_KEY_INTEGER:
   case PSO_KEY_BIGINT:
   case PSO_KEY_DATE:
   case PSO_KEY_TIME:
   case PSO_KEY_TIMESTAMP:
      key[currentKey].type = type;
      key[currentKey].length = 0;
      currentKey++;
      break;

   case PSO_KEY_BINARY:
   case PSO_KEY_CHAR:
      if ( length == 0 ) {
         throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      key[currentKey].type = type;
      key[currentKey].length = length;
      currentKey++;
      break;

   case PSO_KEY_VARBINARY:
   case PSO_KEY_VARCHAR:
      if ( currentKey != numKeys-1 ) {
         throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      key[currentKey].type = type;
      key[currentKey].length = length;
      currentKey++;
      break;

   case PSO_KEY_LONGVARBINARY:
   case PSO_KEY_LONGVARCHAR:
      if ( currentKey != numKeys-1 ) {
         throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                               PSO_INVALID_FIELD_TYPE );
      }
      key[currentKey].type = type;
      key[currentKey].length = 0;
      currentKey++;
      break;

   default:
      throw pso::Exception( "KeyDefinitionODBC::AddKeyLength",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string KeyDefinitionODBC::GetNext() 
{
   string s;
   char name [PSO_MAX_FIELD_LENGTH+1];
   
   if ( ! readOnly ) {
      throw pso::Exception( "KeyDefinitionODBC::GetNext",
                            PSO_INVALID_DEF_OPERATION );
   }
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::GetNext", PSO_NULL_POINTER );
   }

   if ( currentKey >= numKeys ) return s;

   s += "Name: ";   
   if ( key[currentKey].name[PSO_MAX_FIELD_LENGTH-1] == '\0' ) {
      s += key[currentKey].name;
   }
   else {
      memcpy( name, key[currentKey].name, PSO_MAX_FIELD_LENGTH );
      name[PSO_MAX_FIELD_LENGTH] = '\0';
      s += name;
   }
      
   s += ", Type: ";
   switch ( key[currentKey].type ) {

   case PSO_KEY_INTEGER:
      s += "Integer";
      break;
   case PSO_KEY_BIGINT:
      s += "BigInt";
      break;
   case PSO_KEY_DATE:
      s += "Date";
      break;
   case PSO_KEY_TIME:
      s += "Time";
      break;
   case PSO_KEY_TIMESTAMP:
      s += "TimeStamp";
      break;

   case PSO_KEY_BINARY:
      s += "Binary, Length: ";
      s += key[currentKey].length;
      break;
   case PSO_KEY_CHAR:
      s += "Char, Length: ";
      s += key[currentKey].length;
      break;

   case PSO_KEY_VARBINARY:
      s += "VarBinary, Length: ";
      s += key[currentKey].length;
      break;

   case PSO_KEY_VARCHAR:
      s += "VarChar, Length: ";
      s += key[currentKey].length;
      break;

   case PSO_KEY_LONGVARBINARY:
      s += "LongVarBinary";
      break;
   case PSO_KEY_LONGVARCHAR:
      s += "LongVarChar";
      break;

   default:
      throw pso::Exception( "KeyDefinitionODBC::GetNext",
                            PSO_INVALID_FIELD_TYPE );
   }

   currentKey++;

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
void KeyDefinitionODBC::Reset( uint32_t numberOfFields, enum psoObjectType type )
{
   psoFieldDefinition * tmp;
   
   if ( numberOfFields == 0 || numberOfFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( type < PSO_FOLDER || type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_WRONG_OBJECT_TYPE );
   }
   currentField = numberOfFields;
   
   memset( &definition, 0, sizeof(psoObjectDefinition) );
   memset( &key, 0, sizeof(psoKeyDefinition) );
   
   // using calloc - being lazy...
   size_t len = numberOfFields * sizeof(psoFieldDefinition);
   tmp = (psoFieldDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   if ( fields != NULL ) free( fields );
   fields = tmp;
   
   numFields = numberOfFields;
   definition.type = type;
   currentField = 0;
   keyAdded = false;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
void KeyDefinitionODBC::Reset( psoObjectDefinition & inputDef,
                           psoKeyDefinition    * inputKey,
                           psoFieldDefinition  * inputFields )
{
   psoFieldDefinition * tmp;
   
   if ( inputDef.numFields == 0 || inputDef.numFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_INVALID_NUM_FIELDS );
   }
   if ( inputFields == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_NULL_POINTER );
   }
   if ( inputDef.type < PSO_FOLDER || inputDef.type >= PSO_LAST_OBJECT_TYPE ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_WRONG_OBJECT_TYPE );
   }
   
   size_t len = inputDef.numFields * sizeof(psoFieldDefinition);
   tmp = (psoFieldDefinition *)calloc( len, 1 );
   if ( tmp == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::Reset",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   if ( fields != NULL ) free( fields );
   fields = tmp;

   memcpy( &definition, &inputDef, sizeof(psoObjectDefinition) );
   memcpy( fields, inputFields, len );
   
   currentField = inputDef.numFields;
   
   keyAdded = false;
   if ( inputKey != NULL ) {
      memcpy( &key, inputKey, sizeof(psoKeyDefinition) );
      keyAdded = true;
   }
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0
const unsigned char * KeyDefinitionODBC::GetKey()
{
   return key;
}

#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

