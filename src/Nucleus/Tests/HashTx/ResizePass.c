/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Hash.h"
#include "Nucleus/Tests/HashTx/HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSessionContext context;
   psonHashTx * pHash;
   enum psoErrors errcode;
   char key[20];
   char data[20];
   psonHashTxItem * pNewItem;
   int i;
   unsigned char * pData;
   psonHashTxItem * pItem = NULL;
   size_t bucket;
   bool ok, found;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psonHashTxInit( pHash, g_memObjOffset, 12, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      found = psonHashTxGet( pHash,
                             (unsigned char*)key,
                             strlen(key),
                             &pNewItem,
                             &bucket,
                             &context );
      if ( found ) {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      errcode = psonHashTxInsert( pHash,
                                  bucket,
                                  (unsigned char*)key,
                                  strlen(key),
                                  data,
                                  strlen(data),
                                  &pNewItem,
                                  &context );
      if ( errcode != PSO_OK ) {
         fprintf( stderr, "i = %d %d\n", i, pHash->enumResize );
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
         errcode = psonHashTxResize( pHash, &context );
         if ( errcode != PSO_OK ) {
            ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
         }
         else {
            fprintf( stderr, "Resize ok %d\n", i );
         }
         if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
      }
   }
   
   /* 
    * Next step: test that we can reach all the data by doing a get,
    * than a delete and test shrinking at the same time.
    */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      
      ok = psonHashTxGet( pHash,
                          (unsigned char*)key,
                          strlen(key),
                          &pItem,
                          &bucket,
                          &context );
      if ( ! ok ) {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      GET_PTR( pData, pItem->dataOffset, unsigned char );
      if ( memcmp( data, pData, strlen(data) ) != 0 ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      if ( pItem->dataLength != strlen(data) ) {
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      
      psonHashTxDelete( pHash,
                        pItem,
                        &context );
      
      if ( pHash->enumResize == PSON_HASH_TIME_TO_SHRINK ) {
         errcode = psonHashTxResize( pHash, &context );
         if ( errcode != PSO_OK ) {
            ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
         }
         else {
            fprintf( stderr, "Resize (shrink ok %d)\n", 500-i );
         }
         if ( pHash->enumResize == PSON_HASH_TIME_TO_SHRINK ) {
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
      }
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

