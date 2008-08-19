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

#include "hashMapTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnHashMap * pHashMap;
   psnSessionContext context;
   bool ok;
   psnTxStatus status;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   psnHashMapItem item;
   char * ptr1, * ptr2;
   psoObjectDefinition def = { 
      PSO_HASH_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psnTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psnHashMapInit( pHashMap, 
                         0, 1, 0, &status, 4, 
                         "Map1", SET_OFFSET(pHashMap), &def, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data1,
                           8,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data2,
                           8,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashMapGetFirst( pHashMap,
                             &item,
                             7,
                             20,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( ptr1, item.pHashItem->dataOffset, char );

   ok = psnHashMapGetNext( pHashMap,
                            &item,
                            7,
                            20,
                            &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( ptr2, item.pHashItem->dataOffset, char );
   if ( ptr1 == ptr2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   if (memcmp( data1, ptr1, 8 ) == 0 ) {
      if (memcmp( data2, ptr2, 8 ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else {
      if (memcmp( data1, ptr2, 8 ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      if (memcmp( data2, ptr1, 8 ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
