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

#include "Nucleus/MemoryObject.h"
#include "MemObjTest.h"

psonMemObject* pObj;
psonSessionContext context;
unsigned char *buff[9];
psotObjDummy  *pDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psoErrors errcode;
   
   pDummy = initMemObjTest( &context );
   pObj = &pDummy->memObject;
   
   errcode = psonMemObjectInit( pObj, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   assert( errcode == PSO_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( g_pBaseAddr ) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMalloc( pObj, PSON_BLOCK_SIZE, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_obj( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMalloc( NULL, PSON_BLOCK_SIZE, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_bytes( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMalloc( pObj, 0, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoErrors errcode;
   
   buff[0] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   assert_false( buff[0] == NULL );
   assert_true( pDummy->blockGroup.maxFreeBytes == 
        pDummy->blockGroup.freeBytes+PSON_BLOCK_SIZE );
   
   buff[1] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   assert_false( buff[1] == NULL );
   
   buff[2] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   assert_false( buff[2] == NULL );
   
   /* Needs two new blocks at this point */
   assert_true( pDummy->blockGroup.freeBytes < PSON_BLOCK_SIZE );
   buff[3] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   assert_false( buff[3] == NULL );
   assert_true( pObj->totalBlocks == 6 );
   
   /* Needs NO new block at this point */
   buff[4] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   assert_false( buff[4] == NULL );
   assert_true( pObj->totalBlocks == 6 );
   
   /* Needs NO new block at this point */
   buff[5] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   assert_false( buff[5] == NULL );
   assert_true( pObj->totalBlocks == 6 );
   
   /* Needs a new block at this point */
   buff[6] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   assert_false( buff[6] == NULL );
   assert_true( pObj->totalBlocks == 7 );
   
   /* Needs a new block at this point */
   buff[7] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   assert_false( buff[7] == NULL );
   assert_true( pObj->totalBlocks == 8 );
   
   /* We exhausted all the memory */
   buff[8] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   assert_true( buff[8] == NULL );
   assert_true( pObj->totalBlocks == 8 );
   
   errcode = psonMemObjectFini( pObj, PSON_ALLOC_ANY, &context );
   assert_true( errcode == PSO_OK );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_obj,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_bytes,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

