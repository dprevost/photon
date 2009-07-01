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

#include "Common/Options.h"

psocOptionHandle handle;
   
struct psocOptStruct opts[5] = { 
   { '3', "three",   1, "", "repeat the loop three times" },
   { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
   { 'x', "",        1, "DISPLAY", "X display to use" },
   { 'v', "verbose", 1, "", "try to explain what is going on" },
   { 'z', "zzz",     1, "", "go to sleep..." }
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   int errcode;
   
   ok = psocSetSupportedOptions( 5, opts, &handle );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psocUnsetSupportedOptions( handle );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_add_args( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocShowUsage( handle, "MyProgram", NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handle( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocShowUsage( NULL, "MyProgram", "file1 [file2...]" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_program( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocShowUsage( handle, NULL, "file1 [file2...]" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   psocShowUsage( handle, "MyProgram", "file1 [file2...]" );
   psocShowUsage( handle, "", "file1 [file2...]" );
   psocShowUsage( handle, "MyProgram", "" );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_add_args, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_handle,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_program,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,          setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

