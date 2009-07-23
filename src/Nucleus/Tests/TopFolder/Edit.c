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
   
   pTopFolder = initTopFolderTest( &context );

   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition key;
   psonDataDefinition fields;

   ok = psonTopFolderCreateFolder( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &context );
   assert( ok );
   
   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &mapDef,
                                   &fields,
                                   &key,
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

void test_invalid_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderEditObject( pTopFolder,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_FAST_MAP+12345,
                                                   &folderItem,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderEditObject( pTopFolder,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_FAST_MAP,
                                                   &folderItem,
                                                   NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderEditObject( NULL,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_FAST_MAP,
                                                   &folderItem,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderEditObject( pTopFolder,
                                                   "Test1/Test2",
                                                   strlen("Test1/Test2"),
                                                   PSO_FAST_MAP,
                                                   NULL,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderEditObject( pTopFolder,
                                                   NULL,
                                                   strlen("Test1/Test2"),
                                                   PSO_FAST_MAP,
                                                   &folderItem,
                                                   &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   bool ok;

   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test2",
                                 PSO_MAX_FULL_NAME_LENGTH+1,
                                 PSO_FAST_MAP,
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
   int errcode;
   bool ok;

   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test2",
                                 strlen("Test1/Test2"),
                                 PSO_FOLDER,
                                 &folderItem,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_WRONG_OBJECT_TYPE );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   bool ok;

   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test2",
                                 0,
                                 PSO_FAST_MAP,
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

   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test2",
                                 strlen("Test1/Test2"),
                                 PSO_FAST_MAP,
                                 &folderItem,
                                 &context );
   assert_true( ok );
   
   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test3/Test2",
                                 strlen("Test3/Test2"),
                                 PSO_FAST_MAP,
                                 &folderItem,
                                 &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_NO_SUCH_FOLDER );
   
   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test5",
                                 strlen("Test1/Test5"),
                                 PSO_FAST_MAP,
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
      unit_test_setup_teardown( test_invalid_type, setup_test, teardown_test ),
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

