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

#include "Common/Common.h"
#include <photon/photon.h>
#include "API/Connector.h"
#include "Nucleus/InitEngine.h"

psoaConnector connector;
const char * address = "10701";
struct qsrOutput answer;
psocErrorHandler errorHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   ok = psonInitEngine();
   assert( ok );
   
   psocInitErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_address( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaConnect( &connector,
                                       NULL,
                                       &answer,
                                       &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_answer( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaConnect( &connector,
                                       address,
                                       NULL,
                                       &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_connector( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaConnect( NULL,
                                       address,
                                       &answer,
                                       &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_error( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaConnect( &connector,
                                       address,
                                       &answer,
                                       NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;

   errcode = psoaConnect( &connector,
                          address,
                          &answer,
                          &errorHandler );

   assert_true( errcode == 0 );
   
   psoaDisconnect( &connector, &errorHandler );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_address,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_answer,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_connector, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_error,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,           setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

