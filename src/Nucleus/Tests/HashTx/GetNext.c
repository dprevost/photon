/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/Hash.h"
#include "Nucleus/Tests/HashTx/HashTest.h"

psonSessionContext context;
psonHashTx * pHash;
ptrdiff_t offsetFirstItem = PSON_NULL_OFFSET, offsetNextItem = PSON_NULL_OFFSET;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   enum psoErrors errcode;
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   psonHashTxItem * pNewItem;
   bool found;
   size_t bucket;
   
   pHash = initHashTest( &context );
   
   errcode = psonHashTxInit( pHash, g_memObjOffset, 100, &context );
   assert( errcode == PSO_OK );
   
   found = psonHashTxGet( pHash,
                          (unsigned char*)key1,
                          strlen(key1),
                          &pNewItem,
                          &bucket,
                          &context );
   assert( !found );
   
   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key1,
                               strlen(key1),
                               data1,
                               strlen(data1),
                               &pNewItem,
                               &context );
   assert( errcode == PSO_OK );

   found = psonHashTxGet( pHash,
                          (unsigned char*)key2,
                          strlen(key2),
                          &pNewItem,
                          &bucket,
                          &context );
   assert( !found );
   
   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key2,
                               strlen(key2),
                               data2,
                               strlen(data2),
                               &pNewItem,
                               &context );
   assert( errcode == PSO_OK );
   
   found = psonHashTxGetFirst( pHash, &offsetFirstItem );
   assert( found );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGetNext( NULL,
                                             offsetNextItem,
                                             &offsetNextItem ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_next_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGetNext( pHash,
                                             offsetNextItem,
                                             NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_prev_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGetNext( pHash,
                                             PSON_NULL_OFFSET,
                                             &offsetNextItem ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool found;
   
   found = psonHashTxGetNext( pHash,
                              offsetFirstItem,
                              &offsetNextItem );
   assert_true( found );
   assert_true( offsetNextItem != PSON_NULL_OFFSET );
   
   /* Only 2 items - should fail gracefully ! */
   found = psonHashTxGetNext( pHash,
                              offsetNextItem,
                              &offsetNextItem );
   assert_false( found );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_hash,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_next_offset, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_prev_offset, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

