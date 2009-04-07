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
#include <photon/FieldDefinitionUser>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionUser::FieldDefinitionUser( unsigned char * serialFieldDef,
                                          uint32_t        fieldDefLen )
   : FieldDefinition( true ),
     field         ( NULL ),
     numFields     ( 0 ),
     currentField  ( 0 ),
     currentLength ( 0 ),
     maxLength     ( fieldDefLen )
{
   if ( serialFieldDef == NULL ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser", 
                            PSO_NULL_POINTER );
   }
   if ( fieldDefLen == 0 ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser", 
                            PSO_INVALID_LENGTH );
   }

   field = new unsigned char [fieldDefLen];
   memcpy( field, serialFieldDef, fieldDefLen );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionUser::FieldDefinitionUser( uint32_t numFieldFields )
   : FieldDefinition( false ),
     field         ( NULL ),
     numFields     ( numFieldFields ),
     currentField  ( 0 ),
     currentLength ( 0 ),
     maxLength     ( 0 )
{
   if ( numFieldFields == 0 || numFieldFields > PSO_MAX_FIELDS ) {
      throw pso::Exception( "FieldDefinitionUser::FieldDefinitionUser",
                            PSO_INVALID_NUM_FIELDS );
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

FieldDefinitionUser::~FieldDefinitionUser()
{
   if ( field ) delete [] field;
   field = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void FieldDefinitionUser::AddField( std::string fieldDescriptor )
{
   unsigned char * tmp = NULL;
   size_t length;
   
   if ( readOnly ) {
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_INVALID_DEF_OPERATION );
   }

   if ( currentField >= numFields ) {
      throw pso::Exception( "FieldDefinitionUser::AddField",
                            PSO_INVALID_NUM_FIELDS );
   }
   
   if ( currentLength == 0 ) {
      length = fieldDescriptor.length();
      tmp = new unsigned char [length];
   }
   else {
      length = currentLength + fieldDescriptor.length() + 1;
      tmp = new unsigned char [length];
      memcpy( tmp, field, currentLength );
      tmp[currentLength] = '\0';
      currentLength++;
      delete [] field;
   }
   memcpy( &tmp[currentLength], fieldDescriptor.c_str(), fieldDescriptor.length() );

   field = tmp;
   maxLength = currentLength = length;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

