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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonSessionContext context;
   psonHash* pHash;
   enum psoErrors errcode;
   char key[20];
   char data[20];
   int i;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psonHashInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      errcode = psonHashInsert( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                data,
                                strlen(data),
                                &context );
      if ( errcode != PSO_OK ) {
         fprintf( stderr, "i = %d %d\n", i, pHash->enumResize );
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
         errcode = psonHashResize( pHash, NULL );

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
