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
psonHash * pHash;
size_t bucket = (size_t) -1;
psonHashItem * pItem;
char * key1 = "My Key 1";
char * key2 = "My Key 2";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   enum psoErrors errcode;
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   psonHashItem * pHashItem;
   
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
   
   errcode = psonHashInsert( pHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
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

void test_null_bucket( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashGet( pHash,
                                       (unsigned char*)key2,
                                       strlen(key2),
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
   expect_assert_failure( psonHashGet( pHash,
                                       (unsigned char*)key2,
                                       strlen(key2),
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
   expect_assert_failure( psonHashGet( NULL,
                                       (unsigned char*)key2,
                                       strlen(key2),
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
   expect_assert_failure( psonHashGet( pHash,
                                       (unsigned char*)key2,
                                       strlen(key2),
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
   expect_assert_failure( psonHashGet( pHash,
                                       NULL,
                                       strlen(key2),
                                       &pItem,
                                       &bucket,
                                       &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashGet( pHash,
                                       (unsigned char*)key2,
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
   unsigned char* pData = NULL;
   bool found;
   psonHashItem * pHashItem;
   
   found = psonHashGet( pHash,
                        (unsigned char*)key2,
                        strlen(key2),
                        &pItem,
                        &bucket,
                        &context );
   assert_true( found );
   GET_PTR( pData, pItem->dataOffset, unsigned char );
   assert_false( pData == NULL );
   assert_false( pItem->dataLength == 0 );
   assert_false( bucket == (size_t) -1 );
   
   found = psonHashGet( pHash,
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
      unit_test_setup_teardown( test_null_bucket,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

