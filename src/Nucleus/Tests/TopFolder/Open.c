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

psonFolder * pTopFolder;
psonSessionContext context;
psonFolderItem folderItem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psonDataDefinition dataDef;
   
   pTopFolder = initTopFolderTest( &context );
   
   ok = psonTopFolderCreateFolder( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &context );
   assert( ok );
   
   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &def,
                                   &dataDef,
                                   NULL,
                                   &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderOpenObject( pTopFolder,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_QUEUE,
                                                   &folderItem,
                                                   NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderOpenObject( NULL,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_QUEUE,
                                                   &folderItem,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderOpenObject( pTopFolder,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_QUEUE,
                                                   NULL,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderOpenObject( pTopFolder,
                                                   NULL,
                                                   strlen("Test1/Test2"),
                                                   PSO_QUEUE,
                                                   &folderItem,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int errcode;
   
   ok = psonTopFolderOpenObject( pTopFolder,
                                 "Test1/Test2",
                                 PSO_MAX_FULL_NAME_LENGTH+1,
                                 PSO_QUEUE,
                                 &folderItem,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_OBJECT_NAME_TOO_LONG );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderOpenObject( pTopFolder,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   0,
                                                   &folderItem,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int errcode;
   
   ok = psonTopFolderOpenObject( pTopFolder,
                                 "Test1/Test2",
                                 0,
                                 PSO_QUEUE,
                                 &folderItem,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   bool ok;

   ok = psonTopFolderOpenObject( pTopFolder,
                                 "Test1/Test2",
                                 strlen("Test1/Test2"),
                                 PSO_QUEUE,
                                 &folderItem,
                                 &context );
   assert_true( ok );
   
   ok = psonTopFolderOpenObject( pTopFolder,
                                 "Test3/Test2",
                                 strlen("Test3/Test2"),
                                 PSO_QUEUE,
                                 &folderItem,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_NO_SUCH_FOLDER );
   
   ok = psonTopFolderOpenObject( pTopFolder,
                                 "Test1/Test5",
                                 strlen("Test1/Test5"),
                                 PSO_QUEUE,
                                 &folderItem,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_NO_SUCH_OBJECT );
   
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
      unit_test_setup_teardown( test_null_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

