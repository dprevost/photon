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
psonMemObject * pOldMemObj = NULL;
psonFolderItem folderItem;
psonTxStatus status;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition key;

   psonDataDefinition fields;

   pFolder = initFolderTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   assert( ok );
   
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &mapDef,
                                &fields,
                                &key,
                                1,
                                0,
                                &context );
   assert( ok );
   
   ok = psonFolderEditObject( pFolder,
                              "test2",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert( ok );
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
   expect_assert_failure( psonFolderCommitEdit( pFolder,
                                                folderItem.pHashItem, 
                                                PSON_IDENT_MAP,
                                                &pOldMemObj,
                                                NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_folder( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderCommitEdit( NULL,
                                                folderItem.pHashItem, 
                                                PSON_IDENT_MAP,
                                                &pOldMemObj,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderCommitEdit( pFolder,
                                                NULL, 
                                                PSON_IDENT_MAP,
                                                &pOldMemObj,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_mem_obj( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderCommitEdit( pFolder,
                                                folderItem.pHashItem, 
                                                PSON_IDENT_MAP,
                                                NULL,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderCommitEdit( pFolder,
                                                folderItem.pHashItem, 
                                                PSON_IDENT_QUEUE,
                                                &pOldMemObj,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)

   psonFolderCommitEdit( pFolder,
                         folderItem.pHashItem, 
                         PSON_IDENT_MAP,
                         &pOldMemObj,
                         &context );

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
      unit_test_setup_teardown( test_null_mem_obj, setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

