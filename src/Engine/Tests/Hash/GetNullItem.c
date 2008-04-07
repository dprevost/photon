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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   vdseSessionContext context;
   vdseHash* pHash;
   enum ListErrors listErr;
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   vdseHashItem* pNewItem;
   size_t bucket;
   
   pHash = initHashTest( expectedToPass,
                         &context );
   
   listErr = vdseHashInit( pHash, g_memObjOffset, 100, &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   listErr = vdseHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pNewItem,
                             &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   listErr = vdseHashInsert( pHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &pNewItem,
                             &context );
   if ( listErr != LIST_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   listErr = vdseHashGet( pHash,
                          (unsigned char*)key2,
                          strlen(key2),
                          NULL,
                          &context,
                          &bucket );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
