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

psonFastMap * pHashMap;
psonSessionContext context;
char * key  = "my key";
psonHashItem * pItem;
char * data2 = "my data2";
psonTxStatus status;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   char * data1 = "my data1";
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;

   pHashMap = initHashMapTest( &context );
   assert( pHashMap );
   
   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFastMapInit( pHashMap, 0, 1, 0, &status, 4, "Map1", 
                         SET_OFFSET(pHashMap), &def, &keyDef, 
                         &fields, &context );
   assert( ok );
   
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key,
                           6,
                           (const void *) data1,
                           strlen(data1),
                           NULL,
                           &context );
   assert( ok );
   
   ok = psonFastMapGet( pHashMap,
                        (const void *) key,
                        6,
                        &pItem,
                        20,
                        &context );

   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
   pHashMap = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapReplace( pHashMap,
                                              (const void *) key,
                                              6,
                                              (const void *) data2,
                                              strlen(data2),
                                              NULL,
                                              NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapReplace( pHashMap,
                                              (const void *) key,
                                              6,
                                              NULL,
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
   expect_assert_failure( psonFastMapReplace( pHashMap,
                                              NULL,
                                              6,
                                              (const void *) data2,
                                              strlen(data2),
                                              NULL,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_map( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapReplace( NULL,
                                              (const void *) key,
                                              6,
                                              (const void *) data2,
                                              strlen(data2),
                                              NULL,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapReplace( pHashMap,
                                              (const void *) key,
                                              6,
                                              (const void *) data2,
                                              0,
                                              NULL,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length_key( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapReplace( pHashMap,
                                              (const void *) key,
                                              0,
                                              (const void *) data2,
                                              strlen(data2),
                                              NULL,
                                              &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psonHashItem * pItem;
   char * ptr;

   ok = psonFastMapReplace( pHashMap,
                            (const void *) key,
                            6,
                            (const void *) data2,
                            strlen(data2),
                            NULL,
                            &context );
   assert_true( ok );
   
   ok = psonFastMapGet( pHashMap,
                        (const void *) key,
                        6,
                        &pItem,
                        20,
                        &context );
   assert_true( ok );
   GET_PTR( ptr, pItem->dataOffset, char );
   assert_true( memcmp( data2, ptr, strlen(data2)) == 0 );
   
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
      unit_test_setup_teardown( test_null_item,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_map,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length_item, setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length_key,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

