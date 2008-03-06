/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

#include "Common/Common.h"
#include <vdsf/vdsException>
#include <vdsf/vdsSession.h>

using namespace std;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsException::vdsException( int          theErrorCode,
                            VDS_HANDLE   sessionHandle, 
                            const char * functionName )
   : errcode( theErrorCode )
{
   char s[1024];
   int rc = 0;
   
   if ( sessionHandle != NULL ) {
      rc = vdsErrorMsg( sessionHandle, s, 1024 );
      msg = functionName;
      msg += " exception: ";
      msg += s;
   }
   
   if ( rc != 0 || sessionHandle == NULL ) {
      // We build our own message
      sprintf(s, "%s exception: %s%d",
         functionName,
         "Cannot retrieve the error message - the error code is ",
         theErrorCode );
      msg = s;
   }
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdsException::~vdsException()
{
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & vdsException::Message( string & errorMessage )
{
   errorMessage = msg;
   return errorMessage;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

string & vdsException::Message()
{
   return msg;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
