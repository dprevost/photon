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

#ifndef PSON_BLOCK_GROUP_H
#define PSON_BLOCK_GROUP_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemBitmap.h"
#include "Nucleus/LinkNode.h"
#include "Nucleus/LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonBlockGroup
{
   /** Type of memory object */
   psonMemObjIdent objType;

   /** Our own node for the link list of all groups of a memory object */
   psonLinkNode node;

   /** The number of blocks associated with the current group. */
   size_t numBlocks;

   /** Maximum number of free memory if the block group is not used */
   size_t maxFreeBytes;
   
   /** Current amount of free memory in the block group */
   size_t freeBytes;
   
   /** Our linked list of free buffers. */
   psonLinkedList freeList;

   bool isDeletable;
   
   /** Must be last since this struct contains a "variable-length" array. */
   psonMemBitmap bitmap;
   
};

typedef struct psonBlockGroup psonBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This struct is to be set at the end of a group of blocks.
 *  The exact details of the struct might change - the main constraint
 *  is that the struct must be no greater than PSON_ALLOCATION_UNIT.
 *
 *  Note: the struct is also used for group of blocks with no psonBlockGroup
 *  (a group of free blocks, for example). It should make no reference to
 *  that struct, obviously.
 */
struct psonEndBlockGroup
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

typedef struct psonEndBlockGroup psonEndBlockGroup;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
void psonBlockGroupFini( psonBlockGroup* pGroup );

/** 
 * Initialize the psonBlockGroup struct. 
 */
PHOTON_ENGINE_EXPORT
void psonBlockGroupInit( psonBlockGroup  * pGroup,
                         ptrdiff_t         firstBlockOffset,
                         size_t            numBlocks,
                         psonMemObjIdent   objType );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** This small inline function returns the offset of the end block struct. */

static inline
ptrdiff_t psonEndBlockOffset( ptrdiff_t firstBlockOffset, 
                              size_t    numBlocks )
{
   return (firstBlockOffset + 
      (numBlocks <<  PSON_BLOCK_SHIFT) -
      PSON_ALLOCATION_UNIT);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psonEndBlockSet( ptrdiff_t firstBlockOffset, 
                      size_t    numBlocks, 
                      bool      limboStatus,
                      bool      lastBlock )
{
   psonEndBlockGroup* endBlock;
   
   GET_PTR( endBlock, 
            firstBlockOffset + (numBlocks <<  PSON_BLOCK_SHIFT) -
               PSON_ALLOCATION_UNIT,
            psonEndBlockGroup );
                       
   endBlock->firstBlockOffset = firstBlockOffset;
   endBlock->numBlocks = numBlocks;
   endBlock->isInLimbo = limboStatus;
   endBlock->lastBlock = lastBlock;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_BLOCK_GROUP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

