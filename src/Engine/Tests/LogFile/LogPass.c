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

#include "Engine/LogFile.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseLogFile logfile;
   pscErrorHandler errorHandler;
   vdsErrors error;

   pscInitErrorDefs();
   pscInitErrorHandler( &errorHandler );

   error = vdseInitLogFile( &logfile, ".", (void*)0x1234, &errorHandler );
   if ( error != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   error = vdseLogTransaction( &logfile, 1, &errorHandler );
   if ( error != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   vdseCloseLogFile( &logfile, &errorHandler );
   pscFiniErrorDefs();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

