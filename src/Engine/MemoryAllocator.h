/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_MEMORY_ALLOCATOR_H
#define VDSE_MEMORY_ALLOCATOR_H

#include "Engine.h"
#include "Common/ErrorHandler.h"
#include "MemoryObject.h"
#include "LinkedList.h"
#include "BlockGroup.h"
#include "SessionContext.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdseMemAlloc
{
   /**
    * The vdseMemAlloc is itself a memory object and "inherits" from
    * the memObj structure.
    */
   vdseMemObject memObj;
   
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
   vdseLinkedList freeList;

   ptrdiff_t bitmapOffset;
   
   /** 
    * The header of the group of blocks for this object. It MUST be
    * at the end of the struct since it contains a variable array.
    */
   vdseBlockGroup blockGroup;
   
};

typedef struct vdseMemAlloc vdseMemAlloc;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the vdseMemAlloc struct. The second argument is the start of
 * the shared memory itself. 
 *
 * This function should only be called by the watchdog (it might move there
 * eventually). Reason: when a program access the VDS, the allocator is 
 * already there, initialized and all.
 */
VDSF_ENGINE_EXPORT
enum vdsErrors 
vdseMemAllocInit( vdseMemAlloc       * pAlloc,
                  unsigned char      * pBaseAddress, 
                  size_t               length,
                  vdseSessionContext * pContext );

/**
 * 
 */
VDSF_ENGINE_EXPORT unsigned char* 
vdseMallocBlocks( vdseMemAlloc       * pAlloc,
                  vdseAllocTypeEnum    allocType,
                  size_t               numBlocks,
                  vdseSessionContext * pContext );

/** Free ptr, the memory is returned to the pool. */
VDSF_ENGINE_EXPORT
void vdseFreeBlocks( vdseMemAlloc       * pAlloc,
                     vdseAllocTypeEnum    allocType,
                     unsigned char      * ptr, 
                     size_t               numBlocks,
                     vdseSessionContext * pContext );

static inline
bool vdseMemAllocLastBlock( vdseMemAlloc * pAlloc,
                            ptrdiff_t      offset,
                            size_t         numBlocks )
{
   return ( pAlloc->totalLength <= (size_t)offset + (numBlocks << VDSE_BLOCK_SHIFT));
}

VDSF_ENGINE_EXPORT
void vdseMemAllocClose( vdseMemAlloc       * pAlloc,
                        vdseSessionContext * pContext );

/** Returns status and statistics from the memory allocator. Note 
 *  that the number of mallocs/frees are not based on a 64 bits 
 *  integer on 32 bits machine - these numbers might loop around.
 */
VDSF_ENGINE_EXPORT
int vdseMemAllocStats( vdseMemAlloc       * pAlloc,
                       vdsInfo            * pInfo,
                       vdseSessionContext * pContext  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_MEMORY_ALLOCATOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

