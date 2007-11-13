/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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

#include "Engine/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseBlockGroupFini( vdseBlockGroup * pGroup )
{
   VDS_PRE_CONDITION( pGroup != NULL );

   vdseMemBitmapFini(  &pGroup->bitmap );
   vdseLinkedListFini( &pGroup->freeList );
   vdseLinkNodeFini(   &pGroup->node );

   pGroup->numBlocks = 0;
   pGroup->maxFreeBytes = 0;
   pGroup->freeBytes = 0;
   pGroup->objType = VDSE_IDENT_CLEAR;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * Note: no need to initialize the endBlock struct. It is set by the 
 * memory allocator when calling vdseMallocBlocks().
 */
void vdseBlockGroupInit( vdseBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         vdseMemObjIdent   objType )
{
   ptrdiff_t groupOffset;
   size_t currentLength;
   vdseFreeBufferNode * firstNode;
   
   VDS_PRE_CONDITION( pGroup != NULL );
   VDS_PRE_CONDITION( firstBlockOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numBlocks > 0 );
   VDS_PRE_CONDITION( objType > VDSE_IDENT_FIRST && objType < VDSE_IDENT_LAST );

   pGroup->numBlocks = numBlocks;
   pGroup->objType = VDSE_IDENT_PAGE_GROUP & objType;
   
   vdseLinkNodeInit( &pGroup->node );
   vdseLinkedListInit( &pGroup->freeList );
   
   vdseMemBitmapInit( &pGroup->bitmap,
                      firstBlockOffset,
                      numBlocks << VDSE_BLOCK_SHIFT, 
                      VDSE_ALLOCATION_UNIT );
   
   /* Is the blockGroup struct at the beginning of the group ? */
   groupOffset = SET_OFFSET(pGroup);
   if ( SET_OFFSET(pGroup) == firstBlockOffset )
   {
      /* 
       * Yes, we are. This blockGroup is therefore NOT the blockGroup 
       * initially allocated when an object is first allocated.
       */
      pGroup->isDeletable = true;
      currentLength = 0;
   }
   else
   {
      /*
       * No, we're not. Can't deallocate this groupBlock when empty!
       * The offset of "this" (pGroup) tells us how much memory was
       * allocated for the header of the object.
       */
      pGroup->isDeletable = false;
      currentLength = groupOffset-firstBlockOffset;
   }

   currentLength += offsetof(vdseBlockGroup,bitmap) +
                    offsetof(vdseMemBitmap,bitmap) +
                    vdseGetBitmapLengthBytes( numBlocks << VDSE_BLOCK_SHIFT, 
                                              VDSE_ALLOCATION_UNIT );
   currentLength = ((currentLength-1)/VDSE_ALLOCATION_UNIT+1)*VDSE_ALLOCATION_UNIT;
   
   pGroup->maxFreeBytes = (numBlocks << VDSE_BLOCK_SHIFT) - currentLength;
   
   /* remove the space required by the vdseEndBlockGroup struct */
   pGroup->maxFreeBytes -= VDSE_ALLOCATION_UNIT;
   pGroup->freeBytes = pGroup->maxFreeBytes;
   
   /* 
    * So we have one free buffer, starting at offset "currentLength"
    * + firstBlockOffset with length "maxFreeBytes". Insert it in our freeList.
    */
   GET_PTR( firstNode, firstBlockOffset+currentLength, vdseFreeBufferNode );
   vdseLinkNodeInit( &firstNode->node );
   firstNode->numBuffers = pGroup->maxFreeBytes/VDSE_ALLOCATION_UNIT;

   vdseLinkedListPutFirst( &pGroup->freeList, &firstNode->node );

   /* And set the bitmap */
   vdseSetBufferAllocated( &pGroup->bitmap, firstBlockOffset, currentLength );
   vdseSetBufferAllocated( &pGroup->bitmap, 
                           vdseEndBlockOffset(firstBlockOffset, numBlocks), 
                           VDSE_ALLOCATION_UNIT );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
