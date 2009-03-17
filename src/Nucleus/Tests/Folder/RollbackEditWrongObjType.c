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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonFolder* pFolder;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   psonFolderItem folderItem;
   psoObjectDefinition mapDef = { PSO_FAST_MAP, PSO_DEF_USER_DEFINED, PSO_DEF_USER_DEFINED };
   psoKeyDefinition key = { "MyKey", PSO_KEY_VARCHAR, 100 };

   const unsigned char * fields =  (unsigned char *)"A dummy definition";

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
                                &mapDef,
                                (unsigned char *)&key,
                                sizeof(key),
                                fields,
                                sizeof(fields),
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderEditObject( pFolder,
                              "test2",
                              5,
                              PSO_FAST_MAP,
                              &folderItem,
                              &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   psonFolderRollbackEdit( pFolder,
                           folderItem.pHashItem, 
                           PSON_IDENT_FOLDER,
                           NULL,
                           &context );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

