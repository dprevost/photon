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
   void* ptr;
   vdseMemAlloc alloc;
   vdscErrorHandler error;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &error );
   
   alloc.totalAlloc    = 0;
   alloc.numMalloc     = 0;
   alloc.numFree       = 0;
   alloc.poolLength    = 0;
   alloc.poolOffset    = NULL_OFFSET;
   alloc.sizeQuant     = 8;
   
   alloc.freeList.bh.prevfree = 0;
   alloc.freeList.bh.bsize    = 0;

   /* Must be initialized before being use in SET_LINK */
   alloc.pBaseAddr = (unsigned char*) dum;

   alloc.freeList.ql.flink = SET_LINK( &alloc.freeList, &alloc );
   alloc.freeList.ql.blink = SET_LINK( &alloc.freeList, &alloc );

   vdseMemAllocbpool ( &alloc, dum, 10000, NULL );

   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}
