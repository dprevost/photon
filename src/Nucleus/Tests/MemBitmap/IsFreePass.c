/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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
   psonMemBitmap *pBitmap;
   unsigned char* ptr;
   bool answer;
   psonSessionContext context;
   
   initTest( expectedToPass, &context );

   ptr = malloc( PSON_BLOCK_SIZE*10 );
   if (ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   g_pBaseAddr = ptr;
   
   pBitmap = (psonMemBitmap*) ptr;
   
   psonMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*PSON_BLOCK_SIZE,
                      8 );

  psonSetBufferAllocated( pBitmap,
                          PSON_BLOCK_SIZE/4, /* offset */
                          PSON_BLOCK_SIZE*2 ); /* length */

   psonSetBufferFree( pBitmap,
                      PSON_BLOCK_SIZE/2, /* offset */
                      PSON_BLOCK_SIZE/4 ); /* length */

   answer = psonIsBufferFree( pBitmap, PSON_BLOCK_SIZE/4 );
   if ( answer == true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   answer = psonIsBufferFree( pBitmap, PSON_BLOCK_SIZE/2 );
   if ( answer == false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonMemBitmapFini( pBitmap );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

