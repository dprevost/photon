/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "Nucleus/Tests/EngineTestCommon.h"

/*
 * Goal of this test:
 *
 * To allocate a large block of memory. This block should be large enough
 * that it forces the allocator to use two blocks of memory (because of the
 * bitmap array.
 *
 * We use 8*PSON_BLOCK_SIZE*PSON_BLOCK_SIZE for the size.
 *
 * If the bitmap is PSON_BLOCK_SIZE than adding this with the psonMemAlloc struct
 * will force the bitmap to be on 2 blocks. A bitmap of that size contains
 * 8*PSON_BLOCK_SIZE blocks. And of course we multiply this by PSON_BLOCK_SIZE to get the
 * size of the required memory block. 
 */

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSessionContext context;
   psonMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength, i;
   unsigned char* buffer[8*PSON_BLOCK_SIZE-2];
   psonMemBitmap* pBitmap;

   initTest( expectedToPass, &context );
   
   allocatedLength = 8*PSON_BLOCK_SIZE*PSON_BLOCK_SIZE;

   ptr = malloc( allocatedLength );
   if ( ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   
   psonMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   GET_PTR( pBitmap, pAlloc->bitmapOffset, psonMemBitmap );
   if ( pBitmap->lengthInBits != 8*PSON_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Allocate all the blocks, one by one. */
   for ( i = 0; i < 8*PSON_BLOCK_SIZE-3; ++i ) {
      buffer[i] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, &context );
      if ( buffer[i] == NULL ) {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
   }
   buffer[8*PSON_BLOCK_SIZE-3] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, &context );
   if ( buffer[8*PSON_BLOCK_SIZE-3] != NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Check the bitmap pattern */
   for (i = 0; i < pBitmap->lengthInBits/8; ++i ) {
      if (pBitmap->bitmap[i] != 0xff ) {
         fprintf( stderr, "Malloc bitmap issue - "PSO_SIZE_T_FORMAT" 0x%x\n", 
                  i, pBitmap->bitmap[i] );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   
   /* Free 1 block out of two */
   for ( i = 0; i < 8*PSON_BLOCK_SIZE-3; i += 2 ) {
      psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, buffer[i], 1, &context );
   }
   
   /* Check the bitmap pattern - the first 3 are always busy */
   if (pBitmap->bitmap[0] != 0xea ) { /* 11101010 */
      fprintf( stderr, "Malloc bitmap issue - "PSO_SIZE_T_FORMAT" 0x%x\n", 
               i, pBitmap->bitmap[i] );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
      
   for (i = 1; i < pBitmap->lengthInBits/8; ++i ) {
      if (pBitmap->bitmap[i] != 0xaa ) { /* 10101010 */
         fprintf( stderr, "Malloc bitmap issue - "PSO_SIZE_T_FORMAT" 0x%x\n", 
                  i, pBitmap->bitmap[i] );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

