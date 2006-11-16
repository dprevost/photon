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
   vdsErrors err;
   vdseMemAlloc alloc;
   int isIt;
   vdscErrorHandler error;
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &error );
   
   err = vdseMemAllocInit( &alloc, dum, dum, 10000, &error );
   if ( err != 0 )
      return -1;

   ptr = vdseMalloc( &alloc, 100, &error );
   if ( ptr == NULL )
      return -1;
   
   isIt = vdseMemAllocIsBufferFree( &alloc, ptr, &error );
   if ( isIt )
      return -1;

   vdseFree( &alloc, ptr, &error );

   isIt = vdseMemAllocIsBufferFree( &alloc, ptr, &error );
   if ( ! isIt )
      return -1;

   vdscFiniErrorHandler( &error );
   vdscFiniErrorDefs();
 
   return 0;
}

