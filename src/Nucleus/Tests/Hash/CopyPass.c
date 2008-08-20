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
   psnHash * pOldHash, * pNewHash;
   enum psoErrors errcode;
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   unsigned char * pData = NULL;
   psnHashItem* pNewItem;
   size_t bucket = (size_t) -1;
   bool found;
   
   initHashCopyTest( expectedToPass, &pOldHash, &pNewHash, false, &context );
   
   errcode = psnHashInsert( pOldHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pNewItem,
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = psnHashInsert( pOldHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &pNewItem,
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = psnHashCopy( pOldHash,
                           pNewHash,
                           &context );
   fprintf( stderr, "err = %d\n", errcode );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   found = psnHashGet( pNewHash,
                        (unsigned char*)key2,
                        strlen(key2),
                        &pNewItem,
                        &bucket,
                        &context );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   GET_PTR( pData, pNewItem->dataOffset, unsigned char );
   if ( pData == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pNewItem->dataLength != strlen(data2) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( pData, data2, pNewItem->dataLength) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */