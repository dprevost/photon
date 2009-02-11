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

#include "hashMapTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonMap * pHashMap;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   psonFashMapItem item;
   psoObjectDefinition def = { PSO_FAST_MAP, 1 };
   psoKeyDefinition keyDef = { PSO_KEY_VAR_STRING, 0, 1, 100 };
   psoFieldDefinition fields[1] =  { 
      { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } 
   };
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonMapInit( pHashMap, 0, 1, 0, &status, 4, "Map1", 
                     SET_OFFSET(pHashMap), &def, &keyDef, fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapInsert( pHashMap,
                       (const void *) key1,
                       7,
                       (const void *) data1,
                       8,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonMapInsert( pHashMap,
                       (const void *) key2,
                       7,
                       (const void *) data2,
                       8,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapGetFirst( pHashMap,
                         &item,
                         7,
                         20,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonMapGetNext( NULL,
                        &item,
                        7,
                        20,
                        &context );
 
   ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
