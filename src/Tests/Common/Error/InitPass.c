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

int main( int argc, char* argv[] )
{
   vdscErrorHandler errorHandler;
   int i;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );   
   
   for ( i = 0; i < VDSC_ERROR_CHAIN_LENGTH; ++i )
   {
      if ( errorHandler.errorCode[0]   != 0 ) return -1;
      if ( errorHandler.errorHandle[0] != VDSC_NO_ERRHANDLER ) return -1;
   }
   if ( errorHandler.initialized != VDSC_ERROR_HANDLER_SIGNATURE ) return -1;
   
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

