/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#if defined WIN32
#  pragma warning(disable:4273)
#endif
#include "API/Process.c"
#include "API/Connector.c"
#if defined WIN32
#  pragma warning(default:4273)
#endif

#include "Tests/PrintError.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
#if defined(USE_DBC)
   psoaProcess process;
   int errcode;
   psonSessionContext context;
      
   memset( &process, 0, sizeof(psoaProcess) );
   if ( argc > 1 ) {
      errcode = psoaProcessInit( &process, argv[1] );
   }
   else {
      errcode = psoaProcessInit( &process, "10701" );
   }
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      return 0;
   }

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   errcode = psoaOpenMemory( &process, "dummy", 100, NULL );

   /* Should not return */
   ERROR_EXIT( expectedToPass, NULL, ; );
#else
#  if defined(WIN32)
   exit(3);
#  else
   abort();
#  endif
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
