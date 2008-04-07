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
   enum ListErrors listErr;
   char* key1 = "My Key 1";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   vdseHashItem* pNewItem;
   size_t bucket;
   vdseHashItem* pItem = NULL;
   
   pHash = initHashTest( expectedToPass, &context );
   
   listErr = vdseHashInit( pHash, g_memObjOffset, 100, &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /*
    * We first insert an item and retrieve to get the exact bucket.
    * InsertAt() depends on this as you cannot insert in an empty
    * bucket.
    */
   listErr = vdseHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data2,
                             strlen(data2),
                             &pNewItem,
                             &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   listErr = vdseHashGet( pHash,
                          (unsigned char*)key1,
                          strlen(key1),
                          &pNewItem,
                          &context,
                          &bucket );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   listErr = vdseHashInsertAt( pHash,
                               bucket,
                               (unsigned char*)key1,
                               strlen(key1),
                               data1,
                               strlen(data1),
                               &pNewItem,
                               &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   vdseHashDeleteAt( pHash,
                     bucket,
                     pNewItem,
                     &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   listErr = vdseHashGet( pHash,
                          (unsigned char*)key1,
                          strlen(key1),
                          &pItem,
                          &context,
                          &bucket );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

