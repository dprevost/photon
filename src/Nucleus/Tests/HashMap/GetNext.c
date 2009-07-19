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

#include "hashMapTest.h"

psonHashMap * pHashMap;
psonSessionContext context;
psonTxStatus status;
char * key1  = "my key1";
char * key2  = "my key2";
char * data1 = "my data1";
char * data2 = "my data2";
psonHashMapItem item;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;

   pHashMap = initHashMapTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonHashMapInit( pHashMap, 0, 1, 0, &status, 4, "Map1", 
                         SET_OFFSET(pHashMap), 
                         &def, &keyDef,
                         &fields, &context );
   assert( ok );
   
   ok = psonHashMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data1,
                           8,
                           NULL,
                           &context );
   assert( ok );

   ok = psonHashMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data2,
                           8,
                           NULL,
                           &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
   memset( &item, 0, sizeof(psonHashMapItem) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_no_first( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   expect_assert_failure( psonHashMapGetNext( pHashMap,
                                              &item,
                                              7,
                                              20,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   assert_true( ok );

   expect_assert_failure( psonHashMapGetNext( pHashMap,
                                              &item,
                                              7,
                                              20,
                                              NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;

   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   assert_true( ok );

   expect_assert_failure( psonHashMapGetNext( NULL,
                                              &item,
                                              7,
                                              20,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;

   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   assert_true( ok );

   expect_assert_failure( psonHashMapGetNext( pHashMap,
                                              NULL,
                                              7,
                                              20,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_key_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int errcode;
   
   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   assert_true( ok );

   ok = psonHashMapGetNext( pHashMap,
                            &item,
                            6,
                            20,
                            &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_LENGTH );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int errcode;
   
   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   assert_true( ok );

   ok = psonHashMapGetNext( pHashMap,
                            &item,
                            7,
                            7,
                            &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_LENGTH );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   char * ptr1, * ptr2;

   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   assert_true( ok );
   GET_PTR( ptr1, item.pHashItem->dataOffset, char );

   ok = psonHashMapGetNext( pHashMap,
                            &item,
                            7,
                            20,
                            &context );
   assert_true( ok );
   GET_PTR( ptr2, item.pHashItem->dataOffset, char );
   assert_false( ptr1 == ptr2 );
   
   if (memcmp( data1, ptr1, 8 ) == 0 ) {
      assert_true( memcmp( data2, ptr2, 8 ) == 0 );
   }
   else {
      assert_true( memcmp( data1, ptr2, 8 ) == 0 );
      assert_true( memcmp( data2, ptr1, 8 ) == 0 );
   }
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_no_first,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_key_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_length,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

