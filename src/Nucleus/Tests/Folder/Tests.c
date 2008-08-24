/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "folderTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnFolder* pFolder1, *pFolder2;

   psnSessionContext context;
   bool ok;
   psnTxStatus status;
   psnObjectDescriptor* pDescriptor = NULL;
   psnFolderItem folderItem;
   psoObjectDefinition def = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   /* Create "/" */
   pFolder1 = initFolderTest( expectedToPass, &context );

   psnTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psnFolderInit( pFolder1, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Create "/Test2" */   
   ok = psnFolderInsertObject( pFolder1,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Try to create "/Test2" again - must fail */   
   ok = psnFolderInsertObject( pFolder1,
                                "test2",
                                "Test5",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pscGetLastError( &context.errorHandler ) != PSO_OBJECT_ALREADY_PRESENT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Create "/Test3" */   
   ok = psnFolderInsertObject( pFolder1,
                                "test3",
                                "Test3",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Get "/Test2" */   
   ok = psnFolderGetObject( pFolder1,
                             "test2",
                             5,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );   
   }
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psnObjectDescriptor );
   GET_PTR( pFolder2, pDescriptor->offset, psnFolder );

   /* Create "/Test2/Test4" from "/Test2" */   
   ok = psnFolderInsertObject( pFolder2,
                                "test4",
                                "Test4",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /* Create "/Test2/Test2" */   
   ok = psnFolderInsertObject( pFolder2,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( (psnTx *)context.pTransaction, &context );
   
   /* Try to delete "/Test2" - should fail (not empty) */
   ok = psnFolderDeleteObject( pFolder1,
                                "test2",
                                5,
                                &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pscGetLastError( &context.errorHandler ) != PSO_FOLDER_IS_NOT_EMPTY ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Try to delete "/Test55" - should fail (no such object) */
   ok = psnFolderDeleteObject( pFolder1,
                                "test55",
                                6,
                                &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pscGetLastError( &context.errorHandler ) != PSO_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Get "/Test2/Test4" from "/" */   
   ok = psnFolderGetObject( pFolder1,
                             "test2/test4",
                             11,
                             PSO_FOLDER,
                             &folderItem,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );   
   }
   
   GET_PTR( pDescriptor, folderItem.pHashItem->dataOffset, psnObjectDescriptor );
   if ( memcmp( pDescriptor->originalName, 
                "Test4", 
                5*sizeof(char) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Create "/Test2/Test4/Test5 from "/" */
   
   ok = psnFolderInsertObject( pFolder1,
                                "test2/test4/test5",
                                "Test2/Test4/Test5",
                                17,
                                &def,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( (psnTx *)context.pTransaction, &context );
   
   /* Delete "/Test2/Test4/Test6" - must fail (no such object) */
   ok = psnFolderDeleteObject( pFolder1,
                                "test2/test4/test6",
                                17,
                                &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pscGetLastError( &context.errorHandler ) != PSO_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Delete "/Test2/Test5/Test5" - must fail (no such folder) */
   ok = psnFolderDeleteObject( pFolder1,
                                "test2/test5/test5",
                                17,
                                &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pscGetLastError( &context.errorHandler ) != PSO_NO_SUCH_FOLDER ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Delete "/Test2/Test4/Test5" */
   ok = psnFolderDeleteObject( pFolder1,
                                "test2/test4/test5",
                                17,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( context.pTransaction, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
