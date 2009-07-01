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
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_handle( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   struct psocOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };
   
   expect_assert_failure( psocSetSupportedOptions( 5, opts, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_opts( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocSetSupportedOptions( 5, NULL, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_opts_zero( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   struct psocOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };
   
   expect_assert_failure( psocSetSupportedOptions( 0, opts, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_repeated_long( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   struct psocOptStruct opts[3] = {
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'v', "address", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "sleep" }
   };

   expect_assert_failure( psocSetSupportedOptions( 3, opts, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_repeated_short( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   struct psocOptStruct opts[3] = {
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'a', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "sleep" }
   };

   expect_assert_failure( psocSetSupportedOptions( 3, opts, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_ended_comment( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int i;
   
   struct psocOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };
   
   for ( i = 0; i < PSOC_OPT_COMMENT_LENGTH; ++i ) {
      opts[2].comment[i] = 'x';
   }
   
   expect_assert_failure( psocSetSupportedOptions( 5, opts, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_ended_opt( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int i;
   
   struct psocOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };
   
   for ( i = 0; i < PSOC_OPT_LONG_OPT_LENGTH; ++i ) {
      opts[2].longOpt[i] = 'x';
   }
   
   expect_assert_failure( psocSetSupportedOptions( 5, opts, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_ended_value( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int i;
   struct psocOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };

   for ( i = 0; i < PSOC_OPT_ARGUMENT_MSG_LENGTH; ++i ) {
      opts[2].argumentMessage[i] = 'x';
   }
   
   expect_assert_failure( psocSetSupportedOptions( 5, opts, &handle ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   struct psocOptStruct opts[5] = { 
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };
   
   ok = psocSetSupportedOptions( 5, opts, &handle );

   psocUnsetSupportedOptions( handle );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_null_handle ),
      unit_test( test_null_opts ),
      unit_test( test_null_opts_zero ),
      unit_test( test_null_repeated_long ),
      unit_test( test_null_repeated_short ),
      unit_test( test_null_ended_comment ),
      unit_test( test_null_ended_opt ),
      unit_test( test_null_ended_value ),
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

