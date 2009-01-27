/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/MemoryAllocator.h"

/*
 * This is NOT a test. All this program do is calculate the size needed
 * by InitBigShMem and display it.
 *
 * Use this program before running InitBigShMem to see if you have enough RAM
 * (you can always calculate it manually, of course...).
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   size_t allocatedLength;

   allocatedLength = 8*PSON_BLOCK_SIZE*PSON_BLOCK_SIZE;

   fprintf( stderr, "Amount of memory needed = \n" );
   
   fprintf( stderr, "   "PSO_SIZE_T_FORMAT" bytes or \n" , allocatedLength );
   fprintf( stderr, "   "PSO_SIZE_T_FORMAT" kbytes or \n", allocatedLength/1024 );
   fprintf( stderr, "   "PSO_SIZE_T_FORMAT" Mbytes  \n"  , allocatedLength/1024/1024 );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

