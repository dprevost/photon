/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

int main()
{
   psocErrorHandler errorHandler;
   int i;
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );   
   
   for ( i = 0; i < PSOC_ERROR_CHAIN_LENGTH; ++i ) {
      if ( errorHandler.errorCode[0]   != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      if ( errorHandler.errorHandle[0] != PSOC_NO_ERRHANDLER ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   if ( errorHandler.initialized != PSOC_ERROR_HANDLER_SIGNATURE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

