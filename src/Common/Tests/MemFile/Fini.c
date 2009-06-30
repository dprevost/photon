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

#include "Common/MemoryFile.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

psocMemoryFile  mem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   /*
    * The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    *
    * Note: revisit this - it was done some time ago (non-ntfs win32?) on
    * a pc which is now dead.
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );

   psocInitMemoryFile( &mem, 10, "MemFile.mem" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   unlink( "MemFile.mem" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int value;
   
   value = mem.initialized;
   mem.initialized = 0;
   expect_assert_failure( psocFiniMemoryFile( &mem ) );

   mem.initialized = value;
   psocFiniMemoryFile( &mem );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_mem( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocFiniMemoryFile( NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)

   psocFiniMemoryFile( &mem );

   assert_true( mem.initialized == 0 );
   assert_true( mem.name[0] == 0 );
   assert_true( mem.length  == 0 );
   assert_true( mem.baseAddr == PSO_MAP_FAILED );
   assert_true( mem.fileHandle == PSO_INVALID_HANDLE );
   
#if defined (WIN32)
   assert_true( mem.mapHandle == PSO_INVALID_HANDLE );
#endif
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_sig,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_mem,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,           setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

