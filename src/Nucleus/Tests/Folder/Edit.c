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
psonTxStatus status;
psonFolderItem folderItem;

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
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                5,
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
   expect_assert_failure( psonFolderEditObject( NULL,
                                                "test2",
                                                5,
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
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                5,
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
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                NULL,
                                                5,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   pFolder->memObject.objType = PSON_IDENT_HASH_MAP;
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                5,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonFolderEditObject( pFolder,
                                                "test2",
                                                0,
                                                PSO_FAST_MAP,
                                                &folderItem,
                                                &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psonObjectDescriptor * pDescriptor;
   psonTxStatus * txItemStatus;
   psonTreeNode * pNode;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition key;

   psonDataDefinition fields;

   ok = psonFolderEditObject( pFolder,
                              "test2",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert_true( ok );
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psonObjectDescriptor );
   assert_true( memcmp( pDescriptor->originalName, 
                        "Test2", 5*sizeof(char) ) == 0 );
   GET_PTR( pNode, pDescriptor->nodeOffset, psonTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, psonTxStatus );
   assert_true( txItemStatus->parentCounter == 1 );
   assert_true( status.usageCounter == 1 );
   
   ok = psonFolderEditObject( pFolder,
                              "test3",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_NO_SUCH_OBJECT );
   
   ok = psonFolderInsertObject( pFolder,
                                "test4",
                                "Test4",
                                5,
                                &mapDef,
                                &fields,
                                &key,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderEditObject( pFolder,
                              "test4",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   assert_true( ok );
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psonObjectDescriptor );
   assert_true( memcmp( pDescriptor->originalName, 
                        "Test4", 5*sizeof(char) ) == 0 );
   GET_PTR( pNode, pDescriptor->nodeOffset, psonTreeNode);
   GET_PTR( txItemStatus, pNode->txStatusOffset, psonTxStatus );
   assert_true( txItemStatus->parentCounter == 1 );
   assert_true( status.usageCounter == 2 );
   assert_true( pFolder->nodeObject.txCounter == 4 );
   
   psonFolderFini( pFolder, &context );
   
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
      unit_test_setup_teardown( test_wrong_type,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

