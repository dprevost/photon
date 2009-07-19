/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "ListTestCommon.h"
#include "Nucleus/Tests/EngineTestCommon.h"

psonLinkedList list;
psonLinkNode node1, node2, *pNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psonSessionContext context;
   
   initTest( &context );
   InitMem();
   
   psonLinkNodeInit( &node1 );
   psonLinkNodeInit( &node2 );
   psonLinkedListInit( &list );
   
   psonLinkedListPutLast( &list, &node1 );
   psonLinkedListPutLast( &list, &node2 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psonLinkedListFini( &list );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int save = list.initialized;

   list.initialized = 0;
   expect_assert_failure( psonLinkedListPeakNext( &list, &node1, &pNode ) );
   list.initialized = save;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_current( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListPeakNext( &list, NULL, &pNode ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_list( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListPeakNext( NULL, &node1, &pNode ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_next( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkedListPeakNext( &list, &node1, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_next_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   node1.nextOffset = PSON_NULL_OFFSET;
   expect_assert_failure( psonLinkedListPeakNext( &list, &node1, &pNode ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_prev_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   node1.previousOffset = PSON_NULL_OFFSET;
   expect_assert_failure( psonLinkedListPeakNext( &list, &node1, &pNode ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonLinkedListPeakNext( &list, &node1, &pNode );
   assert_true( ok );
   assert_true( pNode == &node2 );
   assert_true( list.currentSize == 2 );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_sig,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_current,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_list,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_next,        setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_next_offset, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_prev_offset, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

