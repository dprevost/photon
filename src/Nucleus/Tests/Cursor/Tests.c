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
psonCursorItem * pItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   size_t numItems;
   
   pCursor = initCursorTest( &context );

   ok = psonCursorInit( pCursor,
                        12345,
                        1,
                        &context );
   assert_true( ok );

   /* Number of items at start */
   psonCursorSize( pCursor, &numItems );
   assert_true( numItems == 0 );

   /* Emptying an empty cursor */
   psonCursorEmpty( pCursor, &context );
   
   ok = psonCursorGetFirst( pCursor, &pItem, &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_IS_EMPTY );
   
   ok = psonCursorGetLast( pCursor, &pItem, &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_IS_EMPTY );
   
   /* Add one element */
   ok = psonCursorInsertFirst( pCursor,
                               (unsigned char *)0x2, // any pointer is ok
                               PSON_HASH_ITEM,
                               &context );
   assert_true( ok );

   ok = psonCursorGetFirst( pCursor, &pItem, &context );
   assert_true( ok );

   ok = psonCursorGetNext( pCursor, pItem, &pItem, &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_REACHED_THE_END );

   ok = psonCursorGetLast( pCursor, &pItem, &context );
   assert_true( ok );

   ok = psonCursorGetPrevious( pCursor, pItem, &pItem, &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_REACHED_THE_END );

   /* Add a second element  */
   ok = psonCursorInsertLast( pCursor,
                              (unsigned char *)0x3, // any pointer is ok
                              PSON_HASH_ITEM,
                              &context );
   assert_true( ok );

   ok = psonCursorGetFirst( pCursor, &pItem, &context );
   assert_true( ok );
   assert_true( pItem->itemOffset == SET_OFFSET((unsigned char *)0x2) );

   ok = psonCursorGetNext( pCursor, pItem, &pItem, &context );
   assert_true( ok );
   assert_true( pItem->itemOffset == SET_OFFSET((unsigned char *)0x3) );

   ok = psonCursorGetPrevious( pCursor, pItem, &pItem, &context );
   assert_true( ok );
   assert_true( pItem->itemOffset == SET_OFFSET((unsigned char *)0x2) );
   
   /* Add a third element  */
   ok = psonCursorInsertFirst( pCursor,
                               (unsigned char *)0x1, // any pointer is ok
                               PSON_HASH_ITEM,
                               &context );
   assert_true( ok );

   ok = psonCursorGetPrevious( pCursor, pItem, &pItem, &context );
   assert_true( ok );
   assert_true( pItem->itemOffset == SET_OFFSET((unsigned char *)0x1) );

   ok = psonCursorGetLast( pCursor, &pItem, &context );
   assert_true( ok );
   assert_true( pItem->itemOffset == SET_OFFSET((unsigned char *)0x3) );

   /* Number of items */
   psonCursorSize( pCursor, &numItems );
   assert_true( numItems == 3 );

   /* Emptying the cursor */
   psonCursorEmpty( pCursor, &context );
   psonCursorSize( pCursor, &numItems );
   assert_true( numItems == 0 );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test1 ),
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

