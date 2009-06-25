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

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
//#include <google/cmockery.h>

#include "Common/Common.h"
#include "Common/DirAccess.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

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
#if defined (WIN32)
   if ( iterator.handle != PSO_INVALID_HANDLE ) {
      psocCloseDir( &iterator );
   }
#else
   if ( iterator.pDir != NULL ) {
      psocCloseDir( &iterator );
   }
#endif

   psocFiniDir( &iterator );
   psocFiniErrorHandler( &errorHandler );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(USE_DBC)
   bool ok;
   int value;
   
   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   value = iterator.initialized;
   iterator.initialized = 0;
   expect_assert_failure( psocCloseDir(&iterator) );
   iterator.initialized = value;

   return;
#else
   return;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_dir( void ** state )
{
#if defined(USE_DBC)
   bool ok;

   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   expect_assert_failure( psocCloseDir(NULL) );

   return;
#else
   return;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
   bool ok;
   
   ok = psocOpenDir( &iterator, "..", &errorHandler );
   assert_true( ok );
   
   psocCloseDir( &iterator );

#if defined (WIN32)
   assert_true( iterator.handle == PSO_INVALID_HANDLE );
#else
   assert_true( iterator.pDir == NULL );
#endif

   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main(int argc, char *argv[])
{
   int rc;
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_sig, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_dir,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   psocInitErrorDefs();
   rc = run_tests(tests);
   psocFiniErrorDefs();
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

