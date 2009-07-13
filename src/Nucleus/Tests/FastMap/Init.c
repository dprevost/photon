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
psonTxStatus status;
psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
psonKeyDefinition keyDef;
psonDataDefinition fields;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   pHashMap = initHashMapTest( &context );
   assert( pHashMap );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
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
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_datadef( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           NULL,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_definition( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           NULL,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hashitem( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           PSON_NULL_OFFSET,
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_keydef( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           NULL,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_map( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( NULL, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           NULL,
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_parent( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           PSON_NULL_OFFSET, 
                                           1, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_status( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           NULL, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_blocks( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           0, 
                                           0, 
                                           &status, 
                                           4, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFastMapInit( pHashMap, 
                                           0, 
                                           1, 
                                           0, 
                                           &status, 
                                           0, 
                                           "Map1",
                                           SET_OFFSET(pHashMap),
                                           &def,
                                           &keyDef,
                                           &fields,
                                           &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonFastMapInit( pHashMap, 
                         0, 
                         1, 
                         0, 
                         &status, 
                         4, 
                         "Map1",
                         SET_OFFSET(pHashMap),
                         &def,
                         &keyDef,
                         &fields,
                         &context );
   assert_true( ok );
   
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
      unit_test_setup_teardown( test_null_datadef, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_definition, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_hashitem,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_keydef,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_map,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_parent,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_status,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_blocks,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

