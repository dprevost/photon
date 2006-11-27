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

#include "MemoryAllocator.c"
#include "EngineTestCommon.h"

int main()
{
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   bool isFree;
   vdseMemBitmap* pBitmap;
   
   initTest( true );
   vdscInitErrorHandler( &context.errorHandler );

   ptr = malloc( 50*PAGESIZE );

   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, 50*PAGESIZE, &context );
   pBitmap = GET_PTR( pAlloc->bitmapOffset, vdseMemBitmap );
   
   if ( (pBitmap->lengthInBits-1)/8+1 != 7 )
   {
      fprintf( stderr, "Wrong bitmapLength, got %d, expected %d\n",
               pBitmap->lengthInBits/8, 7 );
      return 1;
   }
   if ( pBitmap->bitmap[0] != 0xc0 )
   {
      fprintf( stderr, "Wrong bitmap[0], got 0x%x, expected 0x%x\n", 
               pBitmap->bitmap[0], 0xc0 );
      return 1;
   }
   
   isFree = vdseIsBlockFree( pBitmap, 0 );
//   fprintf( stderr, "q = %d\n", isFree );
   if ( isFree )
      return 1;

   isFree = vdseIsBlockFree( pBitmap, 2*PAGESIZE );
//   fprintf( stderr, "q = %d\n", isFree );
   if ( ! isFree )
      return 1;

   isFree = vdseIsBlockFree( pBitmap, -PAGESIZE );
//   fprintf( stderr, "q = %d\n", isFree );
   if ( isFree )
      return 1;
   
   isFree = vdseIsBlockFree( pBitmap, 50*PAGESIZE );
//   fprintf( stderr, "q = %d\n", isFree );
   if ( isFree )
      return 1;
      
   return 0;
}

