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

#include "Nucleus/Hash.h"
#include "Nucleus/Tests/Hash/HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnSessionContext context;
   psnHash* pHash;
   enum psoErrors errcode;
   char* key1 = "My Key 1";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   psnHashItem* pNewItem;
   size_t bucket;
   psnHashItem* pItem = NULL;
   bool ok;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psnHashInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /*
    * We first insert an item and retrieve to get the exact bucket.
    * InsertAt() depends on this as you cannot insert in an empty
    * bucket.
    */
   errcode = psnHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data2,
                             strlen(data2),
                             &pNewItem,
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   ok = psnHashGet( pHash,
                     (unsigned char*)key1,
                     strlen(key1),
                     &pNewItem,
                     &bucket,
                     &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = psnHashInsertAt( pHash,
                               bucket,
                               (unsigned char*)key1,
                               strlen(key1),
                               data1,
                               strlen(data1),
                               &pNewItem,
                               &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnHashDelWithItem( pHash,
                           pNewItem,
                           &context );
   
   ok = psnHashGet( pHash,
                     (unsigned char*)key1,
                     strlen(key1),
                     &pItem,
                     &bucket,
                     &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
