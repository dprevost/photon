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

#include "hashMapTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psnMap * pHashMap;
   psnSessionContext context;
   bool ok;
   psnTxStatus status;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   psnHashMapItem item;
   psoObjectDefinition def = { 
      PSO_FAST_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psnTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psnMapInit( pHashMap, 
                     0, 1, 0, &status, 4, 
                     "Map1", SET_OFFSET(pHashMap), &def, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnMapInsert( pHashMap,
                       (const void *) key1,
                       7,
                       (const void *) data1,
                       8,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnMapInsert( pHashMap,
                       (const void *) key2,
                       7,
                       (const void *) data2,
                       8,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnMapGetFirst( pHashMap,
                         &item,
                         7,
                         20,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnMapGetNext( pHashMap,
                        &item,
                        7,
                        20,
                        NULL );
 
   ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
