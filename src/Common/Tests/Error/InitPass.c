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

int main()
{
   vdscErrorHandler errorHandler;
   int i;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );   
   
   for ( i = 0; i < VDSC_ERROR_CHAIN_LENGTH; ++i ) {
      if ( errorHandler.errorCode[0]   != 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
      if ( errorHandler.errorHandle[0] != VDSC_NO_ERRHANDLER )
         ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.initialized != VDSC_ERROR_HANDLER_SIGNATURE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

