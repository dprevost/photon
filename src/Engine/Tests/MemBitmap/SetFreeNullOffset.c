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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemBitmap *pBitmap;
   unsigned char* ptr;
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

   vdseSetBufferAllocated( pBitmap,
                           VDSE_BLOCK_SIZE/4, /* offset */
                           VDSE_BLOCK_SIZE*2 ); /* length */

   vdseSetBufferFree( pBitmap,
                      NULL_OFFSET, /* offset */
                      VDSE_BLOCK_SIZE/4 ); /* length */
   
   vdseMemBitmapFini( pBitmap );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
