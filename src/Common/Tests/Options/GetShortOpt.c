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

char dummyArgs[100];
char *dummyPtrs[10];
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
   
   strcpy( dummyArgs, "OptionTest2 --address 12345 -v --zzz" );
   /*                  012345678901234567890123456789012345 */
   dummyPtrs[0] = dummyArgs;
   dummyPtrs[1] = &dummyArgs[12];
   dummyPtrs[2] = &dummyArgs[22];
   dummyPtrs[3] = &dummyArgs[28];
   dummyPtrs[4] = &dummyArgs[31];

   dummyArgs[11] = 0;
   dummyArgs[21] = 0;
   dummyArgs[27] = 0;
   dummyArgs[30] = 0;

   errcode = psocValidateUserOptions( handle, 5, dummyPtrs, 1 );
   assert( errcode == 0 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psocUnsetSupportedOptions( handle );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handle( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   char *value = NULL;

   expect_assert_failure( psocGetShortOptArgument( NULL, 'a', &value ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_value( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocGetShortOptArgument( handle, 'a', NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool gotIt;
   char *value = NULL;
   
   /* Option + value are present */
   gotIt = psocGetShortOptArgument( handle, 'a', &value );
   assert_true( gotIt );
   
   /* Option is present but takes no value */
   gotIt = psocGetShortOptArgument( handle, 'v', &value );
   assert_false( gotIt );
   
   /* Unknown option */
   gotIt = psocGetShortOptArgument( handle, 'q', &value );
   assert_false( gotIt );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_handle, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_value,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

