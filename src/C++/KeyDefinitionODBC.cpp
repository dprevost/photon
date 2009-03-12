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
#include <sstream>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionODBC::KeyDefinitionODBC( unsigned char * serialKeyDef,
                                      uint32_t        keyDefLen )
   : KeyDefinition( true ),
     key          ( NULL ),
     currentKey   ( 0 ),
     simpleDef    ( true )
{
   if ( serialKeyDef == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::KeyDefinitionODBC", 
                            PSO_NULL_POINTER );
   }
   if ( keyDefLen == 0 ) {
      throw pso::Exception( "KeyDefinitionODBC::KeyDefinitionODBC", 
                            PSO_INVALID_LENGTH );
   }
   if ( (keyDefLen%sizeof(psoKeyDefinition)) != 0 ) {
      throw pso::Exception( "KeyDefinitionODBC::KeyDefinitionODBC", 
                            PSO_INVALID_LENGTH );
   }

   numKeys = keyDefLen / sizeof(psoKeyDefinition);

   key = (psoKeyDefinition *) malloc( keyDefLen );
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::KeyDefinitionODBC",
                            PSO_NOT_ENOUGH_HEAP_MEMORY );
   }
   memcpy( key, serialKeyDef, keyDefLen );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionODBC::KeyDefinitionODBC( uint32_t numKeyFields,
                                      bool     simple /* = true */ )
   : KeyDefinition( false ),
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
   if ( readOnly ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                            PSO_INVALID_DEF_OPERATION );
   }

   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField", PSO_NULL_POINTER );
   }

   if ( currentKey >= numKeys ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( name == NULL ) {
      throw pso::Exception( "KeyDefinitionODBC::AddKeyField",
                            PSO_NULL_POINTER );
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
      if ( simpleDef && currentKey != numKeys-1 ) {
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
      if ( simpleDef && currentKey != numKeys-1 ) {
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
   stringstream stream;
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

   stream << "Name: ";   
   if ( key[currentKey].name[PSO_MAX_FIELD_LENGTH-1] == '\0' ) {
      stream << key[currentKey].name;
   }
   else {
      memcpy( name, key[currentKey].name, PSO_MAX_FIELD_LENGTH );
      name[PSO_MAX_FIELD_LENGTH] = '\0';
      stream << name;
   }
      
   stream << ", Type: ";
   switch ( key[currentKey].type ) {

   case PSO_KEY_INTEGER:
      stream << "Integer";
      break;
   case PSO_KEY_BIGINT:
      stream << "BigInt";
      break;
   case PSO_KEY_DATE:
      stream << "Date";
      break;
   case PSO_KEY_TIME:
      stream << "Time";
      break;
   case PSO_KEY_TIMESTAMP:
      stream << "TimeStamp";
      break;

   case PSO_KEY_BINARY:
      stream << "Binary, Length: ";
      stream << key[currentKey].length;
      break;
   case PSO_KEY_CHAR:
      stream << "Char, Length: ";
      stream << key[currentKey].length;
      break;

   case PSO_KEY_VARBINARY:
      stream << "VarBinary, Length: ";
      stream << key[currentKey].length;
      break;

   case PSO_KEY_VARCHAR:
      stream << "VarChar, Length: ";
      stream << key[currentKey].length;
      break;

   case PSO_KEY_LONGVARBINARY:
      stream << "LongVarBinary";
      break;
   case PSO_KEY_LONGVARCHAR:
      stream << "LongVarChar";
      break;

   default:
      throw pso::Exception( "KeyDefinitionODBC::GetNext",
                            PSO_INVALID_FIELD_TYPE );
   }

   currentKey++;
   s = stream.str();

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * KeyDefinitionODBC::GetDefinition()
{
   return (unsigned char *)key;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* Returns the length, in bytes, of the definition. */
uint32_t KeyDefinitionODBC::GetDefLength()
{
   return numKeys * sizeof(psoKeyDefinition);
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

