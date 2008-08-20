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

#include "Nucleus/MemBitmap.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnMemBitmap *pBitmap;
   unsigned char* ptr;
   size_t i;
   psnSessionContext context;
   
   initTest( expectedToPass, &context );

   ptr = malloc( PSN_BLOCK_SIZE*10 );
   if (ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   g_pBaseAddr = ptr;
   
   pBitmap = (psnMemBitmap*) ptr;
   
   psnMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*PSN_BLOCK_SIZE,
                      8 );

   /* We do this to test that fini() zero things out */
   psnSetBufferAllocated( pBitmap,
                           PSN_BLOCK_SIZE, /* offset */
                           PSN_BLOCK_SIZE ); /* length */
                             
   psnMemBitmapFini( pBitmap );

   if ( pBitmap->lengthInBits != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pBitmap->allocGranularity != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pBitmap->baseAddressOffset != PSN_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   for ( i = 0; i < pBitmap->lengthInBits / 8; ++i ) {
      if ( pBitmap->bitmap[i] != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   
   free(ptr);
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
