/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSN_MEMORY_ALLOCATOR_H
#define PSN_MEMORY_ALLOCATOR_H

#include "Nucleus/Engine.h"
#include "Common/ErrorHandler.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnMemAlloc
{
   /**
    * The psnMemAlloc is itself a memory object and "inherits" from
    * the memObj structure.
    */
   psnMemObject memObj;
   
   /** Total space currently allocated */
   size_t totalAllocBlocks;   

   /** Number of malloc calls */
   size_t numMallocCalls;

   /** Number of free calls */
   size_t numFreeCalls;

   /** Number of objects (API objects) */
   size_t numApiObjects;

   /** Number of groups of blocks */
   size_t numGroups;

   /** Total size of the shared memory. */
   size_t totalLength;

   /** Structure used to hold the list of free buffers. */
   psnLinkedList freeList;

   ptrdiff_t bitmapOffset;
   
   /** 
    * The header of the group of blocks for this object. It MUST be
    * at the end of the struct since it contains a variable array.
    */
   psnBlockGroup blockGroup;
   
};

typedef struct psnMemAlloc psnMemAlloc;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the psnMemAlloc struct. The second argument is the start of
 * the shared memory itself. 
 *
 * This function should only be called by the watchdog (it might move there
 * eventually). Reason: when a program access the VDS, the allocator is 
 * already there, initialized and all.
 */
VDSF_ENGINE_EXPORT
enum psoErrors 
psnMemAllocInit( psnMemAlloc       * pAlloc,
                  unsigned char      * pBaseAddress, 
                  size_t               length,
                  psnSessionContext * pContext );

/**
 * 
 */
VDSF_ENGINE_EXPORT unsigned char* 
psnMallocBlocks( psnMemAlloc       * pAlloc,
                  psnAllocTypeEnum    allocType,
                  size_t               numBlocks,
                  psnSessionContext * pContext );

/** Free ptr, the memory is returned to the pool. */
VDSF_ENGINE_EXPORT
void psnFreeBlocks( psnMemAlloc       * pAlloc,
                     psnAllocTypeEnum    allocType,
                     unsigned char      * ptr, 
                     size_t               numBlocks,
                     psnSessionContext * pContext );

static inline
bool psnMemAllocLastBlock( psnMemAlloc * pAlloc,
                            ptrdiff_t      offset,
                            size_t         numBlocks )
{
   return ( pAlloc->totalLength <= (size_t)offset + (numBlocks << PSN_BLOCK_SHIFT));
}

VDSF_ENGINE_EXPORT
void psnMemAllocClose( psnMemAlloc       * pAlloc,
                        psnSessionContext * pContext );

/** Returns status and statistics from the memory allocator. Note 
 *  that the number of mallocs/frees are not based on a 64 bits 
 *  integer on 32 bits machine - these numbers might loop around.
 */
VDSF_ENGINE_EXPORT
bool psnMemAllocStats( psnMemAlloc       * pAlloc,
                        psoInfo            * pInfo,
                        psnSessionContext * pContext  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_MEMORY_ALLOCATOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

