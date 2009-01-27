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

#include "MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemBitmapInit( psonMemBitmap* pBitmap,
                        ptrdiff_t      baseAddressOffset,
                        size_t         totalLength,
                        size_t         allocGranularity )
{
   size_t len, i;
   
   PSO_PRE_CONDITION( pBitmap     != NULL );
   PSO_PRE_CONDITION( baseAddressOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( totalLength      > 0 );
   /* Testing that it is non-zero and a power of two */
   PSO_PRE_CONDITION( allocGranularity > 0  && 
                      ! (allocGranularity & (allocGranularity-1)) );

   pBitmap->lengthInBits = totalLength/allocGranularity;
   pBitmap->allocGranularity = allocGranularity;
   pBitmap->baseAddressOffset = baseAddressOffset;
   
   len = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   for ( i = 0; i < len; ++i ) {
      pBitmap->bitmap[i] = 0;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemBitmapFini( psonMemBitmap* pBitmap )
{
   size_t len, i;
   PSO_PRE_CONDITION( pBitmap != NULL );
   
   len = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   for ( i = 0; i < len; ++i ) {
      pBitmap->bitmap[i] = 0;
   }
   
   pBitmap->lengthInBits = 0;
   pBitmap->allocGranularity = 0;
   pBitmap->baseAddressOffset = PSON_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

