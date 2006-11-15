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
      return 0;

   /* Allocate 3 dummy buffers */
   ptr1 = vdseMalloc( &alloc, 100, &error );
   if ( ptr1 == NULL ) return 0;
   ptr2 = vdseMalloc( &alloc, 217, &error );
   if ( ptr2 == NULL ) return 0;
   ptr3 = vdseMalloc( &alloc, 217, &error );
   if ( ptr3 == NULL ) return 0;
   vdseFree( &alloc, ptr2, &error );
   
   vdseMemAllocStats( &alloc,
                      &currentAllocated,
                      &totalFree,
                      &maxFree,
                      &numberOfMallocs,
                      NULL,
                      &error );   
   
   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}

