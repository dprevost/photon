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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonFastMap * pHashMap;
   psonSessionContext context;
   bool ok;
   int errcode;
   psonTxStatus status;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   psonFastMapItem item;
   psoObjectDefinition def = { PSO_FAST_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fields;
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonFastMapInit( pHashMap, 0, 1, 0, &status, 4, "Map1", 
                         SET_OFFSET(pHashMap), &def, &keyDef, 
                         &fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data1,
                           8,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psonFastMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data2,
                           8,
                           NULL,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonFastMapGetNext( pHashMap,
                            &item,
                            7,
                            7,
                            &context );
   if ( ok == true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_LENGTH ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
