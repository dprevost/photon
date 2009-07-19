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

psonSessionContext context;
psonMemAlloc *     pAlloc;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   unsigned char* ptr;
   size_t allocatedLength = PSON_BLOCK_SIZE*10;
   
   initTest( &context );
   
   ptr = malloc( allocatedLength );
   assert( ptr != NULL );
   
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( g_pBaseAddr ) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
   pAlloc = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_alloc( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMallocBlocks( NULL,
                                            PSON_ALLOC_ANY,
                                            2,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMallocBlocks( pAlloc,
                                            PSON_ALLOC_ANY,
                                            2,
                                            NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMallocBlocks( pAlloc,
                                            PSON_ALLOC_ANY,
                                            0,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int i;
   unsigned char *    newBuff[8];
   
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[0] == NULL );
   
   /* 6 blocks remaining */
   newBuff[1] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   assert_false( newBuff[1] == NULL );
   
   /* No blocks remaining */
   newBuff[2] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   assert_true( newBuff[2] == NULL );
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[0], 2, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[1], 6, &context );
   /* 8 blocks remaining */
   
   for ( i = 0; i < 8; ++i ) {
      newBuff[i] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, &context );
      assert_false( newBuff[i] == NULL );
   }
   for ( i = 0; i < 8; i += 2 ) {
      psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[i], 1, &context );
   }
   
   /* 4 blocks remaining - fragmented. This new alloc should fail! */
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_true( newBuff[0] == NULL );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_alloc,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

