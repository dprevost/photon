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
psonHashMapItem item;
char * key  = "my key";
char * data = "my data";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   char * ptr;
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
                           (const void *) key,
                           6,
                           (const void *) data,
                           7,
                           NULL,
                           &context );
   assert( ok );
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
   expect_assert_failure( psonHashMapGetFirst( pHashMap,
                                               &item,
                                               6,
                                               20,
                                               NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapGetFirst( NULL,
                                               &item,
                                               6,
                                               20,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashMapGetFirst( pHashMap,
                                               NULL,
                                               6,
                                               20,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_key_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   bool ok;

   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             5,
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
   int errcode;
   bool ok;

   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             6,
                             6,
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
   char * ptr;

   ok = psonHashMapGetFirst( pHashMap,
                             &item,
                             6,
                             20,
                             &context );
   assert_true( ok );
   GET_PTR( ptr, item.pHashItem->dataOffset, char );
   assert_true( memcmp( data, ptr, 7 ) == 0 );
   
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

