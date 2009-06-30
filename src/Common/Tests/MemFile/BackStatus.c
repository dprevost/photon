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
   bool ok;
   psocMemoryFileStatus status;

   ok = psocCreateBackstore( &mem, 0644, &errorHandler );
   assert_true( ok );

   mem.name[0] = '\0';
   expect_assert_failure( psocBackStoreStatus( &mem, &status ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int value;
   bool ok;
   psocMemoryFileStatus status;

   ok = psocCreateBackstore( &mem, 0644, &errorHandler );
   assert_true( ok );

   value = mem.initialized;
   mem.initialized = 0;
   expect_assert_failure( psocBackStoreStatus( &mem, &status ) );
   mem.initialized = value;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_mem( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psocMemoryFileStatus status;

   ok = psocCreateBackstore( &mem, 0644, &errorHandler );
   assert_true( ok );
   
   expect_assert_failure( psocBackStoreStatus( NULL, &status ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_status( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;

   ok = psocCreateBackstore( &mem, 0644, &errorHandler );
   assert_true( ok );
   
   expect_assert_failure( psocBackStoreStatus( &mem, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psocMemoryFileStatus status;
   int errcode;
   
   ok = psocCreateBackstore( &mem, 0644, &errorHandler );
   assert_true( ok );
   
   psocBackStoreStatus( &mem, &status );
   assert_true( status.fileExist );
   assert_true( status.fileReadable );
   assert_true( status.fileWritable );
   assert_true( status.lenghtOK );
   assert_true( status.actualLLength == 10*1024 );
   
   errcode = unlink( "MemFile.mem" );
   assert_true( errcode == 0 );
   
   ok = psocCreateBackstore( &mem, 0600, &errorHandler );
   assert_true( ok );
   
   psocBackStoreStatus( &mem, &status );
   assert_true( status.fileExist );
   assert_true( status.fileReadable );
   assert_true( status.fileWritable );

   errcode = unlink( "MemFile.mem" );
   assert_true( errcode == 0 );

   ok = psocCreateBackstore( &mem, 0660, &errorHandler );
   assert_true( ok );
   
   psocBackStoreStatus( &mem, &status );
   assert_true( status.fileExist );
   assert_true( status.fileReadable );
   assert_true( status.fileWritable );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_empty_name,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_invalid_sig, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_mem,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_status, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

