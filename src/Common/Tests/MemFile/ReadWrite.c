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

void test_invalid_addr( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   void * pAddr = NULL;
   bool ok;
   
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &mem, &pAddr, &errorHandler );
   assert_true( ok );

   mem.baseAddr = PSO_MAP_FAILED;
   expect_assert_failure( psocSetReadWrite( &mem, &errorHandler ) );
   
   psocCloseMemFile( &mem, &errorHandler );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   void * pAddr = NULL;
   bool ok;
   int value;
   
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &mem, &pAddr, &errorHandler );
   assert_true( ok );

   value = mem.initialized;
   mem.initialized = 0;
   expect_assert_failure( psocSetReadWrite( &mem, &errorHandler ) );
   mem.initialized = value;
   
   psocCloseMemFile( &mem, &errorHandler );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_error( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   void * pAddr = NULL;
   bool ok;
   
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &mem, &pAddr, &errorHandler );
   assert_true( ok );

   expect_assert_failure( psocSetReadWrite( &mem, NULL ) );
   
   psocCloseMemFile( &mem, &errorHandler );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_mem( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   void * pAddr = NULL;
   bool ok;
   
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &mem, &pAddr, &errorHandler );
   assert_true( ok );

   expect_assert_failure( psocSetReadWrite( NULL, &errorHandler ) );
   
   psocCloseMemFile( &mem, &errorHandler );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   void * pAddr = NULL;
   bool ok;
   
   ok = psocCreateBackstore( &mem, 0755, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &mem, &pAddr, &errorHandler );
   assert_true( ok );
   
   ok = psocSetReadWrite( &mem, &errorHandler );
   assert_true( ok );
   
   ((char*)pAddr)[0] = 'x';
   ((char*)pAddr)[1] = 'y';
   
   psocCloseMemFile( &mem, &errorHandler );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_addr, setup_test, teardown_test ),
      unit_test_setup_teardown( test_invalid_sig,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_error,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_mem,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

