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

#include "Nucleus/LinkNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_init( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkNodeInit( NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_test( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonLinkNodeTest( NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_test( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psonLinkNode node;

   psonLinkNodeInit( &node );

   ok = psonLinkNodeTest( &node );
   assert_false( ok );
   
   node.nextOffset = 0x1234;
   ok = psonLinkNodeTest( &node );
   assert_false( ok );
   
   psonLinkNodeInit( &node );

   node.previousOffset = 0x1234;
   ok = psonLinkNodeTest( &node );
   assert_false( ok );
   
   node.nextOffset = 0x1234;
   ok = psonLinkNodeTest( &node );
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
      unit_test( test_null_init ),
      unit_test( test_null_test ),
      unit_test( test_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

