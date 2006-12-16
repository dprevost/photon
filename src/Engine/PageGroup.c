/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "PageGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsePageGroupInit( vdsePageGroup* pGroup,
                        ptrdiff_t      groupOffset,
                        size_t         numPages )
{
   ptrdiff_t offset;
   size_t currentLength;
   vdseFreeBufferNode* firstNode;

   VDS_PRE_CONDITION( pGroup != NULL );
   VDS_PRE_CONDITION( groupOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( numPages > 0 );
   
   pGroup->numPages = numPages;
   
   vdseLinkNodeInit( &pGroup->node );
   vdseLinkedListInit( &pGroup->freeList );
   
   vdseMemBitmapInit( &pGroup->bitmap,
                      groupOffset,
                      numPages*PAGESIZE, 
                      VDSE_ALLOCATION_UNIT );
   
   /* Is the groupPage struct at the beginning of the group ? */
   offset = SET_OFFSET(pGroup);
   if ( SET_OFFSET(pGroup) == groupOffset )
   {
      /* 
       * Yes, we are. This pageGroup is therefore NOT the pageGroup 
       * initially allocated when an object is first allocated.
       */
      pGroup->isDeletable = true;
      currentLength = 0;
   }
   else
   {
      /*
       * No, we're not. Can't deallocate this groupPage when empty!
       * The offset of "this" (pGroup) tells us how much memory was
       * allocated for the header of the object.
       */
      pGroup->isDeletable = false;
      currentLength = offset-groupOffset;
   }

   currentLength += offsetof(vdsePageGroup,bitmap) +
                    offsetof(vdseMemBitmap,bitmap) +
                    vdseGetBitmapLengthBytes( numPages*PAGESIZE, 
                                              VDSE_ALLOCATION_UNIT );
   currentLength = ((currentLength-1)/VDSE_ALLOCATION_UNIT+1)*VDSE_ALLOCATION_UNIT;
   
   pGroup->maxFreeBytes = numPages*PAGESIZE - currentLength;

   /* 
    * So we have one free buffer, starting at offset "currentLength"
    * + groupOffset with length "maxFreeBytes". Insert it in our freeList.
    */
   firstNode = GET_PTR( groupOffset+currentLength, vdseFreeBufferNode );
   vdseLinkNodeInit( &firstNode->node );
   firstNode->numBlocks = pGroup->maxFreeBytes/VDSE_ALLOCATION_UNIT;

   vdseLinkedListPutFirst( &pGroup->freeList, &firstNode->node );

   /* And set the bitmap */
   vdseSetBlocksAllocated( &pGroup->bitmap, groupOffset, currentLength );

}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdsePageGroupFini( vdsePageGroup* pGroup )
{
   VDS_PRE_CONDITION( pGroup != NULL );

   vdseMemBitmapFini(  &pGroup->bitmap );
   vdseLinkedListFini( &pGroup->freeList );
   vdseLinkNodeFini(   &pGroup->node );

   pGroup->numPages = 0;
   pGroup->maxFreeBytes = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
