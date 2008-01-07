/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/MemBitmap.h"
#include "Engine/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemBitmap *pBitmap;
   unsigned char* ptr;
   size_t i;
   vdseSessionContext context;
   
   initTest( expectedToPass, &context );

   ptr = malloc( VDSE_BLOCK_SIZE*10 );
   if (ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   g_pBaseAddr = ptr;
   
   pBitmap = (vdseMemBitmap*) ptr;
   
   vdseMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*VDSE_BLOCK_SIZE,
                      8 );
   if ( pBitmap->lengthInBits != 10*VDSE_BLOCK_SIZE/8 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pBitmap->allocGranularity != 8 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pBitmap->baseAddressOffset != SET_OFFSET(ptr) )
      ERROR_EXIT( expectedToPass, NULL, ; );
   for ( i = 0; i < pBitmap->lengthInBits / 8; ++i )
      if ( pBitmap->bitmap[i] != 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );

   vdseMemBitmapFini( pBitmap );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

