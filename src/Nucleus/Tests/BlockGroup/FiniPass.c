/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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
   psonBlockGroupFini( pGroup );
   
   if ( pGroup->node.nextOffset != PSON_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->node.previousOffset != PSON_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->numBlocks != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->maxFreeBytes != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if (pGroup->freeList.initialized != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->bitmap.baseAddressOffset != PSON_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
