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

#include "Nucleus/BlockGroup.h"
#include "Nucleus/Tests/EngineTestCommon.h"

psonBlockGroup * pGroup;
unsigned char* ptr;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psonSessionContext context;
   
   initTest( &context );

   ptr = malloc( PSON_BLOCK_SIZE*10 );
   assert( ptr != NULL );
   g_pBaseAddr = ptr;
   
   /* This "100" (non-zero) offset should mark this block group 
    * as the first block group of a MemObject.
    */
   pGroup = (psonBlockGroup*) (ptr + 100);
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( ptr) free(ptr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonBlockGroupInit( pGroup, 
                                              SET_OFFSET(ptr),
                                              10,
                                              (psonMemObjIdent)(PSON_IDENT_LAST+200) ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_group( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonBlockGroupInit( NULL, 
                                              SET_OFFSET(ptr),
                                              10,
                                              PSON_IDENT_QUEUE ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonBlockGroupInit( pGroup, 
                                              PSON_NULL_OFFSET,
                                              10,
                                              PSON_IDENT_QUEUE ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_blocks( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonBlockGroupInit( pGroup, 
                                              SET_OFFSET(ptr),
                                              0,
                                              PSON_IDENT_QUEUE ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)

   psonBlockGroupInit( pGroup, 
                      SET_OFFSET(ptr),
                      10,
                      PSON_IDENT_QUEUE );
   assert_true( pGroup->node.nextOffset == PSON_NULL_OFFSET );
   assert_true( pGroup->node.previousOffset == PSON_NULL_OFFSET );
   assert_true( pGroup->numBlocks == 10 );
   assert_true( pGroup->maxFreeBytes >= 9*PSON_BLOCK_SIZE &&
                pGroup->maxFreeBytes < 10*PSON_BLOCK_SIZE );
   assert_true( pGroup->freeList.initialized == PSON_LIST_SIGNATURE );
   assert_false( pGroup->isDeletable );
   assert_true( pGroup->bitmap.baseAddressOffset == SET_OFFSET(ptr) );
   
   psonBlockGroupFini( pGroup );

   /* A zero offset this time */
   pGroup = (psonBlockGroup*) ptr;
   psonBlockGroupInit( pGroup, SET_OFFSET(ptr), 10, PSON_IDENT_QUEUE );

   assert_true( pGroup->isDeletable );
   
   psonBlockGroupFini( pGroup );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_type, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_group,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_offset,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_blocks,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

