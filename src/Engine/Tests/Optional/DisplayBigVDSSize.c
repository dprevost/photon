/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/MemoryAllocator.h"

/*
 * This is NOT a test. All this program do is calculate the size needed
 * by InitBigVDS and display it.
 *
 * Use this program before running InitBigVDS to see if you have enough RAM
 * (you can always calculate it manually, of course...).
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   size_t allocatedLength;

   allocatedLength = 8*VDSE_BLOCK_SIZE*VDSE_BLOCK_SIZE;

   fprintf( stderr, "Amount of memory needed = \n" );
   
   fprintf( stderr, "   %d bytes or \n" , allocatedLength );
   fprintf( stderr, "   %d kbytes or \n", allocatedLength/1024 );
   fprintf( stderr, "   %d Mbytes  \n"  , allocatedLength/1024/1024 );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
