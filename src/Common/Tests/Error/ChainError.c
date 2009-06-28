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

#include "Common/Common.h"
#include "Common/ErrorHandler.h"

psocErrorHandler errorHandler;
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int value;
   
   psocSetError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );

   value = errorHandler.initialized;
   errorHandler.initialized = 0;
   expect_assert_failure( 
      psocChainError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT ) );
   errorHandler.initialized = value;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_no_seterror( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( 
      psocChainError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_def( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocErrorHandler errorHandler2;
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler2 );

   psocSetError( &errorHandler2, PSOC_ERRNO_HANDLE, ENOENT );

   psocFiniErrorDefs();
   expect_assert_failure( 
      psocChainError( &errorHandler2, PSOC_ERRNO_HANDLE, ENOENT ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handler( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocSetError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );

   expect_assert_failure( 
      psocChainError( NULL, PSOC_ERRNO_HANDLE, ENOENT ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_too_many( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int i;
   
   psocSetError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );

   for ( i = 0; i < PSOC_ERROR_CHAIN_LENGTH - 1; ++i ) {      
      psocChainError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );
   }
   expect_assert_failure( 
      psocChainError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT ) );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
         
void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocSetError( &errorHandler, PSOC_ERRNO_HANDLE, ENOENT );

   psocChainError( &errorHandler, PSOC_ERRNO_HANDLE, EINTR );

   assert_true( errorHandler.chainLength == 2 );
   assert_true( errorHandler.errorCode[0] == ENOENT );
   assert_true( errorHandler.errorCode[1] == EINTR );
   assert_true( errorHandler.errorHandle[0] == PSOC_ERRNO_HANDLE );
   assert_true( errorHandler.errorHandle[1] == PSOC_ERRNO_HANDLE );
   
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
      unit_test_setup_teardown( test_null_no_seterror, setup_test, teardown_test ),
      unit_test( test_null_def ),
      unit_test_setup_teardown( test_null_handler,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_too_many,         setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,             setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

