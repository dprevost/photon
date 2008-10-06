/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
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

void readInt( string        & inStr,
              size_t          intLength,
              unsigned char * buffer )
{
   ostringstream oss;
   
   int8_t  i8;
   int16_t i16;
   int32_t i32;
   int64_t i64;
   
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
      i64 = *( (int64_t *)buffer);
      oss << i64;
      break;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void readUint( string        & inStr,
               size_t          intLength,
               unsigned char * buffer )
{
   ostringstream oss;
   
   uint8_t  ui8;
   uint16_t ui16;
   uint32_t ui32;
   uint64_t ui64;
   
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
      ui64 = *( (uint64_t *)buffer);
      oss << ui64;
      break;
   }
}

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
   size_t i;
   unsigned char val1, val2;
   
   if ( (inStr.length()%2) != 0 ) return false;
   
   // the input of binary data  is hexadecimal - possibly preceded with
   // 0x or 0X
   if ( length > 0 ) {
      if ( inStr[0] == '0' && (inStr[1] == 'x' || inStr[1] == 'X') ) length--;
   }
   
   if ( length > binLength ) return false;

   // Probably reinventing the wheel here, but it's faster to do it 
   // this way instead of looking up the proper C/C++ library call.
   for ( i = 0; i < length; ++i ) {
      if ( inStr[2*i] >= '0' && inStr[2*i] <= '9' ) val1 = inStr[2*i] - '0';
      else val1 = inStr[2*i] - 'a' + 10;
      val1 *= 16;
      if ( inStr[2*i+1] >= '0' && inStr[2*i+1] <= '9' ) val2 = inStr[2*i+1] - '0';
      else val2 = inStr[2*i+1] - 'a' + 10;
      buffer[i] = val1 + val2;
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
   size_t i;
   unsigned char val1, val2;
   
   cerr << length << " - " << inStr.length() << endl;
   if ( (inStr.length()%2) != 0 ) return false;
   
   // the input of binary data  is hexadecimal - possibly preceded with
   // 0x or 0X
   if ( length > 0 ) {
      if ( inStr[0] == '0' && (inStr[1] == 'x' || inStr[1] == 'X') ) length--;
   }
   
   cerr << length << " - " << inStr.length() << endl;
   cerr << minLength << " - " << maxLength << endl;
   if ( maxLength > 0 && length > maxLength ) return false;
   if ( length < minLength ) return false;

   // Probably reinventing the wheel here, but it's faster to do it 
   // this way instead of looking up the proper C/C++ library call.
   for ( i = 0; i < length; ++i ) {
      if ( inStr[2*i] >= '0' && inStr[2*i] <= '9' ) val1 = inStr[2*i] - '0';
      else val1 = inStr[2*i] - 'a' + 10;
      val1 *= 16;
      if ( inStr[2*i+1] >= '0' && inStr[2*i+1] <= '9' ) val2 = inStr[2*i+1] - '0';
      else val2 = inStr[2*i+1] - 'a' + 10;
      buffer[i] = val1 + val2;
   }
   
   return true;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

unsigned char * shellInToBuff( string              & inStr, 
                               psoObjectDefinition * pDefinition,
                               size_t              & length )
{
   size_t * offsets = NULL, i;
   unsigned char * buffer = NULL;
   istringstream iss( inStr );
   vector<string> inData;
   string s;
   bool ok;
   
   do {
      getline( iss, s, ',');
      inData.push_back( s );
   } while ( ! iss.eof() );

   if ( inData.size() != pDefinition->numFields ) throw(inData.size()+1);
   
   offsets = new size_t[pDefinition->numFields];
   psoaGetOffsets( pDefinition, offsets );
   length = offsets[pDefinition->numFields-1];
   
   // We partially analyse the last field to determine the length of the
   // buffer to allocate.
   i = pDefinition->numFields-1;
   switch( pDefinition->fields[i].type ) {

   case PSO_BINARY:
   case PSO_STRING:
   case PSO_INTEGER:
      length += pDefinition->fields[i].length;
      break;

   case PSO_DECIMAL:
      length += pDefinition->fields[i].precision + 2;
      break;

   case PSO_BOOLEAN:
      length++;
      break;

   case PSO_VAR_STRING:
      if ( inData[i].length() > pDefinition->fields[i].minLength ) {
         length += inData[i].length();
      }
      else {
         length += pDefinition->fields[i].minLength;
      }
      break;

   case PSO_VAR_BINARY:
      // the input of binary data  is hexadecimal - possibly preceded with
      // 0x or 0X
      size_t z = inData[i].length() / 2;
      if ( z > 0 ) {
         if ( inData[i][0] == '0' && (inData[i][1] == 'x' || inData[i][1] == 'X') ) z--;
      }
      if ( z > pDefinition->fields[i].minLength ) {
         length += z;
      }
      else {
         length += pDefinition->fields[i].minLength;
      }
      break;
   }
   
   buffer = new unsigned char[length];
   memset( buffer, 0, length );
   
   for ( i = 0; i < pDefinition->numFields; ++i ) {

      switch( pDefinition->fields[i].type ) {

      case PSO_INTEGER:
         ok = writeInt( inData[i],
                        pDefinition->fields[i].length,
                        &buffer[offsets[i]] );
         break;
      case PSO_BINARY:
         ok = writeFixBinary( inData[i],
                              pDefinition->fields[i].length,
                              &buffer[offsets[i]] );
         break;
      case PSO_STRING:
         ok = writeFixString( inData[i],
                              pDefinition->fields[i].length,
                              &buffer[offsets[i]] );
         break;

      case PSO_DECIMAL:
         ok = writeDecimal( inData[i],
                            pDefinition->fields[i].precision,
                            pDefinition->fields[i].scale,
                            &buffer[offsets[i]] );
         break;
      case PSO_BOOLEAN:
         ok = writeBool( inData[i], &buffer[offsets[i]] );
         break;
      case PSO_VAR_STRING:
         ok = writeVarString( inData[i],
                              pDefinition->fields[i].minLength,
                              pDefinition->fields[i].maxLength,
                              &buffer[offsets[i]] );
         break;
      case PSO_VAR_BINARY:
         ok = writeVarBinary( inData[i],
                              pDefinition->fields[i].minLength,
                              pDefinition->fields[i].maxLength,
                              &buffer[offsets[i]] );
         break;
      }
      if ( ! ok ) throw(i+1);
   }
   
   return buffer;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

unsigned char * shellInToKey( string           & inKey, 
                              psoKeyDefinition * pDefinition,
                              size_t           & length )
{
   unsigned char * key = NULL;
   bool ok = false;
   
   // We need to determine the length of the key buffer to allocate.
   switch( pDefinition->type ) {

   case PSO_KEY_BINARY:
   case PSO_KEY_STRING:
   case PSO_KEY_INTEGER:
      length += pDefinition->length;
      break;

   case PSO_KEY_VAR_STRING:
      if ( inKey.length() > pDefinition->minLength ) {
         length = inKey.length();
      }
      else {
         length = pDefinition->minLength;
      }
      break;

   case PSO_KEY_VAR_BINARY:
      // the input of binary data  is hexadecimal - possibly preceded with
      // 0x or 0X
      size_t z = inKey.length() / 2;
      if ( z > 0 ) {
         if ( inKey[0] == '0' && (inKey[1] == 'x' || inKey[1] == 'X') ) z--;
      }
      if ( z > pDefinition->minLength ) {
         length += z;
      }
      else {
         length += pDefinition->minLength;
      }
      break;
   }
   
   key = new unsigned char[length];
   memset( key, 0, length );
   
   switch( pDefinition->type ) {

   case PSO_KEY_INTEGER:
      ok = writeInt( inKey,
                     pDefinition->length,
                     key );
      break;
   case PSO_KEY_BINARY:
      ok = writeFixBinary( inKey,
                           pDefinition->length,
                           key );
      break;
   case PSO_KEY_STRING:
      ok = writeFixString( inKey,
                           pDefinition->length,
                           key );
      break;
   case PSO_KEY_VAR_STRING:
      ok = writeVarString( inKey,
                           pDefinition->minLength,
                           pDefinition->maxLength,
                           key );
      break;
   case PSO_KEY_VAR_BINARY:
      ok = writeVarBinary( inKey,
                           pDefinition->minLength,
                           pDefinition->maxLength,
                           key );
      break;
   }

   if ( ! ok ) throw(0);
   
   return key;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

