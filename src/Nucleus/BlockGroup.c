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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonBlockGroupFini( psonBlockGroup * pGroup )
{
   PSO_PRE_CONDITION( pGroup != NULL );

   psonMemBitmapFini(  &pGroup->bitmap );
   psonLinkedListFini( &pGroup->freeList );
   psonLinkNodeFini(   &pGroup->node );

   pGroup->numBlocks = 0;
   pGroup->maxFreeBytes = 0;
   pGroup->freeBytes = 0;
   pGroup->objType = PSON_IDENT_CLEAR;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: no need to initialize the endBlock struct. It is set by the 
 * memory allocator when calling psonMallocBlocks().
 */
void psonBlockGroupInit( psonBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         psonMemObjIdent   objType )
{
   ptrdiff_t groupOffset;
   size_t currentLength;
   psonFreeBufferNode * firstNode;
   
   PSO_PRE_CONDITION( pGroup != NULL );
   PSO_PRE_CONDITION( firstBlockOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( numBlocks > 0 );
   PSO_PRE_CONDITION( objType > PSON_IDENT_FIRST && objType < PSON_IDENT_LAST );

   pGroup->numBlocks = numBlocks;
   pGroup->objType = PSON_IDENT_PAGE_GROUP & objType;
   
   psonLinkNodeInit( &pGroup->node );
   psonLinkedListInit( &pGroup->freeList );
   
   psonMemBitmapInit( &pGroup->bitmap,
                      firstBlockOffset,
                      numBlocks << PSON_BLOCK_SHIFT, 
                      PSON_ALLOCATION_UNIT );
   
   /* Is the blockGroup struct at the beginning of the group ? */
   groupOffset = SET_OFFSET(pGroup);
   if ( SET_OFFSET(pGroup) == firstBlockOffset ) {
      /* 
       * Yes, we are. This blockGroup is therefore NOT the blockGroup 
       * initially allocated when an object is first allocated.
       */
      pGroup->isDeletable = true;
      currentLength = 0;
   }
   else {
      /*
       * No, we're not. Can't deallocate this groupBlock when empty!
       * The offset of "this" (pGroup) tells us how much memory was
       * allocated for the header of the object.
       */
      pGroup->isDeletable = false;
      currentLength = groupOffset-firstBlockOffset;
   }

   currentLength += offsetof(psonBlockGroup,bitmap) +
                    offsetof(psonMemBitmap,bitmap) +
                    psonGetBitmapLengthBytes( numBlocks << PSON_BLOCK_SHIFT, 
                                              PSON_ALLOCATION_UNIT );
   currentLength = ((currentLength-1)/PSON_ALLOCATION_UNIT+1)*PSON_ALLOCATION_UNIT;
   
   pGroup->maxFreeBytes = (numBlocks << PSON_BLOCK_SHIFT) - currentLength;
   
   /* remove the space required by the psonEndBlockGroup struct */
   pGroup->maxFreeBytes -= PSON_ALLOCATION_UNIT;
   pGroup->freeBytes = pGroup->maxFreeBytes;
   
   /* 
    * So we have one free buffer, starting at offset "currentLength"
    * + firstBlockOffset with length "maxFreeBytes". Insert it in our freeList.
    */
   GET_PTR( firstNode, firstBlockOffset+currentLength, psonFreeBufferNode );
   psonLinkNodeInit( &firstNode->node );
   firstNode->numBuffers = pGroup->maxFreeBytes/PSON_ALLOCATION_UNIT;

   psonLinkedListPutFirst( &pGroup->freeList, &firstNode->node );

   /* And set the bitmap */
   psonSetBufferAllocated( &pGroup->bitmap, firstBlockOffset, currentLength );
   psonSetBufferAllocated( &pGroup->bitmap, 
                           psonEndBlockOffset(firstBlockOffset, numBlocks), 
                           PSON_ALLOCATION_UNIT );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
