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
#include "Nucleus/Tests/Hash/HashTest.h"

psonSessionContext context;
psonHash* pHash;
char* key1 = "My Key 1";
char* data1 = "My Data 1";
char* data2 = "My Data 2";
psonHashItem * pHashItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   enum psoErrors errcode;
   
   pHash = initHashTest( &context );
   
   errcode = psonHashInit( pHash, g_memObjOffset, 100, &context );
   assert( errcode == PSO_OK );
   
   errcode = psonHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pHashItem,
                             &context );
   assert( errcode == PSO_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( pHash,
                                          (unsigned char*)key1,
                                          strlen(key1),
                                          data2,
                                          strlen(data2),
                                          &pHashItem,
                                          NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_data( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( pHash,
                                          (unsigned char*)key1,
                                          strlen(key1),
                                          NULL,
                                          strlen(data2),
                                          &pHashItem,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( NULL,
                                          (unsigned char*)key1,
                                          strlen(key1),
                                          data2,
                                          strlen(data2),
                                          &pHashItem,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( pHash,
                                          (unsigned char*)key1,
                                          strlen(key1),
                                          data2,
                                          strlen(data2),
                                          NULL,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( pHash,
                                          NULL,
                                          strlen(key1),
                                          data2,
                                          strlen(data2),
                                          &pHashItem,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_data_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( pHash,
                                          (unsigned char*)key1,
                                          strlen(key1),
                                          data2,
                                          0,
                                          &pHashItem,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_key_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashUpdate( pHash,
                                          (unsigned char*)key1,
                                          0,
                                          data2,
                                          strlen(data2),
                                          &pHashItem,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   enum psoErrors errcode;
   char* data3 = "This data is much longer than data1";
   size_t bucket;
   psonHashItem* pItem = NULL;
   bool found;
   
   /*
    * Test with same data length
    */
   errcode = psonHashUpdate( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data2,
                             strlen(data2),
                             &pHashItem,
                             &context );
   assert_true( errcode == PSO_OK );
   
   found = psonHashGet( pHash,
                        (unsigned char*)key1,
                        strlen(key1),
                        &pItem,
                        &bucket,
                        &context );
   assert_true( found );
   assert_true( memcmp( GET_PTR_FAST(pItem->dataOffset, void), data2, strlen(data2 )) == 0 );

   /*
    * Test with different data length
    */
   errcode = psonHashUpdate( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data3,
                             strlen(data3),
                             &pHashItem,
                             &context );
   assert_true( errcode == PSO_OK );
   
   found = psonHashGet( pHash,
                        (unsigned char*)key1,
                        strlen(key1),
                        &pItem,
                        &bucket,
                        &context );
   assert_true( found );
   assert_true( pItem->dataLength == strlen(data3) );
   assert_true( memcmp( GET_PTR_FAST(pItem->dataOffset, void), data3, strlen(data3 )) == 0 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_data,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_data_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_key_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

