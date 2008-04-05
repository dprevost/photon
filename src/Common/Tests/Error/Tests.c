/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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

int msgErrorHandler( int errorCode, char* msg, unsigned int msgLength )
{
   errorCode = errorCode;
   msgLength = msgLength;

   strncpy( msg, "Dummy Handler", strlen("Dummy Handler") );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdscErrorHandler errorHandler;
   char msg[100];
   vdscErrMsgHandle handle1, handle2;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
  
   handle1 = vdscAddErrorMsgHandler( "Dummy1", &msgErrorHandler );
   if ( handle1 == VDSC_NO_ERRHANDLER ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   handle2 = vdscAddErrorMsgHandler( "Dummy2", &msgErrorHandler );
   if ( handle2 == VDSC_NO_ERRHANDLER ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdscSetError( &errorHandler, handle2, 7 );

   vdscChainError( &errorHandler, handle1, 17 );

   /* This should work, although that would leave a single byte
    * to use for the error message...
    */
   vdscGetErrorMsg( &errorHandler, msg, 81 );

   vdscSetError( &errorHandler, VDSC_ERRNO_HANDLE, ENOENT );

   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

