/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "API/Process.h"
/*
 * On Windows, we either must include Connector.c or... we must export 
 * the function. Exporting the function for a single test? Not worth it.
 */
#if defined WIN32
#  pragma warning(disable:4273)
#  include "API/Connector.c"
#  pragma warning(default:4273)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   psoaProcess process;
   int errcode;
   psonSessionContext context;
   bool ok;
   
   ok = psocInitThreadLock( &g_ProcessMutex );
   if ( ! ok ) {
      return -1;
   }
   
   memset( &process, 0, sizeof(psoaProcess) );
   if ( argc > 1 ) {
      errcode = psoaProcessInit( &process, argv[1], argv[0] );
   }
   else {
      errcode = psoaProcessInit( &process, "10701", argv[0] );
   }
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      return -1;
   }

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   psocInitErrorHandler( &context.errorHandler );
   
   psoaCloseMemory( &process, &context );

   /* Cannot call psoaProcessFini since it calls CloseMemory()  */
   psoaDisconnect( &process.connector, &context.errorHandler );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
