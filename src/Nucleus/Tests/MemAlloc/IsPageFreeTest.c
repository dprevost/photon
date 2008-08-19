/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
   psnSessionContext context;
   psnMemAlloc*     pAlloc;
   unsigned char* ptr;
   bool isFree;
   psnMemBitmap* pBitmap;
   
   initTest( expectedToPass, &context );

   ptr = malloc( 51*PSN_BLOCK_SIZE );
   if ( ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   g_pBaseAddr = (unsigned char *)((((size_t)ptr - 1)/PSN_BLOCK_SIZE + 1)*PSN_BLOCK_SIZE);
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, g_pBaseAddr, 50*PSN_BLOCK_SIZE, &context );
   GET_PTR( pBitmap, pAlloc->bitmapOffset, psnMemBitmap );
   
   if ( (pBitmap->lengthInBits-1)/8+1 != 7 ) {
      fprintf( stderr, "Wrong bitmapLength, got "VDSF_SIZE_T_FORMAT
               ", expected %d\n",
               pBitmap->lengthInBits/8, 7 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pBitmap->bitmap[0] != 0xc0 ) {
      fprintf( stderr, "Wrong bitmap[0], got 0x%x, expected 0x%x\n", 
               pBitmap->bitmap[0], 0xc0 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psnIsBufferFree( pBitmap, 0 );
   if ( isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psnIsBufferFree( pBitmap, 2*PSN_BLOCK_SIZE );
   if ( ! isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psnIsBufferFree( pBitmap, -PSN_BLOCK_SIZE );
   if ( isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = psnIsBufferFree( pBitmap, 50*PSN_BLOCK_SIZE );
   if ( isFree ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

