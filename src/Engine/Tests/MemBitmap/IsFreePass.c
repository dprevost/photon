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

#include "MemBitmap.h"
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemBitmap *pBitmap;
   unsigned char* ptr;
   bool answer;
   
   initTest( expectedToPass );

   ptr = malloc( VDSE_PAGE_SIZE*10 );
   if (ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   g_pBaseAddr = ptr;
   
   pBitmap = (vdseMemBitmap*) ptr;
   
   vdseMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*VDSE_PAGE_SIZE,
                      8 );

  vdseSetBlocksAllocated( pBitmap,
                          VDSE_PAGE_SIZE/4, /* offset */
                          VDSE_PAGE_SIZE*2 ); /* length */

   vdseSetBlocksFree( pBitmap,
                      VDSE_PAGE_SIZE/2, /* offset */
                      VDSE_PAGE_SIZE/4 ); /* length */

   answer = vdseIsBlockFree( pBitmap, VDSE_PAGE_SIZE/4 );
   if ( answer == true )
      ERROR_EXIT( expectedToPass, NULL, );

   answer = vdseIsBlockFree( pBitmap, VDSE_PAGE_SIZE/2 );
   if ( answer == false )
      ERROR_EXIT( expectedToPass, NULL, );

   vdseMemBitmapFini( pBitmap );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

