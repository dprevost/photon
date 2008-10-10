/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSessionContext context;
   psonMemAlloc*     pAlloc;
   unsigned char* ptr;
   bool isFree;
   psonMemBitmap* pBitmap;
   
   initTest( expectedToPass, &context );

   ptr = malloc( 51*PSON_BLOCK_SIZE );
   if ( ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   g_pBaseAddr = (unsigned char *)((((size_t)ptr - 1)/PSON_BLOCK_SIZE + 1)*PSON_BLOCK_SIZE);
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, g_pBaseAddr, 50*PSON_BLOCK_SIZE, &context );
   GET_PTR( pBitmap, pAlloc->bitmapOffset, psonMemBitmap );
   
   if ( (pBitmap->lengthInBits-1)/8+1 != 7 ) {
      fprintf( stderr, "Wrong bitmapLength, got "PSO_SIZE_T_FORMAT
               ", expected %d\n",
               pBitmap->lengthInBits/8, 7 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pBitmap->bitmap[0] != 0xc0 ) {
      fprintf( stderr, "Wrong bitmap[0], got 0x%x, expected 0x%x\n", 
               pBitmap->bitmap[0], 0xc0 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psonIsBufferFree( pBitmap, 0 );
   if ( isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psonIsBufferFree( pBitmap, 2*PSON_BLOCK_SIZE );
   if ( ! isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psonIsBufferFree( pBitmap, -PSON_BLOCK_SIZE );
   if ( isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psonIsBufferFree( pBitmap, 50*PSON_BLOCK_SIZE );
   if ( isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

