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

#ifndef PSN_BLOCK_GROUP_H
#define PSN_BLOCK_GROUP_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemBitmap.h"
#include "Nucleus/LinkNode.h"
#include "Nucleus/LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnBlockGroup
{
   /** Type of memory object */
   psnMemObjIdent objType;

   /** Our own node for the link list of all groups of a memory object */
   psnLinkNode node;

   /** The number of blocks associated with the current group. */
   size_t numBlocks;

   /** Maximum number of free memory if the block group is not used */
   size_t maxFreeBytes;
   
   /** Current amount of free memory in the block group */
   size_t freeBytes;
   
   /** Our linked list of free buffers. */
   psnLinkedList freeList;

   bool isDeletable;
   
   /** Must be last since this struct contains a "variable-length" array. */
   psnMemBitmap bitmap;
   
};

typedef struct psnBlockGroup psnBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This struct is to be set at the end of a group of blocks.
 *  The exact details of the struct might change - the main constraint
 *  is that the struct must be no greater than PSN_ALLOCATION_UNIT.
 *
 *  Note: the struct is also used for group of blocks with no psnBlockGroup
 *  (a group of free blocks, for example). It should make no reference to
 *  that struct, obviously.
 */
struct psnEndBlockGroup
{
   /** The offset to the start of the group of blocks */
   ptrdiff_t firstBlockOffset;
   
   /** The number ofblocks in that group */
   size_t numBlocks;
   
   /** Set to true when a block is in limbo (free but not in the free list */
   bool isInLimbo;
   
   /** Set to true if the block is the last one in the shared memory */
   bool lastBlock;
   
};

typedef struct psnEndBlockGroup psnEndBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
void psnBlockGroupFini( psnBlockGroup* pGroup );

/** 
 * Initialize the psnBlockGroup struct. 
 */
PHOTON_ENGINE_EXPORT
void psnBlockGroupInit( psnBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         psnMemObjIdent   objType );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** This small inline function returns the offset of the end block struct. */

static inline
ptrdiff_t psnEndBlockOffset( ptrdiff_t firstBlockOffset, 
                              size_t    numBlocks )
{
   return (firstBlockOffset + 
      (numBlocks <<  PSN_BLOCK_SHIFT) -
      PSN_ALLOCATION_UNIT);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psnEndBlockSet( ptrdiff_t firstBlockOffset, 
                      size_t    numBlocks, 
                      bool      limboStatus,
                      bool      lastBlock )
{
   psnEndBlockGroup* endBlock;
   
   GET_PTR( endBlock, 
            firstBlockOffset + (numBlocks <<  PSN_BLOCK_SHIFT) -
               PSN_ALLOCATION_UNIT,
            psnEndBlockGroup );
                       
   endBlock->firstBlockOffset = firstBlockOffset;
   endBlock->numBlocks = numBlocks;
   endBlock->isInLimbo = limboStatus;
   endBlock->lastBlock = lastBlock;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_BLOCK_GROUP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

