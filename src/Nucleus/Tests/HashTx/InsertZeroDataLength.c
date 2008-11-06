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
