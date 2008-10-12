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

psoException::psoException( int          theErrorCode,
                            PSO_HANDLE   sessionHandle, 
                            const char * functionName )
   : errcode( theErrorCode )
{
   char s[1024];
   int rc = 1;
   const char * str;
   ostringstream oss;
   
   if ( sessionHandle != NULL &&
      psoLastError(sessionHandle) == theErrorCode ) {
      rc = psoErrorMsg( sessionHandle, s, 1024 );
      msg = functionName;
      msg += " exception: ";
      msg += s;
   }
   
   if ( rc != 0 ) {
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
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

psoException::~psoException()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & psoException::Message( string & errorMessage )
{
   errorMessage = msg;
   return errorMessage;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & psoException::Message()
{
   return msg;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

