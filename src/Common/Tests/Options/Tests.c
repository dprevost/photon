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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode = 0;
   psocOptionHandle handle;
   char dummyArgs[100];
   char *dummyPtrs[10];
   bool ok;
   
   struct psocOptStruct opts[5] = {
      { '3', "three",   1, "", "repeat the loop three times" },
      { 'a', "address", 0, "QUASAR_ADDRESS", "tcp/ip port number of the server" },
      { 'x', "",        1, "DISPLAY", "X display to use" },
      { 'v', "verbose", 1, "", "try to explain what is going on" },
      { 'z', "zzz",     1, "", "go to sleep..." }
   };

   dummyPtrs[0] = dummyArgs;
   
   ok = psocSetSupportedOptions( 5, opts, &handle );
   assert_true( ok );
   
   strcpy( dummyArgs, "OptionTest2 -?" );
   dummyPtrs[1] = &dummyArgs[12];
   dummyArgs[11] = 0;
   
   errcode = psocValidateUserOptions( handle, 2, dummyPtrs, 1 );
   assert_true( errcode == 1 );
   
   strcpy( dummyArgs, "OptionTest2 -a 12345" );
   dummyPtrs[1] = &dummyArgs[12];
   dummyPtrs[2] = &dummyArgs[15];
   dummyArgs[11] = 0;
   dummyArgs[14] = 0;   

   errcode = psocValidateUserOptions( handle, 3, dummyPtrs, 1 );
   assert_true( errcode == 0 );
   
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

