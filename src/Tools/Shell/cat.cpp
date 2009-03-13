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

#include "Tools/Shell/shell.h"
#include "Tools/Shell/cat.h"
#include <sstream>
#include "API/DataDefinition.h" // for psoaGet*Offsets

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoCat::psoCat( Session s, string name ) 
   : session       ( s ),
     objectName    ( name ),
     key           ( NULL ),
     buffer        ( NULL ),
     keyOffsets    ( NULL ),
     fieldOffsets  ( NULL ),
     fieldDef      ( NULL ),
     keyDef        ( NULL ),
     fieldDefLength( 0 ),
     keyDefLength  ( 0 )
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoCat::~psoCat()
{
   if ( key          != NULL ) delete [] key;
   if ( buffer       != NULL ) delete [] buffer;
   if ( keyOffsets   != NULL ) delete [] keyOffsets;
   if ( fieldOffsets != NULL ) delete [] fieldOffsets;
   if ( fieldDef     != NULL ) delete [] fieldDef;
   if ( keyDef       != NULL ) delete [] keyDef;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::Init()
{
   stringstream stream;
   int numKeys = 0, numFields = 0;
   
   try {
      session.GetStatus( objectName, status );
   }
   catch ( Exception exc ) {
      exc = exc; // Avoid a warning
      stream << "psosh: cat: " << objectName << ": Invalid object name";
      return stream.str();
   }
   if ( status.type == PSO_FOLDER ) {
      stream << "psosh: cat: " << objectName << ": Is a directory/folder";
      return stream.str();
   }
   
   if ( status.numDataItem == 0 ) {
      stream << "psosh: cat: " << objectName << ": is empty";
   }
   
   try {
      session.GetDefinitionLength( objectName, &keyDefLength, &fieldDefLength );
   }
   catch ( Exception exc ) {
      stream << "psosh: cat: " << exc.Message();
      return stream.str();
   }

   try {
      if ( keyDefLength > 0 ) {
         keyDef = new unsigned char [keyDefLength];
      }
      if ( fieldDefLength > 0 ) {
         fieldDef = new unsigned char [fieldDefLength];
      }
   }
   catch ( ... ) {
      stream << "psosh: cat: Not enough memory ";
      // The first new might have succeeded
      if ( keyDef != NULL ) delete [] keyDef;
      return stream.str();
   }
   
   try {
      session.GetDefinition( objectName,
                             objDefinition,
                             keyDef,
                             keyDefLength,
                             fieldDef,
                             fieldDefLength );
   }
   catch ( Exception exc ) {
      stream << "psosh: cat: " << exc.Message();
      return stream.str();
   }
   numKeys = keyDefLength/sizeof(psoKeyDefinition);
   numFields = fieldDefLength/sizeof(psoFieldDefinition);
   
   if ( numKeys > 0 && objDefinition.keyDefType == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
      try {
         keyOffsets = new uint32_t[numKeys];
      }
      catch ( ... ) {
         stream << "psosh: cat: Not enough memory " << endl;
         stream << "Number of keys in data definition = " << keyDefLength/sizeof(psoKeyDefinition);
         return stream.str();
      }
      psoaGetKeyOffsets( (psoKeyDefinition *)keyDef, numKeys, keyOffsets );
   }

   if ( numFields > 0 && objDefinition.fieldDefType == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
      try {
         fieldOffsets = new uint32_t[numFields];
      }
      catch ( ... ) {
         stream << "psosh: cat: Not enough memory " << endl;
         stream << "Number of fields in data definition = " << fieldDefLength/sizeof(psoFieldDefinition);
         return stream.str();
      }
      psoaGetFieldOffsets( (psoFieldDefinition *)fieldDef, numFields, fieldOffsets );
   }

   try {
      if ( status.maxDataLength > 0 ) {
         buffer = new unsigned char[status.maxDataLength];
      }
      if ( status.maxKeyLength > 0 ) {
         key = new unsigned char[status.maxKeyLength];
      }
   }
   catch ( ... ) {
      stream << "psosh: cat: Not enough memory ";
      return stream.str();
   }

   return "";
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
