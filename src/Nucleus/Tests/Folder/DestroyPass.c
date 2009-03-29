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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonFolder * pFolder;
   psonSessionContext context;
   int errcode;
   bool ok;
   
   psoObjectDefinition def = { PSO_FOLDER, 0, 0, 0 };
   
   pFolder = initTopFolderTest( expectedToPass, &context );

   ok = psonFolderCreateObject( pFolder,
                                "Test1",
                                strlen("Test1"),
                                &def,
                                NULL,
                                NULL,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Using the topfolder to create a grandchild! */
   ok = psonTopFolderCreateObject( pFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &def,
                                   NULL,
                                   NULL,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonTxCommit( (psonTx *)context.pTransaction, &context );
   
   ok = psonFolderDestroyObject( pFolder,
                                 "Test1",
                                 strlen("Test1"),
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_FOLDER_IS_NOT_EMPTY ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
                                         
   ok = psonTopFolderDestroyObject( pFolder,
                                    "Test1/Test2",
                                    strlen("Test1/Test2"),
                                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderDestroyObject( pFolder,
                                 "Test2",
                                 strlen("Test2"),
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonFolderDestroyObject( pFolder,
                                 "Test1",
                                 PSO_MAX_NAME_LENGTH+1,
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonFolderDestroyObject( pFolder,
                                 "Test1",
                                 0,
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonFolderDestroyObject( pFolder,
                                 "Test1",
                                 strlen("Test1"),
                                 &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Calling destroy on the same object, twice */
   ok = psonFolderDestroyObject( pFolder,
                                 "Test1",
                                 strlen("Test1"),
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_OBJECT_IS_IN_USE ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
