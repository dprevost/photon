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
psonHashTxItem * pNewItem;
size_t bucket = (size_t) -1;
psonHashTxItem * pItem;
char* key1 = "My Key 1";
char* key2 = "My Key 2";
char* data1 = "My Data 1";
char* data2 = "My Data 2";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
 //  assert( ok );
   enum psoErrors errcode;
   unsigned char* pData = NULL;
   bool found;
   
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
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_bucket( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGet( pHash,
                                         (unsigned char*)key1,
                                         strlen(key1),
                                         &pItem,
                                         NULL,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGet( pHash,
                                         (unsigned char*)key1,
                                         strlen(key1),
                                         &pItem,
                                         &bucket,
                                         NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGet( NULL,
                                         (unsigned char*)key1,
                                         strlen(key1),
                                         &pItem,
                                         &bucket,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGet( pHash,
                                         (unsigned char*)key1,
                                         strlen(key1),
                                         NULL,
                                         &bucket,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGet( pHash,
                                         NULL,
                                         strlen(key1),
                                         &pItem,
                                         &bucket,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_key_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashTxGet( pHash,
                                         (unsigned char*)key1,
                                         0,
                                         &pItem,
                                         &bucket,
                                         &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   enum psoErrors errcode;
   unsigned char* pData = NULL;
   bool found;

   found = psonHashTxGet( pHash,
                          (unsigned char*)key2,
                          strlen(key2),
                          &pNewItem,
                          &bucket,
                          &context );
   assert_false( found );
   
   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key2,
                               strlen(key2),
                               data2,
                               strlen(data2),
                               &pNewItem,
                               &context );
   assert_true( errcode == PSO_OK );
   
   found = psonHashTxGet( pHash,
                          (unsigned char*)key2,
                          strlen(key2),
                          &pItem,
                          &bucket,
                          &context );
   assert_true( found );
   GET_PTR( pData, pItem->dataOffset, unsigned char );
   assert_true( pData );
   assert_false( pItem->dataLength == 0 );
   assert_false( bucket == (size_t) -1 );
   
   found = psonHashTxGet( pHash,
                          (unsigned char*)"My Key 3",
                          strlen("My Key 3"),
                          &pItem,
                          &bucket,
                          &context );
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
      unit_test_setup_teardown( test_null_bucket,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_key_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

