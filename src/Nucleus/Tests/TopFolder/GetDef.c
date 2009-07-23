/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/psoCommon.h>

psonFolder * pTopFolder;
psonSessionContext context;
psonKeyDefinition * retKeyDef = NULL;
psonDataDefinition * retDataDef = NULL;
psoObjectDefinition retDef;
psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, 0 };
psonKeyDefinition keyDef;
psonDataDefinition fieldDef;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   
   pTopFolder = initTopFolderTest( &context );

   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &def,
                                   &fieldDef,
                                   &keyDef,
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
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               &retDataDef,
                                               &retKeyDef,
                                               NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_datadef( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               NULL,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_def( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               NULL,
                                               &retDataDef,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( NULL,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               &retDataDef,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_key_def( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               "Test1",
                                               strlen("Test1"),
                                               &retDef,
                                               &retDataDef,
                                               NULL,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTopFolderGetDef( pTopFolder,
                                               NULL,
                                               strlen("Test1"),
                                               &retDef,
                                               &retDataDef,
                                               &retKeyDef,
                                               &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonTopFolderGetDef( pTopFolder,
                             "Test1",
                             strlen("Test1"),
                             &retDef,
                             &retDataDef,
                             &retKeyDef,
                             &context );
   assert_true( ok );
   
   assert_memory_equal( retKeyDef, &keyDef, sizeof(psonKeyDefinition) );
   assert_memory_equal( retDataDef, &fieldDef, sizeof(psonDataDefinition) );
   
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
      unit_test_setup_teardown( test_null_datadef, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_def,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_folder,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_key_def, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_name,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

