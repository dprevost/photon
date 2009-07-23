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

#include "sessionTest.h"

psonSession * pSession;
psonSessionContext context;
void * pApiObject = (void *) &context; /* A dummy pointer */
ptrdiff_t objOffset;
psonObjectContext * pObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   void * pApiSession = (void *) &context; /* A dummy pointer */
   
   pSession = initSessionTest( &context );

   objOffset = SET_OFFSET( pSession ); /* Dummy offset */

   ok = psonSessionInit( pSession, pApiSession, &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSessionAddObj( pSession,
                                             objOffset, 
                                             0,
                                             pApiObject,
                                             &pObject,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_api_obj( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSessionAddObj( pSession,
                                             objOffset, 
                                             PSO_FOLDER,
                                             NULL,
                                             &pObject,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSessionAddObj( pSession,
                                             objOffset, 
                                             PSO_FOLDER,
                                             pApiObject,
                                             &pObject,
                                             NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_object( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSessionAddObj( pSession,
                                             objOffset, 
                                             PSO_FOLDER,
                                             pApiObject,
                                             NULL,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_offset( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSessionAddObj( pSession,
                                             PSON_NULL_OFFSET,
                                             PSO_FOLDER,
                                             pApiObject,
                                             &pObject,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_session( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonSessionAddObj( NULL,
                                             objOffset, 
                                             PSO_FOLDER,
                                             pApiObject,
                                             &pObject,
                                             &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonSessionAddObj( pSession,
                           objOffset, 
                           PSO_FOLDER,
                           pApiObject,
                           &pObject,
                           &context );
   assert( ok );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_type, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_api_obj, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_object,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_offset,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_session, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

