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
#include "Nucleus/Tests/Hash/HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSessionContext context;
   psonHash* pHash;
   enum psoErrors errcode;
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   psonHashItem * firstItem = NULL, * nextItem = NULL;
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
   
   errcode = psonHashInsert( pHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = psonHashGetFirst( pHash, &firstItem );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   found = psonHashGetNext( pHash,
                            firstItem,
                            &nextItem );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( nextItem == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Only 2 items - should fail gracefully ! */
   found = psonHashGetNext( pHash,
                            nextItem,
                            &nextItem );
   if ( found ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
