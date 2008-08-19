/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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
   char * key  = "my key";
   char * data = "my data";
   psnHashMapItem item;
   vdsObjectDefinition def = { 
      VDS_FAST_MAP, 
      1, 
      { VDS_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 1, 100, 0, 0 } } 
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
                       (const void *) key,
                       6,
                       (const void *) data,
                       7,
                       &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnMapGetFirst( pHashMap,
                         &item,
                         6,
                         20,
                         NULL );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
