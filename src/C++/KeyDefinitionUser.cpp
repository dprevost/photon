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

KeyDefinitionUser::KeyDefinitionUser( unsigned char * serialKeyDef,
                                      uint32_t        keyDefLen )
   : KeyDefinition( true ),
     key          ( NULL ),
     numKeys       ( 0 ),
     currentKey    ( 0 ),
     currentLength ( 0 ),
     maxLength     ( keyDefLen )
{
   if ( serialKeyDef == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser", 
                            PSO_NULL_POINTER );
   }
   if ( keyDefLen == 0 ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser", 
                            PSO_INVALID_LENGTH );
   }

   key = new unsigned char [keyDefLen];
   memcpy( key, serialKeyDef, keyDefLen );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionUser::KeyDefinitionUser( uint32_t numKeyFields )
   : KeyDefinition( false ),
     key           ( NULL ),
     numKeys       ( numKeyFields ),
     currentKey    ( 0 ),
     currentLength ( 0 ),
     maxLength     ( 0 )
{
   if ( numKeyFields == 0 || numKeyFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefinitionUser::KeyDefinitionUser",
                            PSO_INVALID_NUM_FIELDS );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefinitionUser::~KeyDefinitionUser()
{
   if ( key ) delete [] key;
   key = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefinitionUser::AddKeyField( std::string fieldDescriptor )
{
   unsigned char * tmp = NULL;
   size_t length;

   if ( readOnly ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                            PSO_INVALID_DEF_OPERATION );
   }

   if ( currentKey >= numKeys ) {
      throw pso::Exception( "KeyDefinitionUser::AddKeyField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( currentLength == 0 ) {
      length = fieldDescriptor.length();
      tmp = new unsigned char [length];
   }
   else {
      length = currentLength + fieldDescriptor.length() + 1;
      tmp = new unsigned char [length];
      memcpy( tmp, key, currentLength );
      tmp[currentLength] = '\0';
      currentLength++;
      delete [] key;
   }
   memcpy( &tmp[currentLength], fieldDescriptor.c_str(), fieldDescriptor.length() );

   key = tmp;
   maxLength = currentLength = length;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string KeyDefinitionUser::GetNext() 
{
   string s;
   uint32_t i;
   
   if ( ! readOnly ) {
      throw pso::Exception( "KeyDefinitionUser::GetNext",
                            PSO_INVALID_DEF_OPERATION );
   }
   if ( key == NULL ) {
      throw pso::Exception( "KeyDefinitionUser::GetNext", PSO_NULL_POINTER );
   }

   if ( currentLength >= maxLength ) return s;

   for ( i = currentLength; i < maxLength; ++i ) {
      if ( key[i] == 0 ) {
         currentLength = i + 1;
         break;
      }
      s += key[i];
   }

   return s;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * KeyDefinitionUser::GetDefinition()
{
   return key;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* Returns the length, in bytes, of the definition. */
uint32_t KeyDefinitionUser::GetDefLength()
{
   return maxLength;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

