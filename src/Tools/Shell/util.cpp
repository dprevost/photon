/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Tools/Shell/util.h"
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "Common/Common.h"
#include "API/DataDefinition.h" // for psoaGetOffsets

using namespace pso;

/*
 * The read functions read from the buffer (obtained from shared memory)
 * an transform the "raw data" into something that can be displayed on the
 * console/terminal.
 *
 * The write functions do the inverse operation, they take the input from
 * the terminal and transform it into "raw data" for the buffer (that can
 * then be inserted in the shared memory).
 *
 * The buffer in both types of function is an array of bytes that repre-
 * sents the C structure matching the metadata of the object container.
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void readInt( string        & outStr,
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

void readUint( string        & outStr,
               size_t          intLength,
               unsigned char * buffer )
{
   ostringstream oss;
   
   uint8_t  ui8;
   uint16_t ui16;
   uint32_t ui32;
#if ! defined (WIN32)
   uint64_t ui64;
#endif

   switch( intLength ) {
   case 1:
      ui8 = buffer[0];
      oss << (unsigned int) ui8; // Not a single character, force it to be an int.
      break;
   case 2:
      ui16 = *( (uint16_t *)buffer);
      oss << ui16;
      break;
   case 4:
      ui32 = *( (uint32_t *)buffer);
      oss << ui32;
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
      ui32 = *( (uint32_t *)&buffer[4]);
      oss << ui32;
      ui32 = *( (uint32_t *)buffer);
      oss << ui32;
#else
      ui64 = *( (uint64_t *)buffer);
      oss << ui64;
#endif
      break;
   }

   outStr = oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void readBool( string        & outStr,
               unsigned char * buffer )
{
   if ( buffer[0] == 0 ) outStr = "False";
   else outStr= "True";
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void readDecimal( string        & outStr,
                  size_t          precision,
                  unsigned char * buffer )
{
   size_t i;
   
   for ( i = 0; i < precision+2; ++i )
      outStr += (char) buffer[i];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void readString( string        & outStr,
                 size_t          strLength,
                 unsigned char * buffer )
{
   size_t i;
   
   for ( i = 0; i < strLength; ++i )
      outStr += (char) buffer[i];
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void readBinary( string        & outStr,
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
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeInt( string        & inStr,
               size_t          intLength,
               unsigned char * buffer )
{
   long int i = strtol( inStr.c_str(), NULL, 0 );
   if ( errno == ERANGE ) return false;
   
   if ( intLength == 1 ) {
      if ( i > 127 ) return false;
      if ( i < -128 ) return false;
   }
   else if ( intLength == 2 ) {
      if ( i > 32767 ) return false;
      if ( i < -32768 ) return false;
   }
#  if SIZEOF_VOID_P > 4
   else if ( intLength == 4 ) {
      if ( i > (long int) 2147483647 ) return false;
      if ( i < (long int)-2147483648 ) return false;
   }
#endif

   memcpy( buffer, &i, intLength );
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeUint( string        & inStr,
                size_t          intLength,
                unsigned char * buffer )
{
   unsigned long i = strtoul( inStr.c_str(), NULL, 0 );
   if ( errno == ERANGE ) return false;
   
   if ( intLength == 1 ) {
      if ( i > 255 ) return false;
   }
   else if ( intLength == 2 ) {
      if ( i > 65535 ) return false;
   }
#  if SIZEOF_VOID_P > 4
   else if ( intLength == 4 ) {
      if ( i > 4294967295UL ) return false;
   }
#endif

   memcpy( buffer, &i, intLength );
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeBool( string        & inStr,
                unsigned char * buffer )
{
   bool b, ok = false; // ok == false indicates that no valid value were found
   
   for ( size_t i = 0; i < inStr.length(); ++i ) {
      inStr[i] = tolower(inStr[i]);
   }

   if ( inStr == "true" || inStr == "1" ) {
      ok = b = true;
   }
   if ( inStr == "false" || inStr == "0" ) {
      ok = true;
      b = false;
   }

   if ( ok ) {
      if ( b ) buffer[0] = 1;
      else buffer[0] = 0;
   }

   return ok;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeDecimal( string        & inStr,
                   size_t          precision,
                   size_t          scale,
                   unsigned char * buffer )
{
   size_t i, currentScale = 0;
   bool separator = false, sign = false;
   string tmp;
   
   // Check the content for invalid characters
   for ( i = 0; i < inStr.length(); ++i ) {
      if ( inStr[i] >= '0' && inStr[i] <= '9' ) continue;
      if ( inStr[i] == '.' ) {
         if ( separator ) return false; // Two separators!
         separator = true;
         currentScale = inStr.length() - i - 1;
         continue;
      }
      if ( i == 0 ) {
         if ( inStr[i] == '+' || inStr[i] == '-' ) {
            sign = true;
            continue;
         }
      }
      
      // Invalid/unknown character - getting out
      return false;
   }
   
   if ( currentScale > scale ) return false;

   
   if ( ! sign ) {
      tmp = inStr;
      inStr = "+";
      inStr += tmp;
   }
   
   if ( ! separator ) inStr += ".";
   for ( i = 0; i < scale - currentScale; ++i ) {
      // Pad the strings with '0', if needed
      inStr += "0";
   }

   if ( inStr.length() > precision + 2 ) return false;
   
   memcpy( buffer, inStr.c_str(), inStr.length() );

   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeFixString( string        & inStr,
                     size_t          strLength,
                     unsigned char * buffer )
{
   if ( inStr.length() > strLength ) return false;
   
   memcpy( buffer, inStr.c_str(), inStr.length() );
   
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeVarString( string        & inStr,
                     size_t          minLength,
                     size_t          maxLength,
                     unsigned char * buffer )
{
   if ( maxLength > 0 && inStr.length() > maxLength ) return false;
   if ( inStr.length() < minLength ) return false;
   
   memcpy( buffer, inStr.c_str(), inStr.length() );
   
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeFixBinary( string        & inStr,
                     size_t          binLength,
                     unsigned char * buffer )
{
   size_t length = inStr.length() / 2;
   size_t i, start = 0;
   unsigned char val1, val2;
   
   if ( (inStr.length()%2) != 0 ) return false;
   
   // the input of binary data  is hexadecimal - possibly preceded with
   // 0x or 0X
   if ( length > 0 ) {
      if ( inStr[0] == '0' && (inStr[1] == 'x' || inStr[1] == 'X') ) {
         length--;
         start = 1;
      }
   }
   
   if ( length > binLength ) return false;

   // Probably reinventing the wheel here, but it's faster to do it 
   // this way instead of looking up the proper C/C++ library call.
   for ( i = start; i < length+start; ++i ) {
      if ( inStr[2*i] >= '0' && inStr[2*i] <= '9' ) val1 = inStr[2*i] - '0';
      else val1 = inStr[2*i] - 'a' + 10;
      val1 *= 16;
      if ( inStr[2*i+1] >= '0' && inStr[2*i+1] <= '9' ) val2 = inStr[2*i+1] - '0';
      else val2 = inStr[2*i+1] - 'a' + 10;
      buffer[i-start] = val1 + val2;
      cerr << " bin2 i = " << i << ", val = " << hex << (int) buffer[i] << endl;
      
   }
   
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

bool writeVarBinary( string        & inStr,
                     size_t          minLength,
                     size_t          maxLength,
                     unsigned char * buffer )
{
   size_t length = inStr.length() / 2;
   size_t i, start = 0;
   unsigned char val1, val2;
   
   cerr << length << " - " << inStr.length() << endl;
   if ( (inStr.length()%2) != 0 ) return false;
   
   // the input of binary data  is hexadecimal - possibly preceded with
   // 0x or 0X
   if ( length > 0 ) {
      if ( inStr[0] == '0' && (inStr[1] == 'x' || inStr[1] == 'X') ) {
         length--;
         start = 1;
      }
   }
   
   cerr << length << " - " << inStr.length() << endl;
   cerr << minLength << " - " << maxLength << endl;
   if ( maxLength > 0 && length > maxLength ) return false;
   if ( length < minLength ) return false;

   // Probably reinventing the wheel here, but it's faster to do it 
   // this way instead of looking up the proper C/C++ library call.
   for ( i = start; i < length+start; ++i ) {
      if ( inStr[2*i] >= '0' && inStr[2*i] <= '9' ) val1 = inStr[2*i] - '0';
      else val1 = inStr[2*i] - 'a' + 10;
      val1 *= 16;
      if ( inStr[2*i+1] >= '0' && inStr[2*i+1] <= '9' ) val2 = inStr[2*i+1] - '0';
      else val2 = inStr[2*i+1] - 'a' + 10;
      buffer[i-start] = val1 + val2;
   }
   
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if 0

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

unsigned char * shellInToBuff( string        & inStr, 
                               ObjDefinition & definition,
                               uint32_t      & length )
{
   uint32_t * offsets = NULL, i;
   unsigned char * buffer = NULL;
   istringstream iss( inStr );
   vector<string> inData;
   string s;
   bool ok;

   psoObjectDefinition def = definition.GetDef();
   const psoFieldDefinition * fields = definition.GetFields();
   
   do {
      getline( iss, s, ',');
      inData.push_back( s );
   } while ( ! iss.eof() );

   if ( inData.size() != def.numFields ) throw(inData.size()+1);
   
   offsets = new uint32_t[def.numFields];
   psoaGetOffsets( &def, (psoFieldDefinition *)fields, offsets );
   length = offsets[def.numFields-1];
   
   // We partially analyse the last field to determine the length of the
   // buffer to allocate.
   i = def.numFields-1;
   switch( fields[i].type ) {

   case PSO_BINARY:
   case PSO_CHAR:
   case PSO_INTEGER:
      length += fields[i].length;
      break;

   case PSO_DECIMAL:
      length += fields[i].precision + 2;
      break;

   case PSO_TINYINT:
      length++;
      break;

   case PSO_VARCHAR:
      if ( inData[i].length() > fields[i].minLength ) {
         length += inData[i].length();
      }
      else {
         length += fields[i].minLength;
      }
      break;

   case PSO_VARBINARY:
      // the input of binary data  is hexadecimal - possibly preceded with
      // 0x or 0X
      size_t z = inData[i].length() / 2;
      if ( z > 0 ) {
         if ( inData[i][0] == '0' && (inData[i][1] == 'x' || inData[i][1] == 'X') ) z--;
      }
      if ( z > fields[i].minLength ) {
         length += z;
      }
      else {
         length += fields[i].minLength;
      }
      break;
   }
   
   buffer = new unsigned char[length];
   memset( buffer, 0, length );
   
   for ( i = 0; i < def.numFields; ++i ) {

      switch( fields[i].type ) {

      case PSO_INTEGER:
         ok = writeInt( inData[i],
                        fields[i].length,
                        &buffer[offsets[i]] );
         break;
      case PSO_BINARY:
         ok = writeFixBinary( inData[i],
                              fields[i].length,
                              &buffer[offsets[i]] );
         break;
      case PSO_CHAR:
         ok = writeFixString( inData[i],
                              fields[i].length,
                              &buffer[offsets[i]] );
         break;

      case PSO_DECIMAL:
         ok = writeDecimal( inData[i],
                            fields[i].precision,
                            fields[i].scale,
                            &buffer[offsets[i]] );
         break;
      case PSO_TINYINT:
         ok = writeBool( inData[i], &buffer[offsets[i]] );
         break;
      case PSO_VARCHAR:
         ok = writeVarString( inData[i],
                              fields[i].minLength,
                              fields[i].maxLength,
                              &buffer[offsets[i]] );
         break;
      case PSO_VARBINARY:
         ok = writeVarBinary( inData[i],
                              fields[i].minLength,
                              fields[i].maxLength,
                              &buffer[offsets[i]] );
         break;
      }
      if ( ! ok ) throw(i+1);
   }
   
   return buffer;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

unsigned char * shellInToKey( string        & inKey, 
                              ObjDefinition & definition,
                              uint32_t      & length )
{
   unsigned char * key = NULL;
   bool ok = false;

   psoObjectDefinition def = definition.GetDef();
   psoKeyFieldDefinition keyDef = definition.GetKey();
   
   // We need to determine the length of the key buffer to allocate.
   switch( keyDef.type ) {

   case PSO_KEY_BINARY:
   case PSO_KEY_STRING:
   case PSO_KEY_INTEGER:
      length += keyDef.length;
      break;

   case PSO_KEY_VAR_STRING:
      if ( inKey.length() > keyDef.minLength ) {
         length = inKey.length();
      }
      else {
         length = keyDef.minLength;
      }
      break;

   case PSO_KEY_VAR_BINARY:
      // the input of binary data  is hexadecimal - possibly preceded with
      // 0x or 0X
      size_t z = inKey.length() / 2;
      if ( z > 0 ) {
         if ( inKey[0] == '0' && (inKey[1] == 'x' || inKey[1] == 'X') ) z--;
      }
      if ( z > keyDef.minLength ) {
         length += z;
      }
      else {
         length += keyDef.minLength;
      }
      break;
   }
   
   key = new unsigned char[length];
   memset( key, 0, length );
   
   switch( keyDef.type ) {

   case PSO_KEY_INTEGER:
      ok = writeInt( inKey,
                     keyDef.length,
                     key );
      break;
   case PSO_KEY_BINARY:
      ok = writeFixBinary( inKey,
                           keyDef.length,
                           key );
      break;
   case PSO_KEY_STRING:
      ok = writeFixString( inKey,
                           keyDef.length,
                           key );
      break;
   case PSO_KEY_VAR_STRING:
      ok = writeVarString( inKey,
                           keyDef.minLength,
                           keyDef.maxLength,
                           key );
      break;
   case PSO_KEY_VAR_BINARY:
      ok = writeVarBinary( inKey,
                           keyDef.minLength,
                           keyDef.maxLength,
                           key );
      break;
   }

   if ( ! ok ) throw(0);
   
   return key;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

