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

int main()
{
   pscErrorHandler errorHandler;
   
   pscInitErrorDefs();
   pscInitErrorHandler( &errorHandler );
   
   pscSetError( &errorHandler, PSC_ERRNO_HANDLE, ENOENT );

   pscChainError( &errorHandler, PSC_ERRNO_HANDLE, EINTR );

   if ( errorHandler.chainLength != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorCode[0] != ENOENT ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorCode[1] != EINTR ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorHandle[0] != PSC_ERRNO_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errorHandler.errorHandle[1] != PSC_ERRNO_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   pscFiniErrorHandler( &errorHandler );
   pscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

