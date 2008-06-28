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
   vdseMap * pHashMap;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   char * key  = "my key";
   char * data = "my data";
   vdseHashItem * pItem;
   vdsObjectDefinition def = { 
      VDS_MAP, 
      1, 
      { VDS_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseMapInit( pHashMap, 
                          0, 1, 0, &status, 4, 
                          "Map1", NULL_OFFSET, &def, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseMapInsert( pHashMap,
                            (const void *) key,
                            6,
                            (const void *) data,
                            7,
                            &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Is the item there? */
   errcode = vdseMapGet( pHashMap,
                         (const void *) key,
                         6,
                         &pItem,
                         20,
                         &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseMapDelete( pHashMap,
                            (const void *) key,
                            0,
                            &context );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
