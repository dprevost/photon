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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure(  );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonFastMap * pHashMap;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   psonFastMapItem item;
   char * ptr1, * ptr2;
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFastMapInit( pHashMap, 0, 1, 0, &status, 4, "Map1", 
                         SET_OFFSET(pHashMap), &def, &keyDef, 
                         &fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data1,
                           8,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data2,
                           8,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonFastMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( ptr1, item.pHashItem->dataOffset, char );

   ok = psonFastMapGetNext( pHashMap,
                            &item,
                            7,
                            20,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( ptr2, item.pHashItem->dataOffset, char );
   if ( ptr1 == ptr2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   if (memcmp( data1, ptr1, 8 ) == 0 ) {
      if (memcmp( data2, ptr2, 8 ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else {
      if (memcmp( data1, ptr2, 8 ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      if (memcmp( data2, ptr1, 8 ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
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
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_cursor,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

