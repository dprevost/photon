/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ErrorHandler.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int errorHandler( int errorCode, char* msg, unsigned int msgLength )
{
   errorCode = errorCode;
   msgLength = msgLength;

   strncpy( msg, "Dummy Handler", strlen("Dummy Handler") );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   pscErrMsgHandle handle1, handle2;
   pscErrorHandler error;
   char msg[100] = "";
   size_t len;

   pscInitErrorDefs();

   handle1 = pscAddErrorMsgHandler( "Dummy", &errorHandler );
   if ( handle1 == PSC_NO_ERRHANDLER ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   handle2 = pscAddErrorMsgHandler( "Dummy2", &errorHandler );
   if ( handle2 == PSC_NO_ERRHANDLER ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pscInitErrorHandler( &error );
   pscSetError( &error, handle2, 7 );

   len = pscGetErrorMsg( &error, msg, 100 );
   if ( len == 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pscFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

