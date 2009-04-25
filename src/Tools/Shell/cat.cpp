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
#include "API/KeyDefinition.h"  // for psoaGet*Offsets
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
     keyDefLength  ( 0 ),
     pDataDef      ( NULL ),
     pKeyDef       ( NULL )
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
      session.GetDefinition( objectName, objDefinition );
      pDataDef = session.GetDataDefinition( objectName );
      // Test for NULLs
      if ( status.type == PSO_FAST_MAP || status.type == PSO_HASH_MAP ) {
         pKeyDef = session.GetKeyDefinition( objectName );
         // Test for NULLs
         keyDefLength = pKeyDef->GetLength();
      }
      fieldDefLength = pDataDef->GetLength();

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
   
      pDataDef->GetDefinition( fieldDef, fieldDefLength );
      if ( pKeyDef != NULL ) {
         pKeyDef->GetDefinition( keyDef, keyDefLength );
      }
      if ( pDataDef->GetType() == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
         numFields = fieldDefLength/sizeof(psoFieldDefinition);
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
      
      if ( pKeyDef != NULL && pKeyDef->GetType() == PSO_DEF_PHOTON_ODBC_SIMPLE ) {
         numKeys = keyDefLength/sizeof(psoKeyFieldDefinition);
         try {
            keyOffsets = new uint32_t[numKeys];
         }
         catch ( ... ) {
            stream << "psosh: cat: Not enough memory " << endl;
            stream << "Number of keys in data definition = " << keyDefLength/sizeof(psoKeyFieldDefinition);
            return stream.str();
         }
         psoaGetKeyOffsets( (psoKeyFieldDefinition *)keyDef, numKeys, keyOffsets );
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
   }
   catch ( Exception exc ) {
      stream << "psosh: cat: " << exc.Message();
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
      HashMap hashMap( session, objectName );
      
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
      FastMap hashMap( session, objectName );
      
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
      Queue queue( session, objectName );
      
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
      Lifo queue( session, objectName );
      
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
         s = readInt8( &buffer[fieldOffsets[i]] );
         break;

      case PSO_SMALLINT:
         s = readInt16( &buffer[fieldOffsets[i]] );
         break;

      case PSO_INTEGER:
         s = readInt32( &buffer[fieldOffsets[i]] );
         break;

      case PSO_BIGINT:
         s = readInt64( &buffer[fieldOffsets[i]] );
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
         s = readDecimal( fieldODBC[i].vals.decimal.precision,
                          fieldODBC[i].vals.decimal.scale,
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
         s = readFloat32( &buffer[fieldOffsets[i]] );
         break;

      case PSO_DOUBLE:
         s = readFloat64( &buffer[fieldOffsets[i]] );
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
   psoKeyFieldDefinition * keyODBC;
   int numKeys, i;
   string outStr;
   
   keyODBC = (psoKeyFieldDefinition *) keyDef;
   numKeys = keyDefLength / sizeof(psoKeyFieldDefinition);
   
   psoaGetKeyOffsets( keyODBC, numKeys, keyOffsets );

   for ( i = 0; i < numKeys; ++i ) {
      string s;
      
      switch( keyODBC[i].type ) {

      case PSO_KEY_INTEGER:
         s = readInt32( &key[keyOffsets[i]] );
         break;

      case PSO_KEY_BIGINT:
         s = readInt64( &key[keyOffsets[i]] );
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

string psoCat::readDate( unsigned char * buffer )
{
   ostringstream oss;
   psoDateStruct * pDate = (psoDateStruct *) buffer;

   oss.width(4);
   oss << pDate->year << "-";
   oss.width(2);
   oss << pDate->month << "-" << pDate->day;
   
   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readDecimal( int precision, int scale, unsigned char * buffer )
{
   int i;
   ostringstream oss;
   psoNumericStruct * pNumeric = (psoNumericStruct *) buffer;
   uint64_t value = 0, shift = 1;
   int8_t  i8low, i8high;
   
   if ( pNumeric->sign != 1 ) oss << "-";
   
   for ( i = PSO_MAX_NUMERIC_LEN-1; i >= 0; i-- ) {
      i8low = buffer[i] / 16;
      i8high = buffer[i] % 16;
      value = value * 256 + i8low + i8high * 16;
   }
   
   if ( scale == 0 ) {
      oss << value;
   }
   else if ( scale < 0 ) {
      oss << value;
      for ( i = 0; i < scale; --i ) oss << "0";
   }
   else {
      for ( i = 0; i < scale; ++i ) {
         shift *= 10;
      }
      oss << value / shift << "." << value % shift;
   }

   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readFloat32( unsigned char * buffer )
{
   ostringstream oss;
   psoFloat32 f32;
   
   f32 = *( (psoFloat32 *)buffer);
   oss << f32;
   
   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readFloat64( unsigned char * buffer )
{
   ostringstream oss;
   psoFloat64 f64;
   
   f64 = *( (psoFloat64 *)buffer);
   oss << f64;
   
   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readInt8( unsigned char * buffer )
{
   ostringstream oss;
   int8_t i8;
   
   i8 = buffer[0];
   oss << (int) i8; // Not a single character, force it to be an int.

   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readInt16( unsigned char * buffer )
{
   ostringstream oss;
   int16_t i16;
   
   i16 = *( (int16_t *)buffer);
   oss << i16;

   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readInt32( unsigned char * buffer )
{
   ostringstream oss;
   int32_t i32;
   
   i32 = *( (int32_t *)buffer);
   oss << i32;

   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readInt64( unsigned char * buffer )
{
   ostringstream oss;
#if ! defined (WIN32)
   int64_t i64;
#else
   int32_t i32;
#endif

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
   
   return oss.str();
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

string psoCat::readTime( unsigned char * buffer )
{
   ostringstream oss;
   psoTimeStruct * pTime = (psoTimeStruct *) buffer;

   oss.width(4);
   oss << pTime->hour << ":";
   oss.width(2);
   oss << pTime->minute << ":" << pTime->second;
   
   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string psoCat::readTimeStamp( unsigned char * buffer )
{
   ostringstream oss;
   psoTimeStampStruct * pStamp = (psoTimeStampStruct *) buffer;

   oss.width(4);
   oss << pStamp->year << "-";
   oss.width(2);
   oss << pStamp->month << ":" << pStamp->day << " ";

   oss.width(4);
   oss << pStamp->hour << ":";
   oss.width(2);
   oss << pStamp->minute << ":" << pStamp->second << ".";

   // Showing the fraction in millisecs. Not sure this is ok.
   oss.width(3);
   oss << pStamp->fraction/1000000;

   return oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

