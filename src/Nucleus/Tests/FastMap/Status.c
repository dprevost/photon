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
   psonTxStatus txStatus;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * key3  = "my key3";
   char * data  = "my data";
   psoObjStatus status;
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psonTxStatusInit( &txStatus, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFastMapInit( pHashMap, 0, 1, 0, &txStatus, 4, "Map1", 
                         SET_OFFSET(pHashMap), &def, &keyDef,
                         &fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data,
                           7,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data,
                           7,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key3,
                           7,
                           (const void *) data,
                           7,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonFastMapStatus( pHashMap, &status );

   if ( status.numDataItem != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
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

