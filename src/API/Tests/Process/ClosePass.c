/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   vdsaProcess process;
   int errcode;
   vdseSessionContext context;
   
   memset( &process, 0, sizeof(vdsaProcess) );
   if ( argc > 1 )
      errcode = vdsaProcessInit( &process, argv[1] );
   else
      errcode = vdsaProcessInit( &process, "10701" );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "err: %d\n", errcode );
      return -1;
   }

   memset( &context, 0, sizeof context );
   context.pidLocker= getpid();
   vdscInitErrorHandler( &context.errorHandler );
   
   vdsaCloseVDS( &process, &context );

   /* Cannot call vdsaProcessFini since it calls CloseVDS()  */
   vdsaDisconnect( &process.connector, &context.errorHandler );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
