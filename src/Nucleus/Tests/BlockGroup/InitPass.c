/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/BlockGroup.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonBlockGroup *pGroup;
   unsigned char* ptr;
   psonSessionContext context;
   
   initTest( expectedToPass, &context );

   ptr = malloc( PSON_BLOCK_SIZE*10 );
   if (ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   g_pBaseAddr = ptr;
   
   /* This "100" (non-zero) offset should mark this block group 
    * as the first block group of a MemObject.
    */
   pGroup = (psonBlockGroup*) (ptr + 100);
   
   psonBlockGroupInit( pGroup, 
                      SET_OFFSET(ptr),
                      10,
                      PSON_IDENT_QUEUE );
   if ( pGroup->node.nextOffset != PSON_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->node.previousOffset != PSON_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->numBlocks != 10 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->maxFreeBytes < 9*PSON_BLOCK_SIZE || 
            pGroup->maxFreeBytes >= 10*PSON_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if (pGroup->freeList.initialized != PSON_LIST_SIGNATURE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->isDeletable == true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->bitmap.baseAddressOffset != SET_OFFSET(ptr) ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonBlockGroupFini( pGroup );

   /* A zero offset this time */
   pGroup = (psonBlockGroup*) ptr;
   psonBlockGroupInit( pGroup, SET_OFFSET(ptr), 10, PSON_IDENT_QUEUE );
   if ( pGroup->isDeletable == false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonBlockGroupFini( pGroup );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
