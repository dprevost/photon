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

#include "Hash.h"
#include "HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseHash* pHash;
   enum ListErrors listErr;
   char key[20];
   char data[20];
   vdseHashItem* pNewItem;
   int i;
   unsigned char * pData;
   vdseHashItem* pItem = NULL;
   size_t bucket;
   
   pHash = initHashTest( expectedToPass,
                         &context );
   
   listErr = vdseHashInit( pHash, 10, &context );
   if ( listErr != LIST_OK )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i )
   {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      listErr = vdseHashInsert( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                data,
                                strlen(data),
                                &pNewItem,
                                &context );
      if ( listErr != LIST_OK )
      {
         fprintf( stderr, "i = %d %d\n", i, pHash->enumResize );
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      if ( pHash->enumResize == VDSE_HASH_TIME_TO_GROW )
      {
         listErr = vdseHashResize( pHash, &context );
         if ( listErr != LIST_OK )
         {
            ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
         }
         else
            fprintf( stderr, "Resize ok %d\n", i );
         if ( pHash->enumResize == VDSE_HASH_TIME_TO_GROW )
            ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   
   /* 
    * Next step: test that we can reach all the data by doing a get,
    * than a delete and test shrinking at the same time.
    */
   for ( i = 0; i < 500; ++i )
   {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      
      listErr = vdseHashGet( pHash,
                             (unsigned char*)key,
                             strlen(key),
                             &pItem,
                             &context,
                             &bucket );
      if ( listErr != LIST_OK )
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      pData = GET_PTR(pItem->dataOffset, unsigned char);
      if ( memcmp( data, pData, strlen(data) ) != 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
      if ( pItem->dataLength != strlen(data) )
         ERROR_EXIT( expectedToPass, NULL, ; );

      listErr = vdseHashDelete( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                &context );
      if ( listErr != LIST_OK )
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

      if ( pHash->enumResize == VDSE_HASH_TIME_TO_SHRINK )
      {
         listErr = vdseHashResize( pHash, &context );
         if ( listErr != LIST_OK )
         {
            ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
         }
         else
            fprintf( stderr, "Resize (shrink ok %d\n", i );
         if ( pHash->enumResize == VDSE_HASH_TIME_TO_SHRINK )
            ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
    
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
