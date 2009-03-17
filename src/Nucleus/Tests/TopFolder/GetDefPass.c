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
#include <photon/psoCommon.h>

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonFolder * pTopFolder;
   psonSessionContext context;
   bool ok;
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
   unsigned char * retKeyDef = NULL, * retDataDef = NULL;
   psoObjectDefinition retDef;
   uint32_t retKeyDefLength = 0, retDataDefLength = 0;
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &def,
                                   (unsigned char *)keyDef,
                                   2*sizeof(psoKeyDefinition),
                                   (unsigned char *)fieldDef,
                                   3*sizeof(psoFieldDefinition),
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonTopFolderGetDef( pTopFolder,
                             "Test1",
                             strlen("Test1"),
                             &retDef,
                             &retKeyDef,
                             &retKeyDefLength,
                             &retDataDef,
                             &retDataDefLength,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( retKeyDefLength != 2*sizeof(psoKeyDefinition) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( retDataDefLength != 3*sizeof(psoFieldDefinition) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp(retKeyDef, keyDef, retKeyDefLength) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp(retDataDef, fieldDef, retDataDefLength) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
