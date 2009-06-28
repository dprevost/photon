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
#include "Common/DirAccess.h"

psocDirIterator  iterator;
psocErrorHandler errorHandler;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psocFiniDir( &iterator );
   psocFiniErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int value;

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   value = iterator.initialized;
   iterator.initialized = 0;
   expect_assert_failure( psocDirGetNextFileName(&iterator, &errorHandler) );
   iterator.initialized = value;

   psocCloseDir( &iterator );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_no_open( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocDirGetNextFileName(&iterator, &errorHandler) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_no_such_dir( void ** state )
{
   /* For non-win32 platforms (linux, Unix), the test for a missing
    * directory is done when the directory is open. On Windows, the
    * handle to the OS iterator is returned when retrieving the name
    * of the first file (FindFirst).
    *
    * In other words this test is only for Windows.
    */
#if defined(PSO_UNIT_TESTS) && defined(WIN32)
   bool ok;
   const char* str;

   ok = psocOpenDir( &iterator, "abc123", &errorHandler );
   assert_true( ok );


   str = psocDirGetNextFileName( &iterator, &errorHandler );

   assert_true( str == NULL );
   assert_true( psocAnyErrors( &errorHandler ) );
   
   psocCloseDir( &iterator );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_dir( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   expect_assert_failure( psocDirGetNextFileName(NULL, &errorHandler) );

   psocCloseDir( &iterator );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_error( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   expect_assert_failure( psocDirGetNextFileName(&iterator, NULL) );

   psocCloseDir( &iterator );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_pdir( void ** state )
{
   /* pDir () is used in Unix/linux, not on Windows. */
#if defined(PSO_UNIT_TESTS) && ! defined(WIN32)
   bool ok;
   DIR * pDir;

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   pDir = iterator.pDir;
   iterator.pDir = NULL;
   expect_assert_failure( psocDirGetNextFileName(&iterator, &errorHandler) );
   iterator.pDir = pDir;

   psocCloseDir( &iterator );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   const char* str;
   
   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );

   assert_true( str != NULL );
   assert_false( psocAnyErrors( &errorHandler ) );
   
   psocCloseDir( &iterator );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_sig, setup_test, teardown_test ),
      unit_test_setup_teardown( test_no_open,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_no_such_dir, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_dir,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_error,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_pdir,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   psocInitErrorDefs();
   rc = run_tests(tests);
   psocFiniErrorDefs();
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

