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

#include "MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* ResetStats loop on all buffers to calculate the real stats,
 * the current number of allocated and free buffers, the 
 * allocated size, etc. So to test it we need to properly setup
 * the allocator.
 */

int main( int argc, char* argv[] )
{
   unsigned char dum[10000];
   vdsErrors err;
   vdseMemAlloc alloc;
   void* ptr;
   vdscErrorHandler error;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &error );
   
   err = vdseMemAllocInit( &alloc, dum, dum, 10000, &error );
   if ( err != 0 )
      return 0;   
   
   /* Allocate 2 dummy buffers */
   ptr = vdseMalloc( &alloc, 100, &error );
   if ( ptr == NULL ) return 0;
   ptr = vdseMalloc( &alloc, 217, &error );
   if ( ptr == NULL ) return 0;

   vdseMemAllocResetStats( NULL, &error );
   
   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}

