/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@photonsoftware.org>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psocMemoryFile  mem;
   psocErrorHandler errorHandler;
   bool ok;
   
   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   psocInitMemoryFile( &mem, 2049, "MemFile.mem" );

   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   ok = psocCopyBackstore( &mem, 0755, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, 
         { unlink( "MemFile.mem" ); unlink( "MemFile.mem.bck" ); } );
   }

   ok = psocCopyBackstore( &mem, 0755, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, 
         { unlink( "MemFile.mem" ); unlink( "MemFile.mem.bck" ); } );
   }
   
   unlink( "MemFile.mem" );
//   unlink( "MemFile.mem" );
   psocFiniMemoryFile( &mem );

   psocInitMemoryFile( &mem, 9, "MemFile.mem" );
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, 
         { unlink( "MemFile.mem" ); unlink( "MemFile.mem.bck" ); } );
   }
   
   ok = psocCopyBackstore( &mem, 0755, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, 
         { unlink( "MemFile.mem" ); unlink( "MemFile.mem.bck" ); } );
   }
   
   unlink( "MemFile.mem" );
   unlink( "MemFile.mem.bck" );
   psocFiniMemoryFile( &mem );

   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

