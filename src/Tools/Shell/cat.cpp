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
#include "Tools/Shell/util.h"

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

void psoCat::Run()
{
   int rc;
   uint32_t keyLength, dataLength;
   
   if ( status.type == PSO_HASH_MAP ) {
      HashMap hashMap(session);
      
      hashMap.Open( objectName );
      
      memset( key, 0, status.maxKeyLength );
      memset( buffer, 0, status.maxDataLength );
      rc = hashMap.GetFirst( key, status.maxKeyLength, 
                             buffer, status.maxDataLength,
                             keyLength, dataLength );
      while ( rc == 0 ) {
         string keyStr, dataStr;
               
         if ( fieldOffsets != NULL ) {
            dataStr = odbcFieldToStr( dataLength );
         }
         else {
            dataStr = binToStr( buffer, dataLength );
         }

         if ( keyOffsets != NULL ) {
            keyStr = odbcKeyToStr( keyLength );
         }
         else {
            keyStr = binToStr( key, keyLength );
         }
    
         cout << "key : " << keyStr << endl;
         cout << "data: " << dataStr << endl;
         memset( key, 0, status.maxKeyLength );
         memset( buffer, 0, status.maxDataLength );
         rc = hashMap.GetNext( key, status.maxKeyLength, 
                               buffer, status.maxDataLength,
                               keyLength, dataLength );
      }
      hashMap.Close();
   }
   else if ( status.type == PSO_FAST_MAP ) {
      FastMap hashMap(session);
         
      hashMap.Open( objectName );
         
      memset( key, 0, status.maxKeyLength );
      memset( buffer, 0, status.maxDataLength );
      rc = hashMap.GetFirst( key, status.maxKeyLength, 
                             buffer, status.maxDataLength,
                             keyLength, dataLength );
      while ( rc == 0 ) {
         string keyStr, dataStr;
               
         if ( fieldOffsets != NULL ) {
            dataStr = odbcFieldToStr( dataLength );
         }
         else {
            dataStr = binToStr( buffer, dataLength );
         }

         if ( keyOffsets != NULL ) {
            keyStr = odbcKeyToStr( keyLength );
         }
         else {
            keyStr = binToStr( key, keyLength );
         }

         cout << "key : " << keyStr << endl;
         cout << "data: " << dataStr << endl;
         memset( key, 0, status.maxKeyLength );
         memset( buffer, 0, status.maxDataLength );
         rc = hashMap.GetNext( key, status.maxKeyLength, 
                               buffer, status.maxDataLength,
                               keyLength, dataLength );
      }
      hashMap.Close();
   }
   else if ( status.type == PSO_QUEUE ) {
      Queue queue(session);
      
      queue.Open( objectName );

      memset( buffer, 0, status.maxDataLength );
      rc = queue.GetFirst( buffer, status.maxDataLength, dataLength );
      while ( rc == 0 ) {
         string dataStr;
               
         if ( fieldOffsets != NULL ) {
            dataStr = odbcFieldToStr( dataLength );
         }
         else {
            dataStr = binToStr( buffer, dataLength );
         }

         cout << dataStr << endl;
         memset( buffer, 0, status.maxDataLength );
         rc = queue.GetNext( buffer, status.maxDataLength, dataLength );
      }
      queue.Close();
   }
   else if ( status.type == PSO_LIFO ) {
      Lifo queue(session);
      
      queue.Open( objectName );

      memset( buffer, 0, status.maxDataLength );
      rc = queue.GetFirst( buffer, status.maxDataLength, dataLength );
      while ( rc == 0 ) {
         string dataStr;

         if ( fieldOffsets != NULL ) {
            dataStr = odbcFieldToStr( dataLength );
         }
         else {
            dataStr = binToStr( buffer, dataLength );
         }

         cout << dataStr << endl;
         memset( buffer, 0, status.maxDataLength );
         rc = queue.GetNext( buffer, status.maxDataLength, dataLength );
      }
      queue.Close();
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
// Private methods
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::binToStr( unsigned char * buff, uint32_t length )
{
   uint32_t i;
   stringstream stream;
   unsigned char c;
   
   stream << "0X" << hex;
   stream.width(2);
   for ( i = 0; i < length; ++i ) {
      c = buff[i];
      stream << (int) c;
   }
   return stream.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::odbcFieldToStr( uint32_t length )
{
   psoFieldDefinition * fieldODBC;
   int numFields, i;
   string outStr;
   
   fieldODBC = (psoFieldDefinition *) fieldDef;
   numFields = fieldDefLength / sizeof(psoFieldDefinition);
   
   psoaGetFieldOffsets( fieldODBC, numFields, fieldOffsets );

   for ( i = 0; i < numFields; ++i ) {
      string s;
      
      switch( fieldODBC[i].type ) {

      case PSO_TINYINT:
         readInt( s, 1, &buffer[fieldOffsets[i]] );
         break;

      case PSO_SMALLINT:
         readInt( s, 2, &buffer[fieldOffsets[i]] );
         break;

      case PSO_INTEGER:
         readInt( s, 4, &buffer[fieldOffsets[i]] );
         break;

      case PSO_BIGINT:
         readInt( s, 8, &buffer[fieldOffsets[i]] );
         break;

      case PSO_BINARY:
         readBinary( s,
                     fieldODBC[i].vals.length,
                     &buffer[fieldOffsets[i]] );
         break;

      case PSO_CHAR:
         readString( s,
                     fieldODBC[i].vals.length,
                     &buffer[fieldOffsets[i]] );
         break;

      case PSO_NUMERIC:
         readDecimal( s,
                      fieldODBC[i].vals.decimal.precision,
                      &buffer[fieldOffsets[i]] );
         break;

      case PSO_VARCHAR:
      case PSO_LONGVARCHAR:
         readString( s,
                     length - fieldOffsets[i],
                     &buffer[fieldOffsets[i]] );
         break;
      case PSO_VARBINARY:
      case PSO_LONGVARBINARY:
         readBinary( s,
                     length - fieldOffsets[i],
                     &buffer[fieldOffsets[i]] );
         break;

      case PSO_REAL:
         readFloat( s, 4, &buffer[fieldOffsets[i]] );
         break;

      case PSO_DOUBLE:
         readFloat( s, 4, &buffer[fieldOffsets[i]] );
         break;
      
      case PSO_DATE:
         s = readDate( &buffer[fieldOffsets[i]] );
         break;

      case PSO_TIME:
         s = readTime( &buffer[fieldOffsets[i]] );
         break;

      case PSO_TIMESTAMP:
         s = readTimeStamp( &buffer[fieldOffsets[i]] );
         break;
      }
      outStr += s;
      if ( i < numFields-1) outStr += ", ";
   }
   
   return outStr;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::odbcKeyToStr( uint32_t length )
{
   psoKeyDefinition * keyODBC;
   int numKeys, i;
   string outStr;
   
   keyODBC = (psoKeyDefinition *) keyDef;
   numKeys = keyDefLength / sizeof(psoKeyDefinition);
   
   psoaGetKeyOffsets( keyODBC, numKeys, keyOffsets );

   for ( i = 0; i < numKeys; ++i ) {
      string s;
      
      switch( keyODBC[i].type ) {

      case PSO_KEY_INTEGER:
         readInt( s, 4, &key[keyOffsets[i]] );
         break;

      case PSO_KEY_BIGINT:
         readInt( s, 8, &key[keyOffsets[i]] );
         break;

      case PSO_KEY_BINARY:
         readBinary( s,
                     keyODBC[i].length,
                     &key[keyOffsets[i]] );
         break;

      case PSO_KEY_CHAR:
         readString( s,
                     keyODBC[i].length,
                     &key[keyOffsets[i]] );
         break;

      case PSO_KEY_VARCHAR:
      case PSO_KEY_LONGVARCHAR:
         readString( s,
                     length - keyOffsets[i],
                     &key[keyOffsets[i]] );
         break;
      case PSO_KEY_VARBINARY:
      case PSO_KEY_LONGVARBINARY:
         readBinary( s,
                     length - keyOffsets[i],
                     &key[keyOffsets[i]] );
         break;

      case PSO_KEY_DATE:
         s = readDate( &key[keyOffsets[i]] );
         break;

      case PSO_KEY_TIME:
         s = readTime( &key[keyOffsets[i]] );
         break;

      case PSO_KEY_TIMESTAMP:
         s = readTimeStamp( &key[keyOffsets[i]] );
         break;
      }
      outStr += s;
      if ( i < numKeys-1) outStr += ", ";
   }
   
   return outStr;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoCat::readInt( string        & outStr,
                      size_t          intLength,
                      unsigned char * buffer )
{
   ostringstream oss;
   
   int8_t  i8;
   int16_t i16;
   int32_t i32;
#if ! defined (WIN32)
   int64_t i64;
#endif

   switch( intLength ) {
   case 1:
      i8 = buffer[0];
      oss << (int) i8; // Not a single character, force it to be an int.
      break;
   case 2:
      i16 = *( (int16_t *)buffer);
      oss << i16;
      break;
   case 4:
      i32 = *( (int32_t *)buffer);
      oss << i32;
      break;
   case 8:
#if defined (WIN32)
      /*
       * On some versions of VC++, the overloaded << operator does not
       * work for 64 bits integer. 
       *
       * \todo: To make sure that this #if only applies to the versions
       * without the appropriate support.
       */
      i32 = *( (int32_t *)&buffer[4]);
      oss << i32;
      i32 = *( (int32_t *)buffer);
      oss << i32;
#else
      i64 = *( (int64_t *)buffer);
      oss << i64;
#endif
      break;
   }
   
   outStr = oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoCat::readString( string        & outStr,
                         size_t          strLength,
                         unsigned char * buffer )
{
   size_t i;
   
   for ( i = 0; i < strLength; ++i )
      outStr += (char) buffer[i];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void psoCat::readBinary( string        & outStr,
                         size_t          binLength,
                         unsigned char * buffer )
{
   size_t i;
   unsigned char x;
   
   outStr = "0x";
   
   for ( i = 0; i < binLength; ++i ) {
//      cerr << " bin i = " << i << ", val = " << hex << (int) buffer[i] << endl;
      x = (buffer[i] & 0xf0) >> 4;
      if ( x > 9 ) outStr += (char) (x + 'a' - 10);
      else outStr += (char) (x + '0');
      x = buffer[i] & 0x0f;
      if ( x > 9 ) outStr += (char) (x + 'a' - 10);
      else outStr += (char) (x + '0');
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

