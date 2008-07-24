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

#include "Engine/Hash.h"
#include "Engine/Tests/Hash/HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseHash* pHash;
   enum vdsErrors errcode;
   char* key1 = "My Key 1";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   char* data3 = "This data is much longer than data1";
   vdseHashItem* pNewItem;
   size_t bucket;
   vdseHashItem* pItem = NULL;
   bool found;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = vdseHashInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pNewItem,
                             &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /*
    * Test with same data length
    */
   errcode = vdseHashUpdate( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data2,
                             strlen(data2),
                             &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = vdseHashGet( pHash,
                        (unsigned char*)key1,
                        strlen(key1),
                        &pItem,
                        &bucket,
                        &context );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( GET_PTR_FAST(pItem->dataOffset, void), data2, strlen(data2 )) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /*
    * Test with different data length
    */
   errcode = vdseHashUpdate( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data3,
                             strlen(data3),
                             &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = vdseHashGet( pHash,
                        (unsigned char*)key1,
                        strlen(key1),
                        &pItem,
                        &bucket,
                        &context );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pItem->dataLength != strlen(data3) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( GET_PTR_FAST(pItem->dataOffset, void), data3, strlen(data3 )) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

