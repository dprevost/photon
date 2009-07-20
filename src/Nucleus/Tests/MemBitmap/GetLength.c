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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_poweroftwo7( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 7 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_poweroftwo9( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 9 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_granu( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 1024, 0 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonGetBitmapLengthBytes( 0, 8 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   size_t calculatedSize;
   
   calculatedSize = psonGetBitmapLengthBytes( 1024, 8 );
   assert_true( calculatedSize == 1024/8/8 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1023, 8 );
   assert_true( calculatedSize == 1024/8/8 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1025, 8 );
   assert_true( calculatedSize == 1024/8/8 + 1 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1016, 8 );
   assert_true( calculatedSize == 1024/8/8 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+63, 8 );
   assert_true( calculatedSize == 1024/8/8 + 1 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+64, 8 );
   assert_true( calculatedSize == 1024/8/8 + 1 );
   
   calculatedSize = psonGetBitmapLengthBytes( 1024+65, 8 );
   assert_true( calculatedSize == 1024/8/8 + 2 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_poweroftwo7 ),
      unit_test( test_poweroftwo9 ),
      unit_test( test_zero_granu ),
      unit_test( test_zero_length ),
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

