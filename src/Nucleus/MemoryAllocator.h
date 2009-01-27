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

#ifndef PSON_MEMORY_ALLOCATOR_H
#define PSON_MEMORY_ALLOCATOR_H

#include "Nucleus/Engine.h"
#include "Common/ErrorHandler.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonMemAlloc
{
   /**
    * The psonMemAlloc is itself a memory object and "inherits" from
    * the memObj structure.
    */
   psonMemObject memObj;
   
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
   psonLinkedList freeList;

   ptrdiff_t bitmapOffset;
   
   /** 
    * The header of the group of blocks for this object. It MUST be
    * at the end of the struct since it contains a variable array.
    */
   psonBlockGroup blockGroup;
   
};

typedef struct psonMemAlloc psonMemAlloc;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the psonMemAlloc struct. The second argument is the start of
 * the shared memory itself. 
 *
 * This function should only be called by the server (it might move there
 * eventually). Reason: when a program access shared memory, the allocator is 
 * already there, initialized and all.
 */
PHOTON_ENGINE_EXPORT
enum psoErrors 
psonMemAllocInit( psonMemAlloc       * pAlloc,
                  unsigned char      * pBaseAddress, 
                  size_t               length,
                  psonSessionContext * pContext );

/**
 * 
 */
PHOTON_ENGINE_EXPORT unsigned char* 
psonMallocBlocks( psonMemAlloc       * pAlloc,
                  psonAllocTypeEnum    allocType,
                  size_t               numBlocks,
                  psonSessionContext * pContext );

/** Free ptr, the memory is returned to the pool. */
PHOTON_ENGINE_EXPORT
void psonFreeBlocks( psonMemAlloc       * pAlloc,
                     psonAllocTypeEnum    allocType,
                     unsigned char      * ptr, 
                     size_t               numBlocks,
                     psonSessionContext * pContext );

static inline
bool psonMemAllocLastBlock( psonMemAlloc * pAlloc,
                            ptrdiff_t      offset,
                            size_t         numBlocks )
{
   return ( pAlloc->totalLength <= (size_t)offset + (numBlocks << PSON_BLOCK_SHIFT));
}

PHOTON_ENGINE_EXPORT
void psonMemAllocClose( psonMemAlloc       * pAlloc,
                        psonSessionContext * pContext );

/** Returns status and statistics from the memory allocator. Note 
 *  that the number of mallocs/frees are not based on a 64 bits 
 *  integer on 32 bits machine - these numbers might loop around.
 */
PHOTON_ENGINE_EXPORT
bool psonMemAllocStats( psonMemAlloc       * pAlloc,
                        psoInfo            * pInfo,
                        psonSessionContext * pContext  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_MEMORY_ALLOCATOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

