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
#include <photon/DataDefBuilderUser>

using namespace std;
using namespace pso;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefBuilderUser::DataDefBuilderUser()
   : field         ( NULL ),
     currentField  ( 0 ),
     currentLength ( 0 )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

DataDefBuilderUser::~DataDefBuilderUser()
{
   if ( field ) delete [] field;
   field = NULL;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void DataDefBuilderUser::AddField( string fieldDescriptor )
{
   unsigned char * tmp = NULL;
   size_t length;
   
   if ( currentField >= PSO_MAX_FIELDS ) {
      throw pso::Exception( "DataDefBuilderUser::AddField",
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

   currentField++;
   field = tmp;
   currentLength = length;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

