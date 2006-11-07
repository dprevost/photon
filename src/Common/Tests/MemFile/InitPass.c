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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   vdscMemoryFile mem;
   
   vdscInitMemoryFile( &mem, 10, "MemFile.mem" );

   if ( mem.initialized != VDSC_MEMFILE_SIGNATURE )
      return -1;

   if ( strcmp( mem.name, "MemFile.mem" ) != 0 )
      return -1;
   
   if ( mem.length != 1024*10 )
      return -1;
   
   if ( mem.baseAddr != VDS_MAP_FAILED )
      return -1;
   
   if ( mem.fileHandle != VDS_INVALID_HANDLE )
      return -1;

#if defined (WIN32)
   if ( mem.mapHandle != VDS_INVALID_HANDLE )
      return -1;
#endif

   vdscFiniMemoryFile( &mem );

   return 0;
}
