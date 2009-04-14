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
#include <photon/KeyDefBuilderUser>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefBuilderUser::KeyDefBuilderUser( int numKeyFields )
   : keys          ( NULL ),
     numKeys       ( numKeyFields ),
     currentKey    ( 0 ),
     currentLength ( 0 )
{
   if ( numKeyFields <= 0 || numKeyFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "KeyDefBuilderUser::KeyDefBuilderUser",
                            PSO_INVALID_NUM_FIELDS );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

KeyDefBuilderUser::~KeyDefBuilderUser()
{
   if ( keys ) delete [] keys;
   keys = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void KeyDefBuilderUser::AddKeyField( std::string fieldDescriptor )
{
   unsigned char * tmp = NULL;
   size_t length;

   if ( currentKey >= numKeys ) {
      throw pso::Exception( "KeyDefBuilderUser::AddKeyField",
                            PSO_INVALID_NUM_FIELDS );
   }

   if ( fieldDescriptor.length() == 0 ) {
      throw pso::Exception( "KeyDefBuilderUser::AddKeyField",
                            PSO_INVALID_LENGTH );
   }

   if ( currentLength == 0 ) {
      length = fieldDescriptor.length();
      tmp = new unsigned char [length];
   }
   else {
      length = currentLength + fieldDescriptor.length() + 1;
      tmp = new unsigned char [length];
      memcpy( tmp, keys, currentLength );
      tmp[currentLength] = '\0';
      currentLength++;
      delete [] keys;
   }
   memcpy( &tmp[currentLength], fieldDescriptor.c_str(), fieldDescriptor.length() );

   currentKey++;
   keys = tmp;
   currentLength = length;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

const unsigned char * KeyDefBuilderUser::GetDefinition()
{
   if ( keys == NULL ) {
      throw pso::Exception( "KeyDefBuilderUser::GetDefinition", PSO_NULL_POINTER );
   }

   if ( currentKey != numKeys ) {
      throw pso::Exception( "KeyDefBuilderUser::GetDefinition",
                            PSO_INVALID_NUM_FIELDS );
   }

   return keys;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* Returns the length, in bytes, of the definition. */
uint32_t KeyDefBuilderUser::GetDefLength()
{
   if ( keys == NULL ) {
      throw pso::Exception( "KeyDefBuilderUser::GetDefinition", PSO_NULL_POINTER );
   }

   if ( currentKey != numKeys ) {
      throw pso::Exception( "KeyDefBuilderUser::GetDefinition",
                            PSO_INVALID_NUM_FIELDS );
   }

   return currentLength;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

