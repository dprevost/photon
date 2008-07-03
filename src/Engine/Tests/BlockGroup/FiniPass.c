/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/BlockGroup.h"
#include "Engine/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseBlockGroup *pGroup;
   unsigned char* ptr;
   vdseSessionContext context;
   
   initTest( expectedToPass, &context );

   ptr = malloc( VDSE_BLOCK_SIZE*10 );
   if (ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   g_pBaseAddr = ptr;
   
   /* This "100" (non-zero) offset should mark this block group 
    * as the first block group of a MemObject.
    */
   pGroup = (vdseBlockGroup*) (ptr + 100);
   
   vdseBlockGroupInit( pGroup, 
                      SET_OFFSET(ptr),
                      10,
                      VDSE_IDENT_QUEUE );
   vdseBlockGroupFini( pGroup );
   
   if ( pGroup->node.nextOffset != VDSE_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pGroup->node.previousOffset != VDSE_NULL_OFFSET ) {
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
   if ( pGroup->bitmap.baseAddressOffset != VDSE_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
