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
#include "Common/ThreadLock.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocThreadLock lock;
   bool ok;
   
   ok = psocInitThreadLock( &lock );
   assert_true( ok );
   
   ok = psocTryAcquireThreadLock( &lock, 0 );
   assert_true( ok );
   
   /* 
    * On Windows, Critical Sections are recursives (for the calling thread,
    * obviously...) while the simplest Posix locks are not.
    *
    * Quite frankly, in retrospect this test seems quite useless but it 
    * does not hurt so...
    */
   ok = psocTryAcquireThreadLock( &lock, 1000 );
#if defined (WIN32)
   assert_true( ok );
   psocReleaseThreadLock( &lock );
#else
   assert_false( ok );
#endif

   psocReleaseThreadLock( &lock );

   ok = psocTryAcquireThreadLock( &lock, 1000 );
   assert_true( ok );
   
   psocReleaseThreadLock( &lock );

   psocFiniThreadLock( &lock );

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

