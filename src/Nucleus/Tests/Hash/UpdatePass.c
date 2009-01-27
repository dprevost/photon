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
#include "Nucleus/Tests/Hash/HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSessionContext context;
   psonHash* pHash;
   enum psoErrors errcode;
   char* key1 = "My Key 1";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   char* data3 = "This data is much longer than data1";
   size_t bucket;
   psonHashItem* pItem = NULL;
   bool found;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psonHashInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = psonHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   /*
    * Test with same data length
    */
   errcode = psonHashUpdate( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data2,
                             strlen(data2),
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = psonHashGet( pHash,
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
   errcode = psonHashUpdate( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data3,
                             strlen(data3),
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = psonHashGet( pHash,
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

