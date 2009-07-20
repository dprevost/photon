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

#include "Nucleus/MemBitmap.h"
#include "Nucleus/Tests/EngineTestCommon.h"

psonMemBitmap * pBitmap;
unsigned char * ptr;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psonSessionContext context;
   
   initTest( &context );

   ptr = malloc( PSON_BLOCK_SIZE*10 );
   assert( ptr != NULL );
   g_pBaseAddr = ptr;
   
   pBitmap = (psonMemBitmap*) ptr;
   
   psonMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*PSON_BLOCK_SIZE,
                      8 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (ptr) free(ptr);
   ptr = NULL;
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_bitmap( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSetBufferFree( NULL,
                                             PSON_BLOCK_SIZE/2,
                                             PSON_BLOCK_SIZE/4 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSetBufferFree( pBitmap,
                                             PSON_NULL_OFFSET,
                                             PSON_BLOCK_SIZE/4 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   size_t i;
   
   psonSetBufferAllocated( pBitmap,
                           PSON_BLOCK_SIZE/4, /* offset */
                           PSON_BLOCK_SIZE*2 ); /* length */

   psonSetBufferFree( pBitmap,
                      PSON_BLOCK_SIZE/2, /* offset */
                      PSON_BLOCK_SIZE/4 ); /* length */
   psonSetBufferFree( pBitmap,
                      PSON_BLOCK_SIZE,
                      PSON_BLOCK_SIZE*3/4 );
   
   for ( i = PSON_BLOCK_SIZE/4/8/8; i < PSON_BLOCK_SIZE/2/8/8 ; ++i ) {
      assert_true( pBitmap->bitmap[i] == 0xff );
   }
   for ( i = PSON_BLOCK_SIZE/2/8/8; i < PSON_BLOCK_SIZE*3/4/8/8 ; ++i ) {
      assert_true( pBitmap->bitmap[i] == 0 );
   }
   for ( i = PSON_BLOCK_SIZE*3/4/8/8; i < PSON_BLOCK_SIZE/8/8 ; ++i ) {
      assert_true( pBitmap->bitmap[i] == 0xff );
   }
   for ( i = PSON_BLOCK_SIZE/8/8; i < PSON_BLOCK_SIZE*7/4/8/8 ; ++i ) {
      assert_true( pBitmap->bitmap[i] == 0 );
   }
   for ( i = PSON_BLOCK_SIZE*7/4/8/8; i < PSON_BLOCK_SIZE*9/4/8/8 ; ++i ) {
      assert_true( pBitmap->bitmap[i] == 0xff );
   }

   assert_true( pBitmap->bitmap[PSON_BLOCK_SIZE*9/4/8/8] == 0 );
   
   psonMemBitmapFini( pBitmap );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_bitmap, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_offset, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

