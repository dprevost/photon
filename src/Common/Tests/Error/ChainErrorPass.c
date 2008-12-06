/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   
   psocSetError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );

   psocChainError( &errorHandler, PSOC_ERRNO_HANDLE, EINTR );

   if ( errorHandler.chainLength != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorCode[0] != ENOENT ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorCode[1] != EINTR ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorHandle[0] != PSOC_ERRNO_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorHandle[1] != PSOC_ERRNO_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

