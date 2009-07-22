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
psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
psonDataDefinition dataDef;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   pTopFolder = initTopFolderTest( &context );

   ok = psonTopFolderCreateFolder( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
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

void test_invalid_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   int errcode;
   
   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   0,
                                   &def,
                                   &dataDef,
                                   NULL,
                                   &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_INVALID_OBJECT_NAME );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderCreateObject( pTopFolder,
                                                     "Test1/Test2",
                                                     strlen("Test1/Test2"),
                                                     &def,
                                                     &dataDef,
                                                     NULL,
                                                     NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_datadef( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderCreateObject( pTopFolder,
                                                     "Test1/Test2",
                                                     strlen("Test1/Test2"),
                                                     &def,
                                                     NULL,
                                                     NULL,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_definition( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderCreateObject( pTopFolder,
                                                     "Test1/Test2",
                                                     strlen("Test1/Test2"),
                                                     NULL,
                                                     &dataDef,
                                                     NULL,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderCreateObject( NULL,
                                                     "Test1/Test2",
                                                     strlen("Test1/Test2"),
                                                     &def,
                                                     &dataDef,
                                                     NULL,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderCreateObject( pTopFolder,
                                                     NULL,
                                                     strlen("Test1/Test2"),
                                                     &def,
                                                     &dataDef,
                                                     NULL,
                                                     &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoObjectDefinition def2 = { 0, 0, 0, 0 };

   expect_assert_failure( psonTopFolderCreateObject( pTopFolder,
                                                     "Test1/Test2",
                                                     strlen("Test1/Test2"),
                                                     &def2,
                                                     &dataDef,
                                                     NULL,
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
   
   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &def,
                                   &dataDef,
                                   NULL,
                                   &context );
   assert_true( ok );
   
   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test3/Test2",
                                   strlen("Test3/Test2"),
                                   &def,
                                   &dataDef,
                                   NULL,
                                   &context );
   assert_false( ok );
   errcode = psocGetLastError( &context.errorHandler );
   assert_true( errcode == PSO_NO_SUCH_FOLDER );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_datadef,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_definition, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_folder,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,       setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,            setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

