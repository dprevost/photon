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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonFolder* pFolder1, *pFolder2;

   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   psonObjectDescriptor* pDescriptor = NULL;
   psonFolderItem folderItem;
   psoObjectDefinition def = { PSO_FOLDER, 0, 0, 0 };
   
   /* Create "/" */
   pFolder1 = initFolderTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFolderInit( pFolder1, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   assert_true( ok );
   
   /* Create "/Test2" */   
   ok = psonFolderInsertObject( pFolder1,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );

   /* Try to create "/Test2" again - must fail */   
   ok = psonFolderInsertObject( pFolder1,
                                "test2",
                                "Test5",
                                5,
                                &def,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_OBJECT_ALREADY_PRESENT );
   
   /* Create "/Test3" */   
   ok = psonFolderInsertObject( pFolder1,
                                "test3",
                                "Test3",
                                5,
                                &def,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );

   /* Get "/Test2" */   
   ok = psonFolderGetObject( pFolder1,
                             "test2",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   assert_true( ok );
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psonObjectDescriptor );
   GET_PTR( pFolder2, pDescriptor->offset, psonFolder );

   /* Create "/Test2/Test4" from "/Test2" */   
   ok = psonFolderInsertObject( pFolder2,
                                "test4",
                                "Test4",
                                5,
                                &def,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );

   /* Create "/Test2/Test2" */   
   ok = psonFolderInsertObject( pFolder2,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   psonTxCommit( (psonTx *)context.pTransaction, &context );
   
   /* Try to delete "/Test2" - should fail (not empty) */
   ok = psonFolderDeleteObject( pFolder1,
                                "test2",
                                5,
                                &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_FOLDER_IS_NOT_EMPTY );
   
   /* Try to delete "/Test55" - should fail (no such object) */
   ok = psonFolderDeleteObject( pFolder1,
                                "test55",
                                6,
                                &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_NO_SUCH_OBJECT );
   
   /* Get "/Test2/Test4" from "/" */   
   ok = psonFolderGetObject( pFolder1,
                             "test2/test4",
                             11,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   assert_true( ok );
   
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psonObjectDescriptor );
   assert_true( memcmp( pDescriptor->originalName, 
                "Test4", 
                5*sizeof(char) ) == 0 );
   
   /* Create "/Test2/Test4/Test5 from "/" */
   
   ok = psonFolderInsertObject( pFolder1,
                                "test2/test4/test5",
                                "Test2/Test4/Test5",
                                17,
                                &def,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   psonTxCommit( (psonTx *)context.pTransaction, &context );
   
   /* Delete "/Test2/Test4/Test6" - must fail (no such object) */
   ok = psonFolderDeleteObject( pFolder1,
                                "test2/test4/test6",
                                17,
                                &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_NO_SUCH_OBJECT );
   
   /* Delete "/Test2/Test5/Test5" - must fail (no such folder) */
   ok = psonFolderDeleteObject( pFolder1,
                                "test2/test5/test5",
                                17,
                                &context );
   assert_false( ok );
   assert_true( psocGetLastError( &context.errorHandler ) == PSO_NO_SUCH_FOLDER );
   
   /* Delete "/Test2/Test4/Test5" */
   ok = psonFolderDeleteObject( pFolder1,
                                "test2/test4/test5",
                                17,
                                &context );
   assert_true( ok );
   
   psonTxCommit( context.pTransaction, &context );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

