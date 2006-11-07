/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#include "ErrorHandler.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int errorHandler( int errorCode, char* msg, unsigned int msgLength )
{
   strncpy( msg, "Dummy Handler", strlen("Dummy Handler") );

   return 0;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char* argv[] )
{
   vdscErrMsgHandle handle1, handle2;
   vdscErrorHandler error;
   char msg[100] = "";
   size_t len;

   vdscInitErrorDefs();

   handle1 = vdscAddErrorMsgHandler( "Dummy", &errorHandler );
   fprintf(stderr, "handle1 = %d %p\n", handle1, &errorHandler );
   handle2 = vdscAddErrorMsgHandler( "Dummy2", &errorHandler );
   fprintf(stderr, "handle2 = %d\n", handle2 );
   
   vdscInitErrorHandler( &error );
   vdscSetError( &error, handle2, 7 );

   len = vdscGetErrorMsg( &error, msg, 100 );
/*    if ( strcmp( msg, "Dummy Handler" ) != 0 ) */
/*       return 1; */
   fprintf( stderr, "len = %d, msg = %s\n", len, msg );
   
   vdscFiniErrorDefs();
   
   return 0;
}

