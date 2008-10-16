/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include <sstream>
#include <iostream>

#include "Common/Common.h"
#include <photon/psoException>
#include <photon/psoSession.h>
#include <Nucleus/psoErrorHandler.h>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

pso::Exception::Exception( PSO_HANDLE   sessionHandle, 
                           const char * functionName )
   : errcode( 0 )
{
   char s[1024];
   int rc = 1;
   ostringstream oss;
   
   rc = psoErrorMsg( sessionHandle, s, 1024 );
   errcode = psoLastError( sessionHandle );
   msg = functionName;
   msg += " exception: ";
   msg += s;
   
   if ( rc != 0 ) {
      // We build our own message
      oss << functionName << " exception: ";
      oss << "Cannot retrieve the error message ";
      msg = oss.str();
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

pso::Exception::Exception( const char * functionName,
                           int          theErrorCode )
   : errcode( theErrorCode )
{
   const char * str;
   ostringstream oss;
   
   str = pson_ErrorMessage( theErrorCode );
   if ( str != NULL ) {
      oss << functionName << " exception: " << str;
   }
   else {
      // We build our own message
      oss << functionName << " exception: ";
      oss << "Cannot retrieve the error message - the error code is " << theErrorCode;
   }
   msg = oss.str();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

pso::Exception::~Exception()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & pso::Exception::Message( string & errorMessage )
{
   errorMessage = msg;
   return errorMessage;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & pso::Exception::Message()
{
   return msg;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

