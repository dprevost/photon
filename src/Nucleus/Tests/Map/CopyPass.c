/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnMap * pOldMap, * pNewMap;
   psnSessionContext context;
   bool ok;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   char * ptr;
   psoObjectDefinition def = { 
      PSO_FAST_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   psnHashItem   hashItem;
   psnHashItem * pItem;
   
   initHashMapCopyTest( expectedToPass, &pOldMap, &pNewMap, &context );

   psnTxStatusInit( &hashItem.txStatus, SET_OFFSET( context.pTransaction ) );
   
   ok = psnMapInit( pOldMap, 
                     0, 1, 0, &hashItem.txStatus, 4, 
                     "Map1", SET_OFFSET(pOldMap), &def, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnMapInsert( pOldMap,
                       (const void *) key1,
                       7,
                       (const void *) data1,
                       8,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnMapInsert( pOldMap,
                       (const void *) key2,
                       7,
                       (const void *) data2,
                       8,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psnMapCopy( pOldMap, 
                     pNewMap,
                     &hashItem,
                     "Map1",
                     &context );
   
   ok = psnMapGet( pNewMap,
                    (const void *) key1,
                    7,
                    &pItem,
                    20,
                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( ptr, pItem->dataOffset, char );
   if (memcmp( data1, ptr, 8 ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   ok = psnMapGet( pNewMap,
                    (const void *) key2,
                    7,
                    &pItem,
                    20,
                    &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( ptr, pItem->dataOffset, char );
   if (memcmp( data2, ptr, 8 ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
