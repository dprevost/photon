/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseHashMap * pHashMap;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   char * key1  = "my key1";
   char * key2  = "my key2";
   char * data1 = "my data1";
   char * data2 = "my data2";
   vdseHashMapItem item;
   char * ptr1, * ptr2;
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseHashMapInit( pHashMap, 
                              0, 1, 0, &status, 4, 
                              strCheck("Map1"), NULL_OFFSET, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseHashMapInsert( pHashMap,
                                (const void *) key1,
                                7,
                                (const void *) data1,
                                8,
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   errcode = vdseHashMapInsert( pHashMap,
                                (const void *) key2,
                                7,
                                (const void *) data2,
                                8,
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseHashMapGetFirst( pHashMap,
                                  &item,
                                  &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   ptr1 = GET_PTR( item.pHashItem->dataOffset, char );

   errcode = vdseHashMapGetNext( pHashMap,
                                 &item,
                                 &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   ptr2 = GET_PTR( item.pHashItem->dataOffset, char );
   if ( ptr1 == ptr2 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   if (memcmp( data1, ptr1, 8 ) == 0 )
   {
      if (memcmp( data2, ptr2, 8 ) != 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
   }
   else
   {
      if (memcmp( data1, ptr2, 8 ) != 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
      if (memcmp( data2, ptr1, 8 ) != 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
