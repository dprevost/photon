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

int main( int argc, char* argv[] )
{
   unsigned char dum[10000];
   vdsErrors err;
   vdseMemAlloc alloc;
   void *ptr1, *ptr2, *ptr3;
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

   /* Allocate 3 dummy buffers and remove one */
   ptr1 = vdseMalloc( &alloc, 123, &error );
   if ( ptr1 == NULL ) return -1;
   ptr2 = vdseMalloc( &alloc, 2217, &error );
   if ( ptr2 == NULL ) return -1;
   ptr3 = vdseMalloc( &alloc, 217, &error );
   if ( ptr3 == NULL ) return -1;
   vdseFree( &alloc, ptr2, &error );
   
   vdseMemAllocStats( &alloc,
                      &currentAllocated,
                      &totalFree,
                      &maxFree,
                      &numberOfMallocs,
                      &numberOfFrees,
                      &error );

   if ( alloc.totalAlloc > 1000 )
      return -1;
   if ( alloc.numMalloc != 3 )
      return -1;
   if ( alloc.numFree != 1 )
      return -1;
   if ( totalFree+currentAllocated != 10000 )
      return -1;
   if ( maxFree == 0 )
      return -1;
   if ( maxFree > totalFree )
      return -1;
   
   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}

