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
psocErrorHandler errorHandler;

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

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   psocInitMemoryFile( &mem, 10, "MemFile.mem" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   unlink( "MemFile.mem" );
   
   psocFiniMemoryFile( &mem );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_empty_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   mem.name[0] = '\0';
   expect_assert_failure( psocCreateBackstore( &mem, 0755, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int value;

   value = mem.initialized;
   mem.initialized = 0;
   expect_assert_failure( psocCreateBackstore( &mem, 0755, &errorHandler ) );
   mem.initialized = value;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   mem.length = 0;
   expect_assert_failure( psocCreateBackstore( &mem, 0755, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_error( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocCreateBackstore( &mem, 0755, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_mem( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocCreateBackstore( NULL, 0755, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_perm_0000( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocCreateBackstore( &mem, 0000, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_perm_0200( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocCreateBackstore( &mem, 0200, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_perm_0400( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocCreateBackstore( &mem, 0400, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_perm_0500( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocCreateBackstore( &mem, 0500, &errorHandler ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   assert_true( ok );
   
   unlink( "MemFile.mem" );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_empty_name,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_invalid_sig,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_invalid_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_error,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_mem,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_perm_0000,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_perm_0200,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_perm_0400,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_perm_0500,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,           setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

