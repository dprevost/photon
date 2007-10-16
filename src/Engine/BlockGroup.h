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

#include "Engine.h"
#include "MemBitmap.h"
#include "LinkNode.h"
#include "LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseBlockGroup
{
   /** Type of memory object */
   vdseMemObjIdent objType;

   /* The linked list itself */
   vdseLinkNode node;

   /* The number of blocks associated with the current group. */
   size_t numBlocks;

   size_t maxFreeBytes;
   
   vdseLinkedList freeList;

   bool isDeletable;
   
   /* Must be last since this struct contains a "variable-length" array. */
   vdseMemBitmap bitmap;
   
} vdseBlockGroup;

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
   
   /* Set to true when a block is in limbo (free but not in the free list */
   bool isInLimbo;
   
   bool lastBlock;
   
} vdseEndBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the vdseBlockGroup struct. 
 */
VDSF_ENGINE_EXPORT
void vdseBlockGroupInit( vdseBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         vdseMemObjIdent   objType );

VDSF_ENGINE_EXPORT
void vdseBlockGroupFini( vdseBlockGroup* pGroup );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdseEndBlockSet( ptrdiff_t firstBlockOffset, 
                      size_t    numBlocks, 
                      bool      limboStatus,
                      bool      lastBlock )
{
   vdseEndBlockGroup* endBlock;
   
   endBlock = GET_PTR( firstBlockOffset + 
                          (numBlocks <<  VDSE_BLOCK_SHIFT) -
                          VDSE_ALLOCATION_UNIT,
                       vdseEndBlockGroup );
                       
   endBlock->firstBlockOffset = firstBlockOffset;
   endBlock->numBlocks = numBlocks;
   endBlock->isInLimbo = limboStatus;
   endBlock->lastBlock = lastBlock;
}

static inline
ptrdiff_t vdseEndBlockOffset( ptrdiff_t firstBlockOffset, 
                              size_t    numBlocks )
{
   return (firstBlockOffset + (numBlocks <<  VDSE_BLOCK_SHIFT) -
                          VDSE_ALLOCATION_UNIT);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_PAGE_GROUP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
