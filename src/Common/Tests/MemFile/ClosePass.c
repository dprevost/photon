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
#include "PrintError.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdscMemoryFile  mem;
   vdscErrorHandler errorHandler;
   void*           pAddr = NULL;
   int errcode = 0, rc = 0;
   
   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &mem, 10, "MemFile.mem" );

   errcode = vdscCreateBackstore( &mem, 0755, &errorHandler );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   errcode = vdscOpenMemFile( &mem, &pAddr, &errorHandler );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   vdscCloseMemFile( &mem, &errorHandler );
   
   if ( mem.fileHandle != VDS_INVALID_HANDLE ) rc = -1;
   if ( mem.baseAddr   != VDS_MAP_FAILED ) rc = -1;

#if defined (WIN32)
   if ( mem.mapHandle != VDS_INVALID_HANDLE ) rc = -1;
#endif

the_exit:

   unlink( "MemFile.mem" );
   
   vdscFiniMemoryFile( &mem );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return rc;
}

