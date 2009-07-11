/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
psonFastMap * pOldMap, * pNewMap;
psonSessionContext context;
psonHashTxItem   hashItem;
char * key1  = "my key1";
char * key2  = "my key2";
char * data1 = "my data1";
char * data2 = "my data2";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;
   
   initHashMapCopyTest( &pOldMap, &pNewMap, &context );

   psonTxStatusInit( &hashItem.txStatus, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFastMapInit( pOldMap, 0, 1, 0, &hashItem.txStatus, 4, "Map1", 
                         SET_OFFSET(pOldMap), &def, &keyDef, 
                         &fields, &context );
   assert( ok );
   
   ok = psonFastMapInsert( pOldMap,
                           (const void *) key1,
                           7,
                           (const void *) data1,
                           8,
                           NULL,
                           &context );
   assert( ok );
   ok = psonFastMapInsert( pOldMap,
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
   pOldMap = pNewMap = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           pNewMap,
                                           &hashItem,
                                           "Map1",
                                           NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           pNewMap,
                                           NULL,
                                           "Map1",
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           pNewMap,
                                           &hashItem,
                                           NULL,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_new_map( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( pOldMap, 
                                           NULL,
                                           &hashItem,
                                           "Map1",
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_old_map( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapCopy( NULL, 
                                           pNewMap,
                                           &hashItem,
                                           "Map1",
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   char * ptr;
   psonHashItem * pItem;
   
   ok = psonFastMapCopy( pOldMap, 
                         pNewMap,
                         &hashItem,
                         "Map1",
                         &context );
   
   ok = psonFastMapGet( pNewMap,
                        (const void *) key1,
                        7,
                        &pItem,
                        20,
                        &context );
   assert_true( ok );
   GET_PTR( ptr, pItem->dataOffset, char );
   assert_memory_equal( data1, ptr, 8 );

   ok = psonFastMapGet( pNewMap,
                        (const void *) key2,
                        7,
                        &pItem,
                        20,
                        &context );
   assert_true( ok );
   GET_PTR( ptr, pItem->dataOffset, char );
   assert_memory_equal( data2, ptr, 8 );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hash_item, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_new_map,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_old_map,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

