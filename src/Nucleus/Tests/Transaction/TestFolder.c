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

#include "txTest.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonTx* pTx;
   psonFolder * pFolder;
   psonSessionContext context;
   bool ok;
   psonFolderItem item;
   psonTxStatus status;
   psoObjectDefinition mapDef = { PSO_HASH_MAP, 0, 0, 0 };
   psonKeyDefinition key;
   psoObjectDefinition folderDef = { PSO_FOLDER, 0, 0, 0 };
   psonDataDefinition fields;

   pFolder = initFolderTest( &context );
   pTx = context.pTransaction;
   
   psonTxStatusInit( &status, SET_OFFSET( pTx ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   assert_true( ok );
   
   /* Test 1 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                &fields,
                                &key,
                                1,
                                0,
                                &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 2 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   psonTxRollback( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 0 );
   
   psonTxCommit( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 0 );
   
   /* Test 2 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                &fields,
                                &key,
                                1,
                                0,
                                &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 2 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   psonTxCommit( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   assert_true( ok );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 2 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   psonTxRollback( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   assert_true( ok );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 2 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   psonTxCommit( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 0 );
   
   /* Test 3 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                &fields,
                                &key,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   assert_true( ok );
   
   psonTxRollback( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 1 );
   assert_true( pFolder->hashObj.numberOfItems == 1 );
   
   ok = psonFolderRelease( pFolder, &item, &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 0 );
   
   /* Test 4 */
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &folderDef,
                                NULL,
                                NULL,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderInsertObject( pFolder,
                                "test3",
                                "Test3",
                                5,
                                &mapDef,
                                &fields,
                                &key,
                                1,
                                0,
                                &context );
   assert_true( ok );
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   assert_true( ok );
   
   psonTxCommit( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   ok = psonFolderRelease( pFolder, &item, &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   ok = psonFolderGetObject( pFolder,
                             "test3",
                             5,
                             PSO_HASH_MAP,
                             &item,
                             &context );
   assert_true( ok );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   assert_true( ok );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 2 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   psonTxRollback( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test2",
                                5,
                                &context );
   assert_true( ok );
   
   ok = psonFolderDeleteObject( pFolder,
                                "test3",
                                5,
                                &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 2 );
   assert_true( pFolder->hashObj.numberOfItems == 2 );
   
   psonTxCommit( pTx, &context );
   assert_true( pFolder->nodeObject.txCounter == 1 );
   assert_true( pFolder->hashObj.numberOfItems == 1 );
   
   ok = psonFolderRelease( pFolder, &item, &context );
   assert_true( ok );
   assert_true( pFolder->nodeObject.txCounter == 0 );
   assert_true( pFolder->hashObj.numberOfItems == 0 );
   
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

