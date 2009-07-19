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
char* key = "My Key 1";
char* data1 = "My Data 1";
psonHashTxItem * pNewItem;
size_t bucket;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   enum psoErrors errcode;
   bool found;
   
   pHash = initHashTest( &context );
   
   errcode = psonHashTxInit( pHash, g_memObjOffset, 100, &context );
   assert( errcode == PSO_OK );
   
   found = psonHashTxGet( pHash,
                        (unsigned char*)key,
                        strlen(key),
                        &pNewItem,
                        &bucket,
                        &context );
   assert( !found );
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
   expect_assert_failure( psonHashTxInsert( pHash,
                                            bucket,
                                            (unsigned char*)key,
                                            strlen(key),
                                            data1,
                                            strlen(data1),
                                            &pNewItem,
                                            NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_data( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxInsert( pHash,
                                            bucket,
                                            (unsigned char*)key,
                                            strlen(key),
                                            NULL,
                                            strlen(data1),
                                            &pNewItem,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxInsert( NULL,
                                            bucket,
                                            (unsigned char*)key,
                                            strlen(key),
                                            data1,
                                            strlen(data1),
                                            &pNewItem,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxInsert( pHash,
                                            bucket,
                                            NULL,
                                            strlen(key),
                                            data1,
                                            strlen(data1),
                                            &pNewItem,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_new_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxInsert( pHash,
                                            bucket,
                                            (unsigned char*)key,
                                            strlen(key),
                                            data1,
                                            strlen(data1),
                                            NULL,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_data_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxInsert( pHash,
                                            bucket,
                                            (unsigned char*)key,
                                            strlen(key),
                                            data1,
                                            0,
                                            &pNewItem,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_key_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxInsert( pHash,
                                            bucket,
                                            (unsigned char*)key,
                                            0,
                                            data1,
                                            strlen(data1),
                                            &pNewItem,
                                            &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   enum psoErrors errcode;
   
   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key,
                               strlen(key),
                               data1,
                               strlen(data1),
                               &pNewItem,
                               &context );
   assert_true( errcode == PSO_OK );
   assert_false( pNewItem == NULL );
   
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
      unit_test_setup_teardown( test_null_key,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_new_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_data_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_key_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

