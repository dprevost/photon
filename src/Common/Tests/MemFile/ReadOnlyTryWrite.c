/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/MemoryFile.h"
#include <signal.h>
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void signalHandler(int s) 
{
   s = s;
   /* A fprintf... probably not a good idea usually but we know where
    * the memory violation is so there is no issue in this case.
    */
   fprintf( stderr, "Signal handler was called - all ok!\n" );

   unlink( "MemFile.mem" );
   
   exit(0);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   pscMemoryFile  mem;
   pscErrorHandler errorHandler;
   void *          pAddr = NULL;
   bool ok;
#if ! defined(WIN32)
   struct sigaction newAction, oldAction;
#endif

   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );
   
   pscInitErrorDefs();
   pscInitErrorHandler( &errorHandler );
   pscInitMemoryFile( &mem, 10, "MemFile.mem" );

   ok = pscCreateBackstore( &mem, 0755, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   ok = pscOpenMemFile( &mem, &pAddr, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   ok = pscSetReadOnly( &mem, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   /* This should crash the whole thing. We intercept it with a signal.
    * This way, if there is no memory violation, we will know.
    */
#if defined(WIN32)
   signal(SIGSEGV, signalHandler );
#else
   newAction.sa_handler = signalHandler;
   newAction.sa_flags   = SA_RESTART;
   sigaction( SIGSEGV, &newAction, &oldAction );
#endif

   ((char*)pAddr)[0] = 'x';
   ((char*)pAddr)[1] = 'y';

   ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

