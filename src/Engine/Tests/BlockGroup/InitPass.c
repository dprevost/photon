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
   if (ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   g_pBaseAddr = ptr;
   
   /* This "100" (non-zero) offset should mark this block group 
    * as the first block group of a MemObject.
    */
   pGroup = (vdseBlockGroup*) (ptr + 100);
   
   vdseBlockGroupInit( pGroup, 
                      SET_OFFSET(ptr),
                      10,
                      VDSE_IDENT_QUEUE );
   if ( pGroup->node.nextOffset != NULL_OFFSET )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pGroup->node.previousOffset != NULL_OFFSET )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pGroup->numBlocks != 10 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pGroup->maxFreeBytes < 9*VDSE_BLOCK_SIZE || 
        pGroup->maxFreeBytes >= 10*VDSE_BLOCK_SIZE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if (pGroup->freeList.initialized != VDSE_LIST_SIGNATURE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pGroup->isDeletable == true )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pGroup->bitmap.baseAddressOffset != SET_OFFSET(ptr) )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseBlockGroupFini( pGroup );

   /* A zero offset this time */
   pGroup = (vdseBlockGroup*) ptr;
   vdseBlockGroupInit( pGroup, SET_OFFSET(ptr), 10, VDSE_IDENT_QUEUE );
   if ( pGroup->isDeletable == false )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdseBlockGroupFini( pGroup );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
