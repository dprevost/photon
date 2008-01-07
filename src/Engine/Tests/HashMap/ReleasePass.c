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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseHashMap * pHashMap;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   char * key  = "my key";
   char * data = "my data";
   vdseHashItem * pItem;
   vdseTxStatus * txItemStatus;
   
   pHashMap = initHashMapTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseHashMapInit( pHashMap, 
                              0, 1, 0, &status, 4, 
                              strCheck("Map1"), NULL_OFFSET, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseHashMapInsert( pHashMap,
                                (const void *) key,
                                6,
                                (const void *) data,
                                7,
                                &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseHashMapGet( pHashMap,
                             (const void *) key,
                             6,
                             &pItem,
                             20,
                             &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   txItemStatus = &pItem->txStatus;
   if ( txItemStatus->usageCounter != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.usageCounter != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   errcode = vdseHashMapRelease( pHashMap,
                                 pItem,
                                 &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( txItemStatus->usageCounter != 0 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.usageCounter != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pHashMap->nodeObject.txCounter != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
