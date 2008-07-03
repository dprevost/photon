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
   vdseHash * pOldHash, * pNewHash;
   enum ListErrors listErr;
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   unsigned char * pData = NULL;
   vdseHashItem* pNewItem;
   size_t bucket = (size_t) -1;
   
   initHashCopyTest( expectedToPass, &pOldHash, &pNewHash, false, &context );
   
   listErr = vdseHashInsert( pOldHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pNewItem,
                             &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   listErr = vdseHashInsert( pOldHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &pNewItem,
                             &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   listErr = vdseHashCopy( pOldHash,
                           pNewHash,
                           &context );
   fprintf( stderr, "err = %d\n", listErr );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   listErr = vdseHashGet( pNewHash,
                          (unsigned char*)key2,
                          strlen(key2),
                          &pNewItem,
                          &context,
                          &bucket );
   if ( listErr != LIST_OK ) {
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
      
   if ( bucket == (size_t) -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
#if 0   
   listErr = vdseHashGetFirst( pHash,
                               &bucketFirst,
                               &offsetFirstItem );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   listErr = vdseHashGetNext( pHash,
                              bucketFirst,
                              offsetFirstItem,
                              &bucketNext,
                              &offsetNextItem );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( bucketNext == (size_t) -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( offsetNextItem == VDSE_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Only 2 items - should fail gracefully ! */
   listErr = vdseHashGetNext( pHash,
                              bucketNext,
                              offsetNextItem,
                              &bucketNext,
                              &offsetNextItem );
   if ( listErr != LIST_END_OF_LIST ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
#endif

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
