/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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
   psnFolder * pTopFolder;
   psnSessionContext context;
   int errcode;
   bool ok;
   psoObjectDefinition def = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   ok = psnTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &def,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &def,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnTxCommit( (psnTx *)context.pTransaction, &context );
   
   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1",
                                    strlen("Test1"),
                                    &context );
                                         
   if ( pscGetLastError(&context.errorHandler) != PSO_FOLDER_IS_NOT_EMPTY ) {
      if ( ok != true ) {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      else {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }

   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1/Test2",
                                    PSO_MAX_FULL_NAME_LENGTH+1,
                                    &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != PSO_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1/Test2",
                                    0,
                                    &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1/Test2",
                                    strlen("Test1/Test2"),
                                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test3/Test2",
                                    strlen("Test3/Test2"),
                                    &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != PSO_NO_SUCH_FOLDER ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1/Test5",
                                    strlen("Test1/Test5"),
                                    &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != PSO_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Calling destroy on the same object, twice */
   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1/Test2",
                                    strlen("Test1/Test2"),
                                    &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != PSO_OBJECT_IS_IN_USE ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderDestroyObject( pTopFolder,
                                    "Test1",
                                    strlen("Test1"),
                                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
