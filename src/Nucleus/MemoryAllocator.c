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

#include "Nucleus/MemoryAllocator.h"
#include "Common/Barrier.h"

/* 
 * group of blocks in limbo.
 *
 * A group of blocks in limbo is a group of blocks that could not be put
 * back in the free list of the memory allocator. Until it is put back in
 * the free list, it becomes unavailable (in limbo).
 *
 * The problem occurs during commit or rollback -  these two operations cannot
 * fail but that may not be able to get a lock on the memory allocator...
 * In these conditions (no lock), they have to release the memory in limbo.
 *
 * Notes:
 * Another strategy would be to get the lock on the memory allocator for
 * the duration of the transaction as a pre-condition but... this creates
 * other problems (contention for that lock -> performance bottleneck, the
 * automatic cleanup upon abnormal client termination (rollback) becomes
 * potentially problematic, etc.).
 *
 ************************************
 *
 * Groups of blocks in limbo are identifies with theit first and last 4 bytes
 * (the first bytes set to PSON_IDENT_LIMBO and the field isInLimbo set to 
 * true in the endblock).
 *
 * When a group of block is deallocated:
 *  - the object must be locked (or must be unreachable by using its name
 *    through its parent folder and its usage count must be zero).
 *  - call psonFreeBlocks()
 *      - if we get a lock on the allocator, go on as usual
 *      - if not
 *         - the bytes following the first bytes must be set with the number of 
 *           blocks in the group (using the psonFreeBlock struct).
  *        - set a write memory barrier to make sure that the stores will
 *           be properly ordered (in other words, to make sure that isInLimbo
 *           and PSON_IDENT_LIMBO are set last.
 *         - the first bytes must be set to PSON_IDENT_LIMBO.
 *         - set the field isInLimbo to true in the endblock.
 *           (the number of blocks should already be set)
 *  - unlock the object, if needed.
 * Results:
 *  - either we could lock the allocator and the group is now in the 
 *    freelist of the allocator (the best result)
 *  - or the group is still marked as allocated but has fields both at the
 *    beginning and the end that indicates that the group is not really
 *    allocated (in limbo). 
 * Warning: the issue: once a group is in limbo, you cannot access it. The
 *          last ops to definitely marked a group as in limbo must be the
 *          last operations on the block of memory (which is why we use a
 *          write memory barrier. Do not remove it!
 *
 * When a group of block is allocated:
 *  - We hold the lock so - no problem of synchronization should arise.
 *  - set the first bytes to PSON_IDENT_ALLOCATED so that the group cannot
 *    be mistaken as in limbo.
 *  - Set the isInLimbo field of the endBlock to false (just in case).
 *  - Set the bitmap of allocated blocks.
 *  - release the allocator lock.
 *
 * This is to avoid a potential race condition between the release of the
 * allocator lock and the initialization of the group of blocks. We want to
 * make sure that no other thread/process getting a hold of the allocator
 * might think that the group is in limbo.
 * Note: the beginning of the group should be a psonFreeBufferNode that
 *       cannot be confused with PSON_IDENT_LIMBO. But... 
 *        1 - one never knows if that code will always stay the same.
 *        2 - being explicit and setting PSON_IDENT_ALLOCATED cannot
 *            hurt and makes the code itself more clear.
 *        3 - and setting the fields like this might help debugging.
 *
 * When trying to regroup loose memory (in psonFreeBlocks):
 *  - if the group is marked as free, just do the normal stuff (concatenate
 *    the buffers).
 *  - when checking previous buffers marked as allocated, if the isInLimbo 
 *    field if true, concatenate.
 *  - when checking forward buffers, if the first bytes are PSON_IDENT_LIMBO,
 *    concatenate.
 *  - Put a read memory barrier between reading the first bytes (or last bytes)
 *    and reading anything else in the block (to make sure that the compiler 
 *    will not reorder the LOAD operations and prefetch data before the other
 *    thread/process write it).
 *    Example of why this is needed:
 *      cpu1 (has lock)       cpu2 (want to put blocks in limbo)
 *     
 *      read # of blocks in group
 *                            set # blocks in group
 *                            write memory barrier
 *                            set PSON_IDENT_LIMBO
 *      read PSON_IDENT_LIMBO
 *
 *    The point being that the cpu/compiler can reorder operations as 
 *    they see fit unless explicitely told not to (a read barrier forces
 *    the load of the # of blocks to be after the loads of either 
 *    PSON_IDENT_LIMBO or isInLimbo.
 *
 */
struct psonFreeBlock
{
    psonMemObjIdent identifier;
    size_t          numBlocks;
};

typedef struct psonFreeBlock psonFreeBlock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 *  The base address of the shared memory as seen for each process (each 
 *  process having their own copy of this global). This pointer is used
 *  everywhere to recover the real pointer addresses from our offsets
 */
PHOTON_ENGINE_EXPORT unsigned char * g_pBaseAddr = NULL;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoErrors 
psonMemAllocInit( psonMemAlloc       * pAlloc,
                  unsigned char      * pBaseAddress,
                  size_t               length,
                  psonSessionContext * pContext )
{
   enum psoErrors errcode;
   psonFreeBufferNode * pNode;
   size_t neededBlocks, neededBytes, bitmapLength;
   psonMemBitmap * pBitmap = NULL;
   
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pAlloc   != NULL );
   PSO_PRE_CONDITION( pBaseAddress != NULL );
   PSO_PRE_CONDITION( length >= (3 << PSON_BLOCK_SHIFT) );
   PSO_INV_CONDITION( g_pBaseAddr != NULL );

   pContext->pAllocator = (void *) pAlloc;
   /*
    * We need to calculate the following:
    *  1) the length of the allocator bitmap.bitmap which depends on the 
    *     total size of the sharedmemory.
    *  2) the number of blocks required to hold this bitmap and wether this
    *     number of blocks can also hold the alloc struct itself (let's
    *     not forget that the alloc struct has also a bitmap as part of
    *     the blockGroup struct).
    *  3) if this is too small, increment the number of blocks by one.
    *
    * Not too complex.
    */

   /* Calculate the size of the bitmap of the allocator */
   bitmapLength = offsetof( psonMemBitmap, bitmap ) + 
                  psonGetBitmapLengthBytes( length, PSON_BLOCK_SIZE );
   /* Align it on PSON_ALLOCATION_UNIT bytes boundary */
   bitmapLength = ( (bitmapLength - 1) / PSON_ALLOCATION_UNIT + 1 ) * 
                 PSON_ALLOCATION_UNIT;

   /* How many blocks do we need, at a minimum, for the allocator */
   neededBlocks =  ((bitmapLength - 1) >> PSON_BLOCK_SHIFT) + 1;
   
   /* How many bytes do we need for the allocator */
   neededBytes = offsetof( struct psonMemAlloc, blockGroup ) +
                 offsetof( psonBlockGroup, bitmap ) + 
                 offsetof( psonMemBitmap, bitmap ) +
                 psonGetBitmapLengthBytes( neededBlocks << PSON_BLOCK_SHIFT, 
                                           PSON_ALLOCATION_UNIT ) +
                 PSON_ALLOCATION_UNIT; /* The endBlock struct */

   /* Align it on PSON_ALLOCATION_UNIT bytes boundary */
   neededBytes = ( (neededBytes - 1) / PSON_ALLOCATION_UNIT + 1 ) * 
                 PSON_ALLOCATION_UNIT;

   /* So, enough space or not ? */
   if ( (neededBlocks << PSON_BLOCK_SHIFT) < (neededBytes + bitmapLength) ) {
      neededBlocks++;
   }
   
   errcode = psonMemObjectInit( &pAlloc->memObj,                         
                                PSON_IDENT_ALLOCATOR,
                                &pAlloc->blockGroup,
                                neededBlocks );
   if ( errcode != PSO_OK ) return errcode;
   
   psonEndBlockSet( SET_OFFSET(pAlloc), 
                    neededBlocks, 
                    false,   /* isInLimbo */
                    false ); /* is at the end of the shared memory */
                                              
   /* Add the blockGroup to the list of groups of the memObject */
   psonLinkNodeInit( &pAlloc->blockGroup.node );
   psonLinkedListPutFirst( &pAlloc->memObj.listBlockGroup, 
                           &pAlloc->blockGroup.node );

   /* The overall header and the memory allocator itself */
   pAlloc->totalAllocBlocks = neededBlocks+1; 
   pAlloc->numMallocCalls  = 0;
   pAlloc->numFreeCalls    = 0;
   pAlloc->numApiObjects   = 0;
   pAlloc->numGroups       = 1;
   pAlloc->totalLength     = length;

   /* Initialize the bitmap for the allocator itself */ 
   pBitmap = (psonMemBitmap*) psonMalloc( &pAlloc->memObj,
                                          bitmapLength,
                                          pContext );
   psonMemBitmapInit( pBitmap,
                      0,
                      pAlloc->totalLength,
                      PSON_BLOCK_SIZE );
   psonSetBufferAllocated( pBitmap, 0, (neededBlocks+1) << PSON_BLOCK_SHIFT );
   pAlloc->bitmapOffset = SET_OFFSET( pBitmap );
   
   /* Initialize the linked list */
   psonLinkedListInit( &pAlloc->freeList );
   
   /* Now put the rest of the free blocks in our free list */
   pNode = (psonFreeBufferNode*)(pBaseAddress + ((neededBlocks+1) << PSON_BLOCK_SHIFT));
   pNode->numBuffers = (length >> PSON_BLOCK_SHIFT) - (neededBlocks+1);
   psonLinkNodeInit( &pNode->node );
   psonLinkedListPutFirst( &pAlloc->freeList, &pNode->node );   
   
   psonEndBlockSet( SET_OFFSET(pNode), pNode->numBuffers, false, true );
   
   return PSO_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define BEST_FIT_MAX_LOOP 100
/**
 * This function was splitted from the psonMalloc() function in order 
 * to be able to test the algorithm without having to setup everything.
 * (And it becomes easier to replace the algorithm, if needed or to 
 * tweak it).
 */
static inline 
psonFreeBufferNode * FindBuffer( psonMemAlloc     * pAlloc,
                                 size_t             requestedBlocks,
                                 psocErrorHandler * pError )
{
   size_t i;
   /* size_t is unsigned. This is check by autoconf AC_TYPE_SIZE_T */
   size_t diff = (size_t) -1;
   size_t numBlocks;
   psonLinkNode * oldNode = NULL;
   psonLinkNode * currentNode = NULL;
   psonLinkNode * bestNode = NULL;
   bool ok;

   /* 
    * A bit tricky. To avoid fragmentation, we search for the best fitted
    * buffer in the first N buffers. By choosing a relative small N we 
    * make sur we don't loop around the whole free list. At the same time 
    * we avoid splitting up the larger buffers (which might be needed when
    * reallocating arrays...).[N is BEST_FIT_MAX_LOOP, a define just in
    * case the compiler can optimize the loop].
    */
   ok = psonLinkedListPeakFirst( &pAlloc->freeList, &oldNode );
   if ( ! ok ) goto error_exit;

   numBlocks = ((psonFreeBufferNode*)oldNode)->numBuffers;
   if ( numBlocks == requestedBlocks ) {
      /* Special case - perfect match */
      return (void*) oldNode;
   }
   if ( numBlocks > requestedBlocks ) {
      if ( (numBlocks - requestedBlocks) < diff ) {
         diff = numBlocks - requestedBlocks;
         bestNode = oldNode;
      }
   }
   
   for ( i = 0; i < BEST_FIT_MAX_LOOP; ++i ) {
      ok = psonLinkedListPeakNext( &pAlloc->freeList,
                                   oldNode,
                                   &currentNode );
      if ( ! ok ) {
         if ( bestNode == NULL ) goto error_exit;
         break;
      }
      numBlocks = ((psonFreeBufferNode*)currentNode)->numBuffers;
      if ( numBlocks == requestedBlocks ) {
         /* Special case - perfect match */
         return (void*) currentNode;
      }
      if ( numBlocks > requestedBlocks ) {
         if ( (numBlocks - requestedBlocks) < diff ) {
            diff = numBlocks - requestedBlocks;
            bestNode = currentNode;
         }
      }
      oldNode = currentNode;
   }
   
   while ( bestNode == NULL ) {
      ok = psonLinkedListPeakNext( &pAlloc->freeList,
                                   oldNode,
                                   &currentNode );
      if ( ! ok ) goto error_exit;

      numBlocks = ((psonFreeBufferNode*)currentNode)->numBuffers;
      if ( numBlocks >= requestedBlocks ) {
         bestNode = currentNode;
      }
      oldNode = currentNode;
   }
   
   return (psonFreeBufferNode*) bestNode;
   
 error_exit:
 
   /** 
    * \todo Eventually, it might be a good idea to separate "no memory"
    * versus a lack of a chunk big enough to accomodate the # of requested
    * blocks.
    */
   psocSetError( pError, g_psoErrorHandle, PSO_NOT_ENOUGH_PSO_MEMORY );

   return NULL;
}
                   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Allocates the blocks of shared memory we need.  
 */
unsigned char * psonMallocBlocks( psonMemAlloc       * pAlloc,
                                  psonAllocTypeEnum    allocType,
                                  size_t               requestedBlocks,
                                  psonSessionContext * pContext )
{
   psonFreeBufferNode * pNode = NULL;
   psonFreeBufferNode * pNewNode = NULL;
   size_t newNumBlocks = 0;
   psonMemBitmap * pBitmap;
   psonMemObjIdent * identifier;
   bool ok;
   
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pAlloc   != NULL );
   PSO_PRE_CONDITION( requestedBlocks > 0 );
   PSO_INV_CONDITION( g_pBaseAddr != NULL );

   ok = psocTryAcquireProcessLock( &pAlloc->memObj.lock, 
                                   getpid(),
                                   PSON_LOCK_TIMEOUT );
   PSO_POST_CONDITION( ok == true || ok == false );
   if ( ! ok ) {
//BUG?      psocSetError( &pContext->errorHandler, g_psoErrorHandle, errcode );
// engine busy vs not enough memory ... not the same thing!!!
      return NULL;
   }
   
   GET_PTR( pBitmap, pAlloc->bitmapOffset, psonMemBitmap );
   
   pNode = FindBuffer( pAlloc, requestedBlocks, &pContext->errorHandler );
   if ( pNode != NULL ) {
      newNumBlocks = pNode->numBuffers - requestedBlocks;
      if ( newNumBlocks == 0 ) {
         /* Remove the node from the list */
         psonLinkedListRemoveItem( &pAlloc->freeList, &pNode->node );
      }
      else {
         pNewNode = (psonFreeBufferNode*)
                    ((unsigned char*) pNode + (requestedBlocks << PSON_BLOCK_SHIFT));
         pNewNode->numBuffers = newNumBlocks;
         psonLinkNodeInit( &pNewNode->node );
         psonLinkedListReplaceItem( &pAlloc->freeList, 
                                    &pNode->node, 
                                    &pNewNode->node );
         
         /* Reset the psonEndBlock struct */
         psonEndBlockSet( SET_OFFSET(pNewNode), 
                          newNumBlocks, 
                          false,
                          psonMemAllocLastBlock( pAlloc,
                                                 SET_OFFSET(pNewNode),
                                                 newNumBlocks ) );
      }
      /* Set the first bytes to "allocated" and set the endBlock of the
       * newly allocated blocks.
       */
      identifier = (psonMemObjIdent *) pNode;
      *identifier = PSON_IDENT_ALLOCATED;
      psonEndBlockSet( SET_OFFSET(pNode), 
                       requestedBlocks, 
                       false,
                       psonMemAllocLastBlock( pAlloc,
                                              SET_OFFSET(pNode),
                                              requestedBlocks ) );
      
      pAlloc->totalAllocBlocks += requestedBlocks;   
      pAlloc->numMallocCalls++;
      pAlloc->numGroups++;
      if ( allocType == PSON_ALLOC_API_OBJ ) pAlloc->numApiObjects++;
      
      /* Set the bitmap */
      psonSetBufferAllocated( pBitmap, SET_OFFSET(pNode), 
                              requestedBlocks << PSON_BLOCK_SHIFT );
   }
   psocReleaseProcessLock( &pAlloc->memObj.lock );

   return (unsigned char *)pNode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Free ptr, the memory is returned to the pool. */

void psonFreeBlocks( psonMemAlloc       * pAlloc,
                     psonAllocTypeEnum    allocType,
                     unsigned char      * ptr, 
                     size_t               numBlocks,
                     psonSessionContext * pContext )
{
   psonFreeBufferNode * otherNode, * previousNode = NULL, * newNode = NULL;
   bool otherBufferisFree = false;
   psonMemBitmap * pBitmap;
   psonEndBlockGroup * endBlock;
   bool isInLimbo;
   psonMemObjIdent ident;
   psonFreeBlock * pFreeHeader;
   
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pAlloc   != NULL );
   PSO_PRE_CONDITION( ptr      != NULL );
   PSO_PRE_CONDITION( numBlocks > 0 );

   if ( ! psonLock( &pAlloc->memObj, pContext ) ) {
      /* 
       * So we did not get the lock and still need to free the memory.
       * We can't put the blocks back but we can put them in "limbo".
       *
       * Setting the field isInLimbo and PSON_IDENT_LIMBO
       * must be done after the memory barrier (to make sure they are the
       * last operations done on this piece of memory from the point of
       * view of other processes using the read barrier to access the
       * same data).
       */
      pFreeHeader = (psonFreeBlock*) ptr;
      endBlock = (psonEndBlockGroup *)
         (ptr + (numBlocks << PSON_BLOCK_SHIFT) - PSON_ALLOCATION_UNIT);

      pFreeHeader->numBlocks = numBlocks;
      
      psocWriteMemoryBarrier();
      pFreeHeader->identifier = PSON_IDENT_LIMBO;
      endBlock->isInLimbo = true;

      return;
   }

   GET_PTR( pBitmap, pAlloc->bitmapOffset, psonMemBitmap );
   newNode = (psonFreeBufferNode*)ptr;
   newNode->numBuffers = numBlocks;

   /*
    * We need to recover any memory left in limbo. We also must consolidate
    * free groups of blocks together in a single block.
    */

   /* 
    * Starts with the previous blocks. We use a do while loop since we know 
    * that the first group at the beginning of the shared memory is allocated
    * for the shared-memory header - no chance of going beyond the allocated
    * memory.
    */
   do {
      endBlock = (psonEndBlockGroup*)((unsigned char*)newNode-PSON_ALLOCATION_UNIT);
      isInLimbo = endBlock->isInLimbo;
      
      /* 
       * Impose a load (read) memory barrier before reading the location
       * of the start of the page group, etc.
       */
      psocReadMemoryBarrier();

      GET_PTR( previousNode, endBlock->firstBlockOffset, psonFreeBufferNode );
      otherBufferisFree = psonIsBufferFree( pBitmap, endBlock->firstBlockOffset );

      if ( isInLimbo ) {
         previousNode->numBuffers = newNode->numBuffers + endBlock->numBlocks;
         newNode = previousNode;
      }
      else if ( otherBufferisFree ) {
         /*
          * It might be better to remove this check later when the system
          * is all working and stable.
          */
         PSO_INV_CONDITION( previousNode->numBuffers == endBlock->numBlocks );
      
         /*
          * The previous node is already in our list of free blocks. We remove
          * it from the list (at the end we put back, in the list, the 
          * consolidated group).
          */
         previousNode->numBuffers += newNode->numBuffers;
         psonLinkedListRemoveItem( &pAlloc->freeList, 
                                   &previousNode->node );
         newNode = previousNode;
      }
      
   } while ( otherBufferisFree || isInLimbo );
   
   /* 
    * Now do the following group of blocks.
    *
    * Things we have to watchout for:
    *   - not going past the end of the shared memory...
    *   - if the group of block is allocated, test the first few bytes for
    *     the PSON_IDENT_LIMBO identifier.
    *
    * We use a while loop to test if the current group would not be the last
    * group before doing any data processing beyond the shared memory boundary.
    */

   otherNode = (psonFreeBufferNode*)
      ((unsigned char*)newNode + (newNode->numBuffers << PSON_BLOCK_SHIFT) );
   pFreeHeader = (psonFreeBlock*) otherNode;   
   /* 
    * At this point enBlock is set to the end of the current buffer, not to
    * the end of the next one (otherNode) we are looking at.
    */
   endBlock = (psonEndBlockGroup*)((unsigned char*)otherNode-PSON_ALLOCATION_UNIT);
      
   while ( ! endBlock->lastBlock ) {
      otherBufferisFree = psonIsBufferFree( pBitmap, SET_OFFSET(otherNode) );
      if ( otherBufferisFree ) {
         newNode->numBuffers += otherNode->numBuffers;
         psonLinkedListRemoveItem( &pAlloc->freeList, 
                                   &otherNode->node );
      }
      else {
         ident = pFreeHeader->identifier;

         /* 
          * Impose a load (read) memory barrier before reading the number
          * of blocks  in this group, etc.
          */
         psocReadMemoryBarrier();
         
         if ( ident == PSON_IDENT_LIMBO ) {
            newNode->numBuffers += pFreeHeader->numBlocks;
         }
         else {
            break;
         }
      }

      otherNode = (psonFreeBufferNode*)
         ((unsigned char*)newNode + (newNode->numBuffers << PSON_BLOCK_SHIFT) );
      pFreeHeader = (psonFreeBlock*) otherNode;   
      /* 
       * At this point enBlock is set to the end of the current consolidated
       * buffer, not to the end of the next one (otherNode) we'll be looking
       * at in the next iteration.
       */
      endBlock = (psonEndBlockGroup*)((unsigned char*)otherNode-PSON_ALLOCATION_UNIT);
   }
   /*
    * All consolidation done. Complete the job by putting the group in the
    * free list and setting the bitmap.
    */
   psonLinkNodeInit( &newNode->node );
   psonLinkedListPutLast( &pAlloc->freeList, &newNode->node );
                             
   pAlloc->totalAllocBlocks -= numBlocks;   
   pAlloc->numFreeCalls++;
   pAlloc->numGroups--;
   if ( allocType == PSON_ALLOC_API_OBJ ) pAlloc->numApiObjects--;

   /* Set the bitmap */
   psonSetBufferFree( pBitmap, SET_OFFSET(newNode), newNode->numBuffers << PSON_BLOCK_SHIFT );

   psonEndBlockSet( SET_OFFSET(newNode), 
                    newNode->numBuffers, 
                    false, /* limbo flag */
                    psonMemAllocLastBlock( pAlloc,
                                           SET_OFFSET(newNode),
                                           newNode->numBuffers ));
   
   psonUnlock( &pAlloc->memObj, pContext );

   return;
}
  
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonMemAllocClose( psonMemAlloc       * pAlloc,
                        psonSessionContext * pContext )
{
   PSO_PRE_CONDITION( pContext != NULL );
   PSO_PRE_CONDITION( pAlloc   != NULL );

   pAlloc->totalAllocBlocks = 0;
   pAlloc->numMallocCalls   = 0;
   pAlloc->numFreeCalls     = 0;
   pAlloc->totalLength      = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonMemAllocStats( psonMemAlloc       * pAlloc,
                        psoInfo            * pInfo,
                        psonSessionContext * pContext )
{
   size_t numBlocks;
   psonLinkNode *oldNode = NULL;
   psonLinkNode *currentNode = NULL;
   bool ok;
   
   PSO_PRE_CONDITION( pAlloc   != NULL );
   PSO_PRE_CONDITION( pInfo    != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   if ( psonLock( &pAlloc->memObj, pContext ) ) {

      pInfo->totalSizeInBytes     = pAlloc->totalLength;
      pInfo->allocatedSizeInBytes = pAlloc->totalAllocBlocks << PSON_BLOCK_SHIFT;
      pInfo->numObjects           = pAlloc->numApiObjects;
      pInfo->numGroups            = pAlloc->numGroups;
      pInfo->numMallocs           = pAlloc->numMallocCalls;
      pInfo->numFrees             = pAlloc->numFreeCalls;
      pInfo->largestFreeInBytes   = 0;
      
      ok = psonLinkedListPeakFirst( &pAlloc->freeList,
                                    &currentNode );
      while ( ok ) {
         numBlocks = ((psonFreeBufferNode*)currentNode)->numBuffers;
         if ( numBlocks > pInfo->largestFreeInBytes ) {
            pInfo->largestFreeInBytes = numBlocks;
         }
         
         oldNode = currentNode;
         ok = psonLinkedListPeakNext( &pAlloc->freeList,
                                      oldNode,
                                      &currentNode );
      }
      pInfo->largestFreeInBytes = pInfo->largestFreeInBytes << PSON_BLOCK_SHIFT;
      
      psonUnlock( &pAlloc->memObj, pContext );

      return true;
   }

   psocSetError( &pContext->errorHandler, g_psoErrorHandle, PSO_ENGINE_BUSY );
   return false;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

