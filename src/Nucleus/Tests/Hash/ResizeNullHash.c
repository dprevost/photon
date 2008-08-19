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

#include "Nucleus/Hash.h"
#include "Nucleus/Tests/Hash/HashTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psnSessionContext context;
   psnHash* pHash;
   enum vdsErrors errcode;
   char key[20];
   char data[20];
   psnHashItem* pNewItem;
   int i;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psnHashInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      errcode = psnHashInsert( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                data,
                                strlen(data),
                                &pNewItem,
                                &context );
      if ( errcode != VDS_OK ) {
         fprintf( stderr, "i = %d %d\n", i, pHash->enumResize );
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      if ( pHash->enumResize == PSN_HASH_TIME_TO_GROW ) {
         errcode = psnHashResize( NULL, &context );

         /* We should never come here! */
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

