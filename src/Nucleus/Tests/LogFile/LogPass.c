/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/LogFile.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonLogFile logfile;
   psocErrorHandler errorHandler;
   psoErrors error;

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );

   error = psonInitLogFile( &logfile, ".", (void*)0x1234, &errorHandler );
   if ( error != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   error = psonLogTransaction( &logfile, 1, &errorHandler );
   if ( error != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psonCloseLogFile( &logfile, &errorHandler );
   psocFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

