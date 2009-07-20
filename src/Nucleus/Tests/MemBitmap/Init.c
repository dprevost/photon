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
   size_t i;
   psonSessionContext context;
   
   initTest( &context );

   ptr = malloc( PSON_BLOCK_SIZE*10 );
   assert( ptr != NULL );
   g_pBaseAddr = ptr;
   
   pBitmap = (psonMemBitmap*) ptr;
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
   expect_assert_failure( psonMemBitmapInit( NULL, 
                                             SET_OFFSET(ptr),
                                             10*PSON_BLOCK_SIZE,
                                             8 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemBitmapInit( pBitmap, 
                                             PSON_NULL_OFFSET,
                                             10*PSON_BLOCK_SIZE,
                                             8 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_poweroftwo7( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemBitmapInit( pBitmap, 
                                             SET_OFFSET(ptr),
                                             10*PSON_BLOCK_SIZE,
                                             7 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_poweroftwo9( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemBitmapInit( pBitmap, 
                                             SET_OFFSET(ptr),
                                             10*PSON_BLOCK_SIZE,
                                             9 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_granu( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemBitmapInit( pBitmap, 
                                             SET_OFFSET(ptr),
                                             10*PSON_BLOCK_SIZE,
                                             0 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemBitmapInit( pBitmap, 
                                             SET_OFFSET(ptr),
                                             0,
                                             8 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   size_t i;
   psonSessionContext context;
   
   psonMemBitmapInit( pBitmap, 
                      SET_OFFSET(ptr),
                      10*PSON_BLOCK_SIZE,
                      8 );
   
   assert_true( pBitmap->lengthInBits == 10*PSON_BLOCK_SIZE/8 );
   assert_true( pBitmap->allocGranularity == 8 );
   assert_true( pBitmap->baseAddressOffset == SET_OFFSET(ptr) );
   for ( i = 0; i < pBitmap->lengthInBits / 8; ++i ) {
      assert_true( pBitmap->bitmap[i] == 0 );
   }
   
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
      unit_test_setup_teardown( test_poweroftwo7, setup_test, teardown_test ),
      unit_test_setup_teardown( test_poweroftwo9, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_granu,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

