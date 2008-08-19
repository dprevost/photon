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
   psnTxStatus txStatus;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * key3  = "my key3";
   char * data  = "my data";
   psoObjStatus status;
   psoObjectDefinition def = { 
      PSO_HASH_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   psnTxStatusInit( &txStatus, SET_OFFSET( context.pTransaction ) );
   
   ok = psnHashMapInit( pHashMap, 
                         0, 1, 0, &txStatus, 4, 
                         "Map1", SET_OFFSET(pHashMap), &def, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapInsert( pHashMap,
                           (const void *) key1,
                           7,
                           (const void *) data,
                           7,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapInsert( pHashMap,
                           (const void *) key2,
                           7,
                           (const void *) data,
                           7,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnHashMapInsert( pHashMap,
                           (const void *) key3,
                           7,
                           (const void *) data,
                           7,
                           &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnHashMapStatus( pHashMap, &status );

   if ( status.numDataItem != 3 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

