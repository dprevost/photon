/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "MemBitmap.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMemBitmapInit( vdseMemBitmap* pBitmap,
                        ptrdiff_t      baseAddressOffset,
                        size_t         totalLength,
                        size_t         allocGranularity )
{
   size_t len, i;
   
   VDS_PRE_CONDITION( pBitmap     != NULL );
   VDS_PRE_CONDITION( baseAddressOffset != VDSE_NULL_OFFSET );
   VDS_PRE_CONDITION( totalLength      > 0 );
   /* Testing that it is non-zero and a power of two */
   VDS_PRE_CONDITION( allocGranularity > 0  && 
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

void vdseMemBitmapFini( vdseMemBitmap* pBitmap )
{
   size_t len, i;
   VDS_PRE_CONDITION( pBitmap != NULL );
   
   len = ( (pBitmap->lengthInBits - 1 ) >> 3 ) + 1;
   for ( i = 0; i < len; ++i ) {
      pBitmap->bitmap[i] = 0;
   }
   
   pBitmap->lengthInBits = 0;
   pBitmap->allocGranularity = 0;
   pBitmap->baseAddressOffset = VDSE_NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

