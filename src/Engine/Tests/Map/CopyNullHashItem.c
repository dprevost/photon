/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   vdseMap * pOldMap, * pNewMap;
   vdseSessionContext context;
   int errcode;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   vdsObjectDefinition def = { 
      VDS_FAST_MAP, 
      1, 
      { VDS_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", VDS_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   vdseHashItem   hashItem;
   
   initHashMapCopyTest( expectedToPass, &pOldMap, &pNewMap, &context );

   vdseTxStatusInit( &hashItem.txStatus, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseMapInit( pOldMap, 
                          0, 1, 0, &hashItem.txStatus, 4, 
                          "Map1", SET_OFFSET(pOldMap), &def, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseMapInsert( pOldMap,
                            (const void *) key1,
                            7,
                            (const void *) data1,
                            8,
                            &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = vdseMapInsert( pOldMap,
                            (const void *) key2,
                            7,
                            (const void *) data2,
                            8,
                            &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = vdseMapCopy( pOldMap, 
                          pNewMap,
                          NULL,
                          "Map1",
                          &context );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
