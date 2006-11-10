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

#include "MemoryFile.h"
#include <signal.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void signalHandler(int s) 
{
   s = s;
   /* A fprintf... probably not a good idea usually but we know where
    * the memory violation is so there is no issue in this case.
    */
   fprintf( stderr, "Failed in signal handler - all ok!\n" );
   
   exit(1);
}

int main()
{
   vdscMemoryFile  mem;
   vdscErrorHandler errorHandler;
   char *          pAddr = NULL;
   int errcode = 0, rc = 0;
#if ! defined(WIN32)
   struct sigaction newAction, oldAction;
#endif

   unlink( "MemFile.mem" );
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &mem, 10, "MemFile.mem" );

   errcode = vdscCreateBackstore( &mem, 0755, &errorHandler );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   errcode = vdscOpenMemFile( &mem, (void**)&pAddr, &errorHandler );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   errcode = vdscSetReadOnly( &mem, &errorHandler );
   if ( errcode != 0 ) 
      rc = -1;
   
   /* This should crash the whole thing. We intercept it with a signal
    * handler to make the output look cleaner.
    */
#if defined(WIN32)
   signal(SIGSEGV, signalHandler );
#else
   newAction.sa_handler = signalHandler;
   newAction.sa_flags   = SA_RESTART;
   errcode = sigaction( SIGSEGV, &newAction, &oldAction );
#endif

   pAddr[0] = 'x';
   pAddr[1] = 'y';

   vdscCloseMemFile( &mem, &errorHandler );

the_exit:
   unlink( "MemFile.mem" );
   
   vdscFiniMemoryFile( &mem );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return rc;
}

