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

#include "Nucleus/LogFile.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnLogFile logfile;
   pscErrorHandler errorHandler;
   vdsErrors error;

   pscInitErrorDefs();
   pscInitErrorHandler( &errorHandler );

   error = psnInitLogFile( &logfile, ".", (void*)0x1234, &errorHandler );
   if ( error != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   error = psnLogTransaction( &logfile, 1, &errorHandler );
   if ( error != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psnCloseLogFile( &logfile, &errorHandler );
   pscFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

