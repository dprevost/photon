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

#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Tests/EngineTestCommon.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonSessionContext context;
   psonMemAlloc*     pAlloc;
   unsigned char* ptr;
   bool isFree;
   psonMemBitmap* pBitmap;
   
   initTest( &context );

   ptr = malloc( 51*PSON_BLOCK_SIZE );
   assert_false( ptr == NULL );
   
   g_pBaseAddr = (unsigned char *)((((size_t)ptr - 1)/PSON_BLOCK_SIZE + 1)*PSON_BLOCK_SIZE);
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, g_pBaseAddr, 50*PSON_BLOCK_SIZE, &context );
   GET_PTR( pBitmap, pAlloc->bitmapOffset, psonMemBitmap );
   
   if ( (pBitmap->lengthInBits-1)/8+1 != 7 ) {
      fprintf( stderr, "Wrong bitmapLength, got "PSO_SIZE_T_FORMAT
               ", expected %d\n",
               pBitmap->lengthInBits/8, 7 );
      assert_true( (pBitmap->lengthInBits-1)/8+1 == 7 );
   }
   if ( pBitmap->bitmap[0] != 0xc0 ) {
      fprintf( stderr, "Wrong bitmap[0], got 0x%x, expected 0x%x\n", 
               pBitmap->bitmap[0], 0xc0 );
      assert_true( pBitmap->bitmap[0] == 0xc0 );
   }
   
   isFree = psonIsBufferFree( pBitmap, 0 );
   assert_false( isFree );
   
   isFree = psonIsBufferFree( pBitmap, 2*PSON_BLOCK_SIZE );
   assert_true( isFree );
   
   isFree = psonIsBufferFree( pBitmap, -PSON_BLOCK_SIZE );
   assert_false( isFree );
   
   isFree = psonIsBufferFree( pBitmap, 50*PSON_BLOCK_SIZE );
   assert_false( isFree );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

