/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
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
   
   psocFiniErrorHandler( &errorHandler );
   
   for ( i = 0; i < PSOC_ERROR_CHAIN_LENGTH; ++i ) {
      if ( errorHandler.errorCode[0]   != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      if ( errorHandler.errorHandle[0] != PSOC_NO_ERRHANDLER ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   if ( errorHandler.initialized != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

