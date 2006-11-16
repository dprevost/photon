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


/* #define GET_FLINK(p,alloc) ( (struct bfhead *) \ */
/*    ((alloc)->pBaseAddr+(p)->ql.flink ) ) */

/* #define GET_BLINK(p,alloc) ( (struct bfhead *) \ */
/*    ((alloc)->pBaseAddr+(p)->ql.blink ) ) */

/* #define SET_LINK(p,alloc)  ( (size_t) \ */
/*    ( (unsigned char*)(p) - (alloc)->pBaseAddr ) ) */

int main( int argc, char* argv[] )
{
   unsigned char dum[10000];
   vdsErrors err;
   vdseMemAlloc alloc;
   /* We use volatile to avoid the risk that a C compiler
    * might optimize the code by removing one of them... 
    */
   volatile unsigned char* ptr;
   volatile unsigned char* ptr2;
   ptrdiff_t offset;
   vdscErrorHandler error;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &error );
   
   err = vdseMemAllocInit( &alloc, dum, dum, 10000, &error );
   if ( err != 0 )
      return -1;
   
   /* Allocate 2 dummy buffers */
   ptr = (unsigned char*) vdseMalloc( &alloc, 1000, &error );
   if ( ptr == NULL ) return -1;

   offset = SET_OFFSET( ptr, &alloc );
   
   if ( offset != (ptr - dum ) )
      return -1;
   
   ptr2 = GET_PTR( offset, unsigned char, &alloc );
   if ( ptr2 != ptr )
      return -1;

   SET_PTR( ptr2, offset, unsigned char, &alloc );
   if ( ptr2 != ptr )
      return -1;

   SET_PTR( ptr2, NULL_OFFSET, unsigned char, &alloc );
   if ( ptr2 != NULL )
      return -1;

   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}

