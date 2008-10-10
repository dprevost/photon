/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
   psocErrMsgHandle handle1, handle2;
   psocErrorHandler error;
   char msg[100] = "";
   size_t len;

   psocInitErrorDefs();

   handle1 = psocAddErrorMsgHandler( "Dummy", &errorHandler );
   if ( handle1 == PSOC_NO_ERRHANDLER ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   handle2 = psocAddErrorMsgHandler( "Dummy2", &errorHandler );
   if ( handle2 == PSOC_NO_ERRHANDLER ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocInitErrorHandler( &error );
   psocSetError( &error, handle2, 7 );

   len = psocGetErrorMsg( &error, msg, 100 );
   if ( len == 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

