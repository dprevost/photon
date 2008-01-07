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
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   
   vdscSetError( &errorHandler, VDSC_ERRNO_HANDLE, ENOENT );

   vdscChainError( &errorHandler, VDSC_ERRNO_HANDLE, EINTR );

   if ( errorHandler.chainLength != 2 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( errorHandler.errorCode[0] != ENOENT )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( errorHandler.errorCode[1] != EINTR )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( errorHandler.errorHandle[0] != VDSC_ERRNO_HANDLE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( errorHandler.errorHandle[1] != VDSC_ERRNO_HANDLE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

