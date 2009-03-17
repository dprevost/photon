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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonFolder* pFolder;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   psoObjectDefinition def = { PSO_HASH_MAP, PSO_DEF_USER_DEFINED, PSO_DEF_USER_DEFINED };
   psoKeyDefinition keyDef[2] = {
       { "MyKey1", PSO_KEY_CHAR,    20 },
       { "MyKey2", PSO_KEY_VARCHAR, 30 }
   };
   psoFieldDefinition fieldDef[3] = {
      { "Field_1", PSO_CHAR,    {10}  },
      { "Field_1", PSO_INTEGER, {0}   },
      { "Field_1", PSO_VARCHAR, {100} }
   };
   uint32_t retKeyDefLength = 0, retDataDefLength = 0;
   
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
                                (unsigned char *)keyDef,
                                2*sizeof(psoKeyDefinition),
                                (unsigned char *)fieldDef,
                                3*sizeof(psoFieldDefinition),
                                1,
                                0,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFolderGetDefLength( pFolder,
                                "test2",
                                0,
                                &retKeyDefLength,
                                &retDataDefLength,
                                &context );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

