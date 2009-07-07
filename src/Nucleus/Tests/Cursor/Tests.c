/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "cursorTest.h"

psonCursor * pCursor;
psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   pCursor = initCursorTest( &context );

   ok = psonCursorInit( pCursor,
                        12345,
                        1,
                        &context );
   assert( ok );

#if 0

   ok = psonCursorInsertLast( pCursor,
                           (const void *) key,
                           6,
                           (const void *) data,
                           7,
                           NULL,
                           &context );
   assert( ok );
   
   /* Is the item there? */
   ok = psonFastMapGet( pHashMap,
                        (const void *) key,
                        6,
                        &pItem,
                        20,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psonCursorFini( pCursor, &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_cursor( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonCursorEmpty( NULL, &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   size_t numItems;
   psonCursorItem * pItem;
   
   pCursor = initCursorTest( &context );

   ok = psonCursorInit( pCursor,
                        12345,
                        1,
                        &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Number of items at start */
   psonCursorSize( pCursor, &numItems );
   if ( numItems != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Emptying an empty cursor */
   psonCursorEmpty( pCursor, &context );
   
   ok = psonCursorGetFirst( pCursor, &pItem, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( psocGetLastError( &context.errorHandler ) != PSO_IS_EMPTY ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonCursorGetLast( pCursor, &pItem, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( psocGetLastError( &context.errorHandler ) != PSO_IS_EMPTY ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Add one element */
   ok = psonCursorInsertFirst( pCursor,
                               (unsigned char *)0x2, // any pointer is ok
                               PSON_HASH_ITEM,
                               &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonCursorGetFirst( pCursor, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonCursorGetNext( pCursor, pItem, &pItem, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( psocGetLastError( &context.errorHandler ) != PSO_REACHED_THE_END ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonCursorGetLast( pCursor, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonCursorGetPrevious( pCursor, pItem, &pItem, &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( psocGetLastError( &context.errorHandler ) != PSO_REACHED_THE_END ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Add a second element  */
   ok = psonCursorInsertLast( pCursor,
                              (unsigned char *)0x3, // any pointer is ok
                              PSON_HASH_ITEM,
                              &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonCursorGetFirst( pCursor, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->itemOffset != SET_OFFSET((unsigned char *)0x2) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   ok = psonCursorGetNext( pCursor, pItem, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->itemOffset != SET_OFFSET((unsigned char *)0x3) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   ok = psonCursorGetPrevious( pCursor, pItem, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->itemOffset != SET_OFFSET((unsigned char *)0x2) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Add a third element  */
   ok = psonCursorInsertFirst( pCursor,
                               (unsigned char *)0x1, // any pointer is ok
                               PSON_HASH_ITEM,
                               &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonCursorGetPrevious( pCursor, pItem, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->itemOffset != SET_OFFSET((unsigned char *)0x1) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   ok = psonCursorGetLast( pCursor, &pItem, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->itemOffset != SET_OFFSET((unsigned char *)0x3) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Number of items */
   psonCursorSize( pCursor, &numItems );
   if ( numItems != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Emptying the cursor */
   psonCursorEmpty( pCursor, &context );
   psonCursorSize( pCursor, &numItems );
   if ( numItems != 0 ) {
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
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

