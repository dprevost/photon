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

#include "MemBitmap.h"
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemBitmap *pBitmap;
   vdsErrors errcode;
   unsigned char* ptr;
   size_t i;
   
   initTest( expectedToPass );

   ptr = malloc( PAGESIZE*10 );
   if (ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   g_pBaseAddr = ptr;
   
   pBitmap = (vdseMemBitmap*) ptr;
   
   vdseMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*PAGESIZE,
                      8 );

   vdseSetBlocksAllocated( pBitmap,
                           PAGESIZE/4, /* offset */
                           PAGESIZE*2 ); /* length */

   vdseSetBlocksFree( pBitmap,
                      PAGESIZE/2, /* offset */
                      PAGESIZE/4 ); /* length */
   vdseSetBlocksFree( pBitmap,
                      PAGESIZE,
                      PAGESIZE*3/4 );
   
   for ( i = PAGESIZE/4/8/8; i < PAGESIZE/2/8/8 ; ++i )
      if ( pBitmap->bitmap[i] != 0xff )
         ERROR_EXIT( expectedToPass, NULL, );
   for ( i = PAGESIZE/2/8/8; i < PAGESIZE*3/4/8/8 ; ++i )
      if ( pBitmap->bitmap[i] != 0 )
         ERROR_EXIT( expectedToPass, NULL, );
   for ( i = PAGESIZE*3/4/8/8; i < PAGESIZE/8/8 ; ++i )
      if ( pBitmap->bitmap[i] != 0xff )
         ERROR_EXIT( expectedToPass, NULL, );
   for ( i = PAGESIZE/8/8; i < PAGESIZE*7/4/8/8 ; ++i )
      if ( pBitmap->bitmap[i] != 0 )
         ERROR_EXIT( expectedToPass, NULL, );
   for ( i = PAGESIZE*7/4/8/8; i < PAGESIZE*9/4/8/8 ; ++i )
      if ( pBitmap->bitmap[i] != 0xff )
         ERROR_EXIT( expectedToPass, NULL, );

   if ( pBitmap->bitmap[PAGESIZE*9/4/8/8] != 0 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   vdseMemBitmapFini( pBitmap );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

