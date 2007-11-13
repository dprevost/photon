/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
#include "Engine/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   bool isFree;
   vdseMemBitmap* pBitmap;
   
   initTest( expectedToPass, &context );

   ptr = malloc( 51*VDSE_BLOCK_SIZE );
   if ( ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   g_pBaseAddr = (unsigned char *)((((size_t)ptr - 1)/VDSE_BLOCK_SIZE + 1)*VDSE_BLOCK_SIZE);
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, g_pBaseAddr, 50*VDSE_BLOCK_SIZE, &context );
   GET_PTR( pBitmap, pAlloc->bitmapOffset, vdseMemBitmap );
   
   if ( (pBitmap->lengthInBits-1)/8+1 != 7 )
   {
      fprintf( stderr, "Wrong bitmapLength, got %d, expected %d\n",
               pBitmap->lengthInBits/8, 7 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pBitmap->bitmap[0] != 0xc0 )
   {
      fprintf( stderr, "Wrong bitmap[0], got 0x%x, expected 0x%x\n", 
               pBitmap->bitmap[0], 0xc0 );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   isFree = vdseIsBufferFree( pBitmap, 0 );
   if ( isFree )
      ERROR_EXIT( expectedToPass, NULL, ; );

   isFree = vdseIsBufferFree( pBitmap, 2*VDSE_BLOCK_SIZE );
   if ( ! isFree )
      ERROR_EXIT( expectedToPass, NULL, ; );

   isFree = vdseIsBufferFree( pBitmap, -VDSE_BLOCK_SIZE );
   if ( isFree )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   isFree = vdseIsBufferFree( pBitmap, 50*VDSE_BLOCK_SIZE );
   if ( isFree )
      ERROR_EXIT( expectedToPass, NULL, ; );
      
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

