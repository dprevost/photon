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
   psonFolder* pFolder;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   psoObjStatus objStatus;
   psoObjectDefinition def = { PSO_FOLDER, 0 };
   
   pFolder = initFolderTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFolderInit( pFolder, 0, 1, 0, &status, 5, "Test1", 1234, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test2",
                                "Test2",
                                5,
                                &def,
                                NULL,
                                0,
                                NULL,
                                0,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderInsertObject( pFolder,
                                "test2/test3",
                                "Test2/test3",
                                11,
                                &def,
                                NULL,
                                0,
                                NULL,
                                0,
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderGetStatus( pFolder,
                             "test2",
                             5,
                             &objStatus,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( objStatus.numDataItem != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( objStatus.numBlocks != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( objStatus.numBlockGroup != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( objStatus.freeBytes == 0 || objStatus.freeBytes >=PSON_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

