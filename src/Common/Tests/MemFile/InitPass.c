/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Common/MemoryFile.h"
#include "Tests/PrintError.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psocMemoryFile mem;
   
   psocInitMemoryFile( &mem, 10, "MemFile.mem" );

   if ( mem.initialized != PSOC_MEMFILE_SIGNATURE ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   
   if ( strcmp( mem.name, "MemFile.mem" ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   
   if ( mem.length != 1024*10 ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   
   if ( mem.baseAddr != PSO_MAP_FAILED ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   
   if ( mem.fileHandle != PSO_INVALID_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   
#if defined (WIN32)
   if ( mem.mapHandle != PSO_INVALID_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
#endif

   psocFiniMemoryFile( &mem );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

