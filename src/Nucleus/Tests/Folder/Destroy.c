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

psonFolder * pFolder;
psonSessionContext context;
psoObjectDefinition def = { PSO_FOLDER, 0, 0, 0 };

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   pFolder = initTopFolderTest( &context );

   ok = psonAPIFolderCreateObject( pFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &def,
                                   NULL,
                                   NULL,
                                   &context );
   assert( ok );
   
   psonTxCommit( (psonTx *)context.pTransaction, &context );
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
   expect_assert_failure( psonAPIFolderDestroyObject( pFolder,
                                                      "Test1",
                                                      strlen("Test1"),
                                                      NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonAPIFolderDestroyObject( NULL,
                                                      "Test1",
                                                      strlen("Test1"),
                                                      &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonAPIFolderDestroyObject( pFolder,
                                                      NULL,
                                                      strlen("Test1"),
                                                      &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   bool ok;
   
   /* Using the topfolder to create a grandchild! */
   ok = psonTopFolderCreateFolder( pFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &context );
   assert_true( ok );
   
   psonTxCommit( (psonTx *)context.pTransaction, &context );
   
   ok = psonAPIFolderDestroyObject( pFolder,
                                    "Test1",
                                    strlen("Test1"),
                                    &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_FOLDER_IS_NOT_EMPTY );
                                         
   ok = psonTopFolderDestroyObject( pFolder,
                                    "Test1/Test2",
                                    strlen("Test1/Test2"),
                                    &context );
   assert_true( ok );
   
   ok = psonAPIFolderDestroyObject( pFolder,
                                 "Test2",
                                 strlen("Test2"),
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_NO_SUCH_OBJECT );

   ok = psonAPIFolderDestroyObject( pFolder,
                                 "Test1",
                                 PSO_MAX_NAME_LENGTH+1,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_OBJECT_NAME_TOO_LONG );

   ok = psonAPIFolderDestroyObject( pFolder,
                                    "Test1",
                                    0,
                                    &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );

   ok = psonAPIFolderDestroyObject( pFolder,
                                    "Test1",
                                    strlen("Test1"),
                                    &context );
   assert_true( ok );
   
   /* Calling destroy on the same object, twice */
   ok = psonAPIFolderDestroyObject( pFolder,
                                 "Test1",
                                 strlen("Test1"),
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_OBJECT_IS_IN_USE );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_folder,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

