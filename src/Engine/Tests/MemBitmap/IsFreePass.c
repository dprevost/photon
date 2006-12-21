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
   bool answer;
   
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

   answer = vdseIsBlockFree( pBitmap, PAGESIZE/4 );
   if ( answer == true )
      ERROR_EXIT( expectedToPass, NULL, );

   answer = vdseIsBlockFree( pBitmap, PAGESIZE/2 );
   if ( answer == false )
      ERROR_EXIT( expectedToPass, NULL, );

   vdseMemBitmapFini( pBitmap );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

