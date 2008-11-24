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
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   ptrdiff_t offsetFirstItem;
   psonHashTxItem * pNewItem;
   bool found;
   size_t bucket;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psonHashTxInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = psonHashTxGet( pHash,
                          (unsigned char*)key1,
                          strlen(key1),
                          &pNewItem,
                          &bucket,
                          &context );
   if ( found ) {
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

   found = psonHashTxGet( pHash,
                          (unsigned char*)key2,
                          strlen(key2),
                          &pNewItem,
                          &bucket,
                          &context );
   if ( found ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = psonHashTxInsert( pHash,
                               bucket,
                               (unsigned char*)key2,
                               strlen(key2),
                               data2,
                               strlen(data2),
                               &pNewItem,
                               &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = psonHashTxGetFirst( pHash, &offsetFirstItem );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonHashTxGetNext( pHash,
                      PSON_NULL_OFFSET,
                      &offsetFirstItem );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */