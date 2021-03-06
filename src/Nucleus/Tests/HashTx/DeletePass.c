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
   char* key1 = "My Key 1";
   char* data1 = "My Data 1";
   psonHashTxItem * pNewItem;
   size_t bucket;
   psonHashTxItem * pItem = NULL;
   bool ok;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psonHashTxInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* For this test, we will insert a duplicate. */
   
   ok = psonHashTxGet( pHash,
                       (unsigned char*)key1,
                       strlen(key1),
                       &pNewItem,
                       &bucket,
                       &context );
   if ( ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key1,
                               strlen(key1),
                               data1,
                               strlen(data1),
                               &pItem,
                               &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = psonHashTxInsert( pHash,
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
   
   psonHashTxDelete( pHash,
                     pNewItem,
                     &context );
   
   ok = psonHashTxGet( pHash,
                     (unsigned char*)key1,
                     strlen(key1),
                     &pItem,
                     &bucket,
                     &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psonHashTxDelete( pHash, pItem, &context );

   /* Do it again - but reverse the order of deletions */

   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key1,
                               strlen(key1),
                               data1,
                               strlen(data1),
                               &pItem,
                               &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   errcode = psonHashTxInsert( pHash,
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
   
   psonHashTxDelete( pHash,
                     pItem,
                     &context );
   
   ok = psonHashTxGet( pHash,
                       (unsigned char*)key1,
                       strlen(key1),
                       &pItem,
                       &bucket,
                       &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psonHashTxDelete( pHash, pItem, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

