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

#include "Common/MemoryFile.h"
#include "Tests/PrintError.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psocMemoryFile  mem;
   psocErrorHandler errorHandler;
   bool ok;
   psocMemoryFileStatus status;
   
   /* The rename is a work around for a bug on Windows. It seems that the 
    * delete call (unlink) is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   psocInitMemoryFile( &mem, 10, "MemFile.mem" );

   ok = psocCreateBackstore( &mem, 0644, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   mem.initialized = 0;
   psocBackStoreStatus( &mem, &status );

   ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

