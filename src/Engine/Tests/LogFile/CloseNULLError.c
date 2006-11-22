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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "LogFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   vdseLogFile logfile;
   vdscErrorHandler errorHandler;
   vdsErrors error;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );

   error = vdseInitLogFile( &logfile, ".", (void*)0x1234, &errorHandler );
   if ( error != VDS_OK )
      return 0;
   
   vdseCloseLogFile( &logfile, NULL );

   vdscFiniErrorDefs();
   
   return 0;
}
