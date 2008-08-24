/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   psaProcess process;
   int errcode;
   psnSessionContext context;
      
   memset( &process, 0, sizeof(psaProcess) );
   if ( argc > 1 ) {
      errcode = psaProcessInit( &process, argv[1] );
   }
   else {
      errcode = psaProcessInit( &process, "10701" );
   }
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      return -1;
   }

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   pscInitErrorHandler( &context.errorHandler );
   
   errcode = psaOpenVDS( &process, "dummy", 100, &context );

   if ( errcode != PSO_BACKSTORE_FILE_MISSING ) {
      fprintf( stderr, "err: %d\n", errcode );
      return -1;
   }

   psaProcessFini();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
