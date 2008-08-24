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
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnBlockGroupFini( psnBlockGroup * pGroup )
{
   PSO_PRE_CONDITION( pGroup != NULL );

   psnMemBitmapFini(  &pGroup->bitmap );
   psnLinkedListFini( &pGroup->freeList );
   psnLinkNodeFini(   &pGroup->node );

   pGroup->numBlocks = 0;
   pGroup->maxFreeBytes = 0;
   pGroup->freeBytes = 0;
   pGroup->objType = PSN_IDENT_CLEAR;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: no need to initialize the endBlock struct. It is set by the 
 * memory allocator when calling psnMallocBlocks().
 */
void psnBlockGroupInit( psnBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         psnMemObjIdent   objType )
{
   ptrdiff_t groupOffset;
   size_t currentLength;
   psnFreeBufferNode * firstNode;
   
   PSO_PRE_CONDITION( pGroup != NULL );
   PSO_PRE_CONDITION( firstBlockOffset != PSN_NULL_OFFSET );
   PSO_PRE_CONDITION( numBlocks > 0 );
   PSO_PRE_CONDITION( objType > PSN_IDENT_FIRST && objType < PSN_IDENT_LAST );

   pGroup->numBlocks = numBlocks;
   pGroup->objType = PSN_IDENT_PAGE_GROUP & objType;
   
   psnLinkNodeInit( &pGroup->node );
   psnLinkedListInit( &pGroup->freeList );
   
   psnMemBitmapInit( &pGroup->bitmap,
                      firstBlockOffset,
                      numBlocks << PSN_BLOCK_SHIFT, 
                      PSN_ALLOCATION_UNIT );
   
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

   currentLength += offsetof(psnBlockGroup,bitmap) +
                    offsetof(psnMemBitmap,bitmap) +
                    psnGetBitmapLengthBytes( numBlocks << PSN_BLOCK_SHIFT, 
                                              PSN_ALLOCATION_UNIT );
   currentLength = ((currentLength-1)/PSN_ALLOCATION_UNIT+1)*PSN_ALLOCATION_UNIT;
   
   pGroup->maxFreeBytes = (numBlocks << PSN_BLOCK_SHIFT) - currentLength;
   
   /* remove the space required by the psnEndBlockGroup struct */
   pGroup->maxFreeBytes -= PSN_ALLOCATION_UNIT;
   pGroup->freeBytes = pGroup->maxFreeBytes;
   
   /* 
    * So we have one free buffer, starting at offset "currentLength"
    * + firstBlockOffset with length "maxFreeBytes". Insert it in our freeList.
    */
   GET_PTR( firstNode, firstBlockOffset+currentLength, psnFreeBufferNode );
   psnLinkNodeInit( &firstNode->node );
   firstNode->numBuffers = pGroup->maxFreeBytes/PSN_ALLOCATION_UNIT;

   psnLinkedListPutFirst( &pGroup->freeList, &firstNode->node );

   /* And set the bitmap */
   psnSetBufferAllocated( &pGroup->bitmap, firstBlockOffset, currentLength );
   psnSetBufferAllocated( &pGroup->bitmap, 
                           psnEndBlockOffset(firstBlockOffset, numBlocks), 
                           PSN_ALLOCATION_UNIT );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
