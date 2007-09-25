/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "API/Process.c"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdsaProcess process;
   int errcode;
   vdseSessionContext context;
      
   errcode = vdsaProcessInit( &process, "12345" );
   if ( errcode != VDS_CONNECT_ERROR )
   {
      fprintf( stderr, "err: %d\n", errcode );
      return -1;
   }

   memset( &context, 0, sizeof context );
   context.lockValue = getpid();
   vdscInitErrorHandler( &context.errorHandler );
   
   errcode = vdsaOpenVDS( &process, "dummy", 100, &context );

   if ( errcode != VDS_BACKSTORE_FILE_MISSING )
   {
      fprintf( stderr, "err: %d\n", errcode );
      return -1;
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */