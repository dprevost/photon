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

#ifndef VDSE_BLOCK_GROUP_H
#define VDSE_BLOCK_GROUP_H

#include "Engine/Engine.h"
#include "Engine/MemBitmap.h"
#include "Engine/LinkNode.h"
#include "Engine/LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseBlockGroup
{
   /** Type of memory object */
   vdseMemObjIdent objType;

   /** Our own node for the link list of all groups of a memory object */
   vdseLinkNode node;

   /** The number of blocks associated with the current group. */
   size_t numBlocks;

   /** Maximum number of free memory if the block group is not used */
   size_t maxFreeBytes;
   
   /** Current amount of free memory in the block group */
   size_t freeBytes;
   
   /** Our linked list of free buffers. */
   vdseLinkedList freeList;

   bool isDeletable;
   
   /** Must be last since this struct contains a "variable-length" array. */
   vdseMemBitmap bitmap;
   
} vdseBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This struct is to be set at the end of a group of blocks.
 *  The exact details of the struct might change - the main constraint
 *  is that the struct must be no greater than VDSE_ALLOCATION_UNIT.
 *
 *  Note: the struct is also used for group of blocks with no vdseBlockGroup
 *  (a group of free blocks, for example). It should make no reference to
 *  that struct, obviously.
 */
typedef struct vdseEndBlockGroup
{
   /** The offset to the start of the group of blocks */
   ptrdiff_t firstBlockOffset;
   
   /** The number ofblocks in that group */
   size_t numBlocks;
   
   /** Set to true when a block is in limbo (free but not in the free list */
   bool isInLimbo;
   
   /** Set to true if the block is the last one in the vds */
   bool lastBlock;
   
} vdseEndBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
void vdseBlockGroupFini( vdseBlockGroup* pGroup );

/** 
 * Initialize the vdseBlockGroup struct. 
 */
VDSF_ENGINE_EXPORT
void vdseBlockGroupInit( vdseBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         vdseMemObjIdent   objType );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** This small inline function returns the offset of the end block struct. */

static inline
ptrdiff_t vdseEndBlockOffset( ptrdiff_t firstBlockOffset, 
                              size_t    numBlocks )
{
   return (firstBlockOffset + 
      (numBlocks <<  VDSE_BLOCK_SHIFT) -
      VDSE_ALLOCATION_UNIT);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseEndBlockSet( ptrdiff_t firstBlockOffset, 
                      size_t    numBlocks, 
                      bool      limboStatus,
                      bool      lastBlock )
{
   vdseEndBlockGroup* endBlock;
   
   GET_PTR( endBlock, 
            firstBlockOffset + (numBlocks <<  VDSE_BLOCK_SHIFT) -
               VDSE_ALLOCATION_UNIT,
            vdseEndBlockGroup );
                       
   endBlock->firstBlockOffset = firstBlockOffset;
   endBlock->numBlocks = numBlocks;
   endBlock->isInLimbo = limboStatus;
   endBlock->lastBlock = lastBlock;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_BLOCK_GROUP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
