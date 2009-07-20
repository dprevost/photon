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

#include "processTest.h"

psonProcess * process;
psonSessionContext context;
psonSession * pSession1, * pSession2;
void * pApiSession = (void *) &context; /* A dummy pointer */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   process = initProcessTest( &context );

   ok = psonProcessInit( process, 12345, &context );
   assert( ok );

   ok = psonProcessAddSession( process,
                               pApiSession,
                               &pSession1,
                               &context );
   assert( ok );
   
   ok = psonProcessGetFirstSession( process,
                                    &pSession1,
                                    &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonProcessGetNextSession( process,
                                                     pSession1,
                                                     &pSession2,
                                                     NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_current( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonProcessGetNextSession( process,
                                                     NULL,
                                                     &pSession2,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_next( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonProcessGetNextSession( process,
                                                     pSession1,
                                                     NULL,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_process( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonProcessGetNextSession( NULL,
                                                     pSession1,
                                                     &pSession2,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
      
   /* Only one session - this should fail */
   ok = psonProcessGetNextSession( process,
                                   pSession1,
                                   &pSession2,
                                   &context );
   assert_false( ok );

   ok = psonProcessAddSession( process,
                               pApiSession,
                               &pSession1,
                               &context );
   assert_true( ok );
   
   ok = psonProcessGetFirstSession( process,
                                    &pSession1,
                                    &context );
   assert_true( ok );
   
   ok = psonProcessGetNextSession( process,
                                   pSession1,
                                   &pSession2,
                                   &context );
   assert_true( ok );
   assert_false( pSession1 == pSession2 );
   
   psonProcessFini( process, &context );
                                 
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
      unit_test_setup_teardown( test_null_current, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_next,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_process, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

