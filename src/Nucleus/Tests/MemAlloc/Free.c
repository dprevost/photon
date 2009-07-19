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

psonMemAlloc* pAlloc;
psonSessionContext context;

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
   unsigned char* newBuff;

   newBuff = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff == NULL );

   expect_assert_failure( psonFreeBlocks( NULL, 
                                          PSON_ALLOC_ANY,
                                          newBuff,
                                          2,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_buffer( void ** state )
{
#if defined(PSO_UNIT_TESTS)

   expect_assert_failure( psonFreeBlocks( pAlloc, 
                                          PSON_ALLOC_ANY,
                                          NULL,
                                          2,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   unsigned char* newBuff;
   
   newBuff = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff == NULL );

   expect_assert_failure( psonFreeBlocks( pAlloc, 
                                          PSON_ALLOC_ANY,
                                          newBuff,
                                          2,
                                          NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   unsigned char* newBuff;
   
   newBuff = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff == NULL );

   expect_assert_failure( psonFreeBlocks( pAlloc, 
                                          PSON_ALLOC_ANY,
                                          newBuff,
                                          0,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
//   unsigned char* ptr;
//   size_t allocatedLength = PSON_BLOCK_SIZE*10;
   unsigned char* newBuff[5];
   
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[0] == NULL );
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[0], 2, &context );
   assert_true( pAlloc->totalAllocBlocks == 2 );
   assert_true( pAlloc->numFreeCalls == 1 );
   
   /*
    * Test that the code will reunite free buffers (if they are
    * following each other, of course).
    */
   /* unite with following buffer */
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 3, &context );
   assert_false( newBuff[0] == NULL );
   newBuff[1] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 3, &context );
   assert_false( newBuff[1] == NULL );
   newBuff[2] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[2] == NULL );
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[1], 3, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[0], 3, &context );
   /* if the "unite" failed, no 6 blocks free buffer should exist */
   newBuff[3] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   assert_false( newBuff[3] == NULL );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[3], 6, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[2], 2, &context );

   /* unite with preceding buffer */
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[0] == NULL );
   newBuff[1] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 3, &context );
   assert_false( newBuff[1] == NULL );
   newBuff[2] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 3, &context );
   assert_false( newBuff[2] == NULL );
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[1], 3, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[2], 3, &context );
   /* if the "unite" failed, no 6 blocks free buffer should exist */
   newBuff[3] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   assert_false( newBuff[3] == NULL );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[0], 2, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[3], 6, &context );

   /* unite with both */
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, &context );
   assert_false( newBuff[0] == NULL );
   newBuff[1] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[1] == NULL );
   newBuff[2] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[2] == NULL );
   newBuff[3] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   assert_false( newBuff[3] == NULL );
   newBuff[4] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, &context );
   assert_false( newBuff[4] == NULL );
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[1], 2, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[3], 2, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[2], 2, &context );
   /* if the "unite" failed, no 6 blocks free buffer should exist */
   newBuff[1] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   assert_false( newBuff[1] == NULL );
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[0], 1, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[1], 6, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[4], 1, &context );

   assert_true( pAlloc->totalAllocBlocks == 2 );
   
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
      unit_test_setup_teardown( test_null_buffer,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

