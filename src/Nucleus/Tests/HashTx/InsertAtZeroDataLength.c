/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include "Nucleus/Tests/HashTx/HashTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonSessionContext context;
   psonHashTx * pHash;
   enum psoErrors errcode;
   char* key = "My Key";
   char* data1 = "My Data 1";
   psonHashTxItem * pNewItem;
   size_t bucket;
   bool found;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psonHashTxInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /*
    * We first insert an item and retrieve to get the exact bucket.
    * InsertAt() depends on this as you cannot insert in an empty
    * bucket.
    */
   errcode = psonHashTxInsert( pHash,
                             (unsigned char*)key,
                             strlen(key),
                             data1,
                             strlen(data1),
                             &pNewItem,
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   found = psonHashTxGet( pHash,
                        (unsigned char*)key,
                        strlen(key),
                        &pNewItem,
                        &bucket,
                        &context );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = psonHashTxInsertAt( pHash,
                               bucket,
                               (unsigned char*)key,
                               strlen(key),
                               data1,
                               0,
                               &pNewItem,
                               &context );
                             
   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
