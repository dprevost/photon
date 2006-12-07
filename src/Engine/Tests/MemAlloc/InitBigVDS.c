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
#include "EngineTestCommon.h"

/*
 * Goal of this test:
 *
 * To allocate a large block of memory. This block should be large enough
 * that it forces the allocator to use two pages of memory (because of the
 * bitmap array.
 *
 * We use 8*PAGESIZE*PAGESIZE for the size.
 *
 * If the bitmap is PAGESIZE than adding this with the vdseMemAlloc struct
 * will force the bitmap to be on 2 pages. A bitmap of that size contains
 * 8*PAGESIZE pages. And of course we multiply this by PAGESIZE to get the
 * size of the required memory block. 
 */

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength, i;
   unsigned char* buffer[8*PAGESIZE-2];
   vdseMemBitmap* pBitmap;

   initTest( expectedToPass );
   vdscInitErrorHandler( &context.errorHandler );
   
   allocatedLength = 8*PAGESIZE*PAGESIZE;

   ptr = malloc( allocatedLength );
   if ( ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   pBitmap = GET_PTR( pAlloc->bitmapOffset, vdseMemBitmap );
   if ( pBitmap->lengthInBits != 8*PAGESIZE )
      ERROR_EXIT( expectedToPass, NULL, );
   
   /* Allocate all the pages, one by one. */
   for ( i = 0; i < 8*PAGESIZE-3; ++i )
   {
      buffer[i] = vdseMallocPages( pAlloc, 1, &context );
      if ( buffer[i] == NULL )
         ERROR_EXIT( expectedToPass, &context.errorHandler, );
   }
   buffer[8*PAGESIZE-3] = vdseMallocPages( pAlloc, 1, &context );
   if ( buffer[8*PAGESIZE-3] != NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   
   /* Check the bitmap pattern */
   for (i = 0; i < pBitmap->lengthInBits/8; ++i )
   {
      if (pBitmap->bitmap[i] != 0xff )
      {
         fprintf( stderr, "Malloc bitmap issue - %d 0x%x\n", i, 
                  pBitmap->bitmap[i] );
         ERROR_EXIT( expectedToPass, NULL, );
      }
   }
   
   /* Free 1 page out of two */
   for ( i = 0; i < 8*PAGESIZE-3; i += 2 )
   {
      vdseFreePages( pAlloc, buffer[i], 1, &context );
   }
   
   /* Check the bitmap pattern - the first 3 are always busy */
   if (pBitmap->bitmap[0] != 0xea ) /* 11101010 */
   {
      fprintf( stderr, "Malloc bitmap issue - %d 0x%x\n", i, 
               pBitmap->bitmap[i] );
      ERROR_EXIT( expectedToPass, NULL, );
   }
      
   for (i = 1; i < pBitmap->lengthInBits/8; ++i )
   {
      if (pBitmap->bitmap[i] != 0xaa ) /* 10101010 */
      {
         fprintf( stderr, "Malloc bitmap issue - %d 0x%x\n", i, 
                  pBitmap->bitmap[i] );
         ERROR_EXIT( expectedToPass, NULL, );
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

