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
#include <photon/KeyDefinitionUser>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionODBC::KeyDefinitionUser( unsigned char * serialKeyDef,
                                      uint32_t        keyDefLen )
   : KeyDefinition( true ),
     key          ( NULL ),
     currentKey   ( 0 ),
     simpleDef    ( true )
{
   if ( serialKeyDef == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser", 
                            PSO_NULL_POINTER );
   }
   if ( keyDefLen == 0 ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser", 
                            PSO_INVALID_LENGTH );
   }
   if ( (keyDefLen%sizeof(psoKeyDefinition)) != 0 ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser", 
                            PSO_INVALID_LENGTH );
   }

   numKeys = keyDefLen / sizeof(psoKeyDefinition);

   key = (psoKeyDefinition *) malloc( keyDefLen );
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   memcpy( key, serialKeyDef, keyDefLen );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionUser::KeyDefinitionUser( uint32_t numKeyFields,
                                      bool     simple /* = true */ )
   : KeyDefinition( false ),
     key          ( NULL ),
     numKeys      ( numKeyFields ),
     currentKey   ( 0 ),
     simpleDef    ( simple )
{
   if ( numKeyFields == 0 || numKeyFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   // using calloc - being lazy...
   size_t len = numKeyFields * sizeof(psoKeyDefinition);
   key = (psoKeyDefinition *)calloc( len, 1 );
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionUser::~KeyDefinitionUser()
{
   if ( key ) free( key );
   key = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinitionUser::AddKeyField( std::string & name,
                                     psoKeyType    type,
                                     uint32_t      length )
{
   AddKeyField( name.c_str(),
                name.length(),
                type,
                length );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinitionUser::AddKeyField( const char * name,
                                     uint32_t     nameLength,
                                     psoKeyType   type,
                                     uint32_t     length )
{
   if ( readOnly ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                            PSO_INVALID_DEF_OPERATION );
   }

   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField", PSO_NULL_POINTER );
   }

   if ( currentKey >= numKeys ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( name == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                            PSO_NULL_POINTER );
   }

   if ( nameLength == 0 || nameLength > PSO_MAX_FIELD_LENGTH ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField",
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
         throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      key[currentKey].type = type;
      key[currentKey].length = length;
      currentKey++;
      break;

   case PSO_KEY_VARBINARY:
   case PSO_KEY_VARCHAR:
      if ( simpleDef && currentKey != numKeys-1 ) {
         throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                               PSO_INVALID_FIELD_TYPE );
      }
      if ( length == 0 ) {
         throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                               PSO_INVALID_FIELD_LENGTH );
      }
      key[currentKey].type = type;
      key[currentKey].length = length;
      currentKey++;
      break;

   case PSO_KEY_LONGVARBINARY:
   case PSO_KEY_LONGVARCHAR:
      if ( simpleDef && currentKey != numKeys-1 ) {
         throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                               PSO_INVALID_FIELD_TYPE );
      }
      key[currentKey].type = type;
      key[currentKey].length = 0;
      currentKey++;
      break;

   default:
      throw pso::Exception( "KeyDefinitionUser::AddKeyLength",
                            PSO_INVALID_FIELD_TYPE );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string KeyDefinitionUser::GetNext() 
{
   string s;
   char name [PSO_MAX_FIELD_LENGTH+1];
   
   if ( ! readOnly ) {
      throw pso::Exception( "KeyDefinitionUser::GetNext",
                            PSO_INVALID_DEF_OPERATION );
   }
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::GetNext", PSO_NULL_POINTER );
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
      throw pso::Exception( "KeyDefinitionUser::GetNext",
                            PSO_INVALID_FIELD_TYPE );
   }

   currentKey++;

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * KeyDefinitionUser::GetDefinition()
{
   return (unsigned char *)key;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* Returns the length, in bytes, of the definition. */
uint32_t KeyDefinitionUser::GetDefLength()
{
   return numKeys * sizeof(psoKeyDefinition);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

