/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "folderTest.h"

psonFolder* pFolder;
psonSessionContext context;
psonTxStatus status;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   pFolder = initFolderTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
   pFolder = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          0,
                                          1,
                                          0,
                                          &status,
                                          5,
                                          "Test1",
                                          1234,
                                          NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( NULL, 
                                          0,
                                          1,
                                          0,
                                          &status,
                                          5,
                                          "Test1",
                                          1234,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item_off( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          0,
                                          1,
                                          0,
                                          &status,
                                          5,
                                          "Test1",
                                          PSON_NULL_OFFSET,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          0,
                                          1,
                                          0,
                                          &status,
                                          5,
                                          NULL,
                                          1234,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_parent_off( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          PSON_NULL_OFFSET,
                                          1,
                                          0,
                                          &status,
                                          5,
                                          "Test1",
                                          1234,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_status( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          0,
                                          1,
                                          0,
                                          NULL,
                                          5,
                                          "Test1",
                                          1234,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_blocks( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          0,
                                          0,
                                          0,
                                          &status,
                                          5,
                                          "Test1",
                                          1234,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderInit( pFolder, 
                                          0,
                                          1,
                                          0,
                                          &status,
                                          0,
                                          "Test1",
                                          1234,
                                          &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   assert_true( ok );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_folder,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_item_off,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_parent_off, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_status,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_blocks,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

