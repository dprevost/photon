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
   bufsize_T currentAllocated;
   bufsize_T totalFree;
   bufsize_T maxFree;
   size_t    numberOfMallocs;
   size_t    numberOfFrees;
   vdscErrorHandler error;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &error );

   err = vdseMemAllocInit( &alloc, dum, dum, 10000, &error );
   if ( err != 0 )
      return -1;
   
   /* Allocate 2 dummy buffers */
   ptr = vdseMalloc( &alloc, 1000, &error );
   if ( ptr == NULL ) return -1;
   vdseFree( &alloc, ptr, &error );
   ptr = vdseMalloc( &alloc, 100, &error );
   if ( ptr == NULL ) return -1;
   ptr = vdseMalloc( &alloc, 217, &error );
   if ( ptr == NULL ) return -1;

   alloc.numMalloc = 25;
   alloc.totalAlloc = 1600;
   alloc.numFree = 178;
   
   vdseMemAllocResetStats( &alloc, &error );

   vdseMemAllocStats( &alloc,
                      &currentAllocated,
                      &totalFree,
                      &maxFree,
                      &numberOfMallocs,
                      &numberOfFrees,
                      &error );   

   if ( alloc.totalAlloc > 1000 )
      return -1;
   if ( alloc.numMalloc-alloc.numFree != 2 )
      return -1;
   if ( totalFree+currentAllocated != 10000 )
      return -1;
   
   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}

