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

#include "Engine/MemoryAllocator.h"
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
 * (the first bytes set to VDSE_IDENT_LIMBO and the field isInLimbo set to 
 * true in the endblock).
 *
 * When a group of block is deallocated:
 *  - the object must be locked (or must be unreachable by using its name
 *    through its parent folder and its usage count must be zero).
 *  - call vdseFreeBlocks()
 *      - if we get a lock on the allocator, go on as usual
 *      - if not
 *         - the bytes following the first bytes must be set with the number of 
 *           blocks in the group (using the vdseFreeBlock struct).
  *        - set a write memory barrier to make sure that the stores will
 *           be properly ordered (in other words, to make sure that isInLimbo
 *           and VDSE_IDENT_LIMBO are set last.
 *         - the first bytes must be set to VDSE_IDENT_LIMBO.
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
 *  - set the first bytes to VDSE_IDENT_ALLOCATED so that the group cannot
 *    be mistaken as in limbo.
 *  - Set the isInLimbo field of the endBlock to false (just in case).
 *  - Set the bitmap of allocated blocks.
 *  - release the allocator lock.
 *
 * This is to avoid a potential race condition between the release of the
 * allocator lock and the initialization of the group of blocks. We want to
 * make sure that no other thread/process getting a hold of the allocator
 * might think that the group is in limbo.
 * Note: the beginning of the group should be a vdseFreeBufferNode that
 *       cannot be confused with VDSE_IDENT_LIMBO. But... 
 *        1 - one never knows if that code will always stay the same.
 *        2 - being explicit and setting VDSE_IDENT_ALLOCATED cannot
 *            hurt and makes the code itself more clear.
 *        3 - and setting the fields like this might help debugging.
 *
 * When trying to regroup loose memory (in vdseFreeBlocks):
 *  - if the group is marked as free, just do the normal stuff (concatenate
 *    the buffers).
 *  - when checking previous buffers marked as allocated, if the isInLimbo 
 *    field if true, concatenate.
 *  - when checking forward buffers, if the first bytes are VDSE_IDENT_LIMBO,
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
 *                            set VDSE_IDENT_LIMBO
 *      read VDSE_IDENT_LIMBO
 *
 *    The point being that the cpu/compiler can reorder operations as 
 *    they see fit unless explicitely told not to (a read barrier forces
 *    the load of the # of blocks to be after the loads of either 
 *    VDSE_IDENT_LIMBO or isInLimbo.
 *
 */

typedef struct vdseFreeBlock
{
    enum ObjectIdentifier identifier;
    size_t numBlocks;
} vdseFreeBlock;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 *  The base address of the shared memory as seen for each process (each 
 *  process having their own copy of this global). This pointer is used
 *  everywhere to recover the real pointer addresses from our offsets
 */
unsigned char* g_pBaseAddr = NULL;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors 
vdseMemAllocInit( vdseMemAlloc*       pAlloc,
                  unsigned char*      pBaseAddress,
                  size_t              length,
                  vdseSessionContext* pContext )
{
   enum vdsErrors errcode;
   vdseFreeBufferNode* pNode;
   size_t neededBlocks, neededBytes, bitmapLength;
   vdseMemBitmap* pBitmap = NULL;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc   != NULL );
   VDS_PRE_CONDITION( pBaseAddress != NULL );
   VDS_PRE_CONDITION( length >= (3 << VDSE_BLOCK_SHIFT) );
   VDS_INV_CONDITION( g_pBaseAddr != NULL );

   pContext->pAllocator = (void*) pAlloc;
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
   bitmapLength = offsetof( vdseMemBitmap, bitmap ) + 
                  vdseGetBitmapLengthBytes( length, VDSE_BLOCK_SIZE );
   /* Align it on VDSE_ALLOCATION_UNIT bytes boundary */
   bitmapLength = ( (bitmapLength - 1) / VDSE_ALLOCATION_UNIT + 1 ) * 
                 VDSE_ALLOCATION_UNIT;

   /* How many blocks do we need, at a minimum, for the allocator */
   neededBlocks =  ((bitmapLength - 1) >> VDSE_BLOCK_SHIFT) + 1;
   
   /* How many bytes do we need for the allocator */
   neededBytes = offsetof( struct vdseMemAlloc, blockGroup ) +
                 offsetof( vdseBlockGroup, bitmap ) + 
                 offsetof( vdseMemBitmap, bitmap ) +
                 vdseGetBitmapLengthBytes( neededBlocks << VDSE_BLOCK_SHIFT, 
                                           VDSE_ALLOCATION_UNIT ) +
                 VDSE_ALLOCATION_UNIT; /* The endBlock struct */

   /* Align it on VDSE_ALLOCATION_UNIT bytes boundary */
   neededBytes = ( (neededBytes - 1) / VDSE_ALLOCATION_UNIT + 1 ) * 
                 VDSE_ALLOCATION_UNIT;

   /* So, enough space or not ? */
   if ( (neededBlocks << VDSE_BLOCK_SHIFT) < (neededBytes + bitmapLength) )
      neededBlocks++;

   errcode = vdseMemObjectInit( &pAlloc->memObj,                         
                                VDSE_IDENT_ALLOCATOR,
                                &pAlloc->blockGroup,
                                neededBlocks );
   if ( errcode != VDS_OK )
      return errcode;
   
//   vdseBlockGroupInit( &pAlloc->blockGroup,
//                       VDSE_BLOCK_SIZE, /* offset of the allocator */
//                       neededBlocks );
   vdseEndBlockSet( SET_OFFSET(pAlloc), 
                    neededBlocks, 
                    false,   /* isInLimbo */
                    false ); /* is at the end of the VDS */
                                              
   /* Add the blockGroup to the list of groups of the memObject */
   vdseLinkedListPutFirst( &pAlloc->memObj.listBlockGroup, 
                           &pAlloc->blockGroup.node );

   /* The overall header and the memory allocator itself */
   pAlloc->totalAllocBlocks = neededBlocks+1; 
   pAlloc->numMallocCalls  = 0;
   pAlloc->numFreeCalls    = 0;
   pAlloc->totalLength     = length;

   /* Initialize the bitmap for the allocator itself */ 
   pBitmap = (vdseMemBitmap*) vdseMalloc( &pAlloc->memObj,
                                          bitmapLength,
                                          pContext );
   vdseMemBitmapInit( pBitmap,
                      0,
                      pAlloc->totalLength,
                      VDSE_BLOCK_SIZE );
   vdseSetBufferAllocated( pBitmap, 0, (neededBlocks+1) << VDSE_BLOCK_SHIFT );
   pAlloc->bitmapOffset = SET_OFFSET( pBitmap );
   
   /* Initialize the linked list */
   vdseLinkedListInit( &pAlloc->freeList );
   
   /* Now put the rest of the free blocks in our free list */
   pNode = (vdseFreeBufferNode*)(pBaseAddress + ((neededBlocks+1) << VDSE_BLOCK_SHIFT));
   pNode->numBuffers = (length >> VDSE_BLOCK_SHIFT) - (neededBlocks+1);
   vdseLinkedListPutFirst( &pAlloc->freeList, &pNode->node );   
   
   vdseEndBlockSet( SET_OFFSET(pNode), pNode->numBuffers, false, true );
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define BEST_FIT_MAX_LOOP 100
/**
 * This function was splitted from the vdseMalloc() function in order 
 * to be able to test the algorithm without having to setup everything.
 * (And it becomes easier to replace the algorithm, if needed or to 
 * tweak it).
 */
static inline 
vdseFreeBufferNode* FindBuffer( vdseMemAlloc*     pAlloc,
                                size_t            requestedBlocks,
                                vdscErrorHandler* pError )
{
   size_t i;
   /* size_t is unsigned. This is check by autoconf AC_TYPE_SIZE_T */
   size_t diff = (size_t) -1;
   size_t numBlocks;
   vdseLinkNode *oldNode = NULL;
   vdseLinkNode *currentNode = NULL;
   vdseLinkNode *bestNode = NULL;
   enum ListErrors errcode;

   /* 
    * A bit tricky. To avoid fragmentation, we search for the best fitted
    * buffer in the first N buffers. By choosing a relative small N we 
    * make sur we don't loop around the whole free list. At the same time 
    * we avoid splitting up the larger buffers (which might be needed when
    * reallocating arrays...).[N is BEST_FIT_MAX_LOOP, a define just in
    * case the compiler can optimize the loop].
    */
   errcode = vdseLinkedListPeakFirst( &pAlloc->freeList,
                                      &oldNode );
   if ( errcode != LIST_OK )
      goto error_exit;

   numBlocks = ((vdseFreeBufferNode*)oldNode)->numBuffers;
   if ( numBlocks == requestedBlocks )
   {
      /* Special case - perfect match */
      return (void*) oldNode;
   }
   if ( numBlocks > requestedBlocks )
   {
      if ( (numBlocks - requestedBlocks) < diff )
      {
         diff = numBlocks - requestedBlocks;
         bestNode = oldNode;
      }
   }
   
   for ( i = 0; i < BEST_FIT_MAX_LOOP; ++i )
   {
      errcode = vdseLinkedListPeakNext( &pAlloc->freeList,
                                        oldNode,
                                        &currentNode );
      if ( errcode != LIST_OK ) 
      {
         if ( bestNode == NULL ) goto error_exit;
         break;
      }
      numBlocks = ((vdseFreeBufferNode*)currentNode)->numBuffers;
      if ( numBlocks == requestedBlocks )
      {
         /* Special case - perfect match */
         return (void*) currentNode;
      }
      if ( numBlocks > requestedBlocks )
      {
         if ( (numBlocks - requestedBlocks) < diff )
         {
            diff = numBlocks - requestedBlocks;
            bestNode = currentNode;
         }
      }
      oldNode = currentNode;
   }
   
   while ( bestNode == NULL )
   {
      errcode = vdseLinkedListPeakNext( &pAlloc->freeList,
                                        oldNode,
                                        &currentNode );
      if ( errcode != LIST_OK ) 
      {
         goto error_exit;
      }
      numBlocks = ((vdseFreeBufferNode*)currentNode)->numBuffers;
      if ( numBlocks >= requestedBlocks )
      {
         bestNode = currentNode;
      }
      oldNode = currentNode;
   }
   
   return (vdseFreeBufferNode*) bestNode;
   
 error_exit:
 
   /** 
    * \todo Eventually, it might be a good idea to separate "no memory"
    * versus a lack of a chunk big enough to accomodate the # of requested
    * blocks.
    */
   vdscSetError( pError, g_vdsErrorHandle, VDS_NOT_ENOUGH_VDS_MEMORY );

   return NULL;
}
                   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Allocates the blocks of shared memory we need.  
 */
unsigned char* vdseMallocBlocks( vdseMemAlloc*       pAlloc,
                                 size_t              requestedBlocks,
                                 vdseSessionContext* pContext )
{
   vdseFreeBufferNode* pNode = NULL;
   vdseFreeBufferNode* pNewNode = NULL;
   int errcode = 0;
   size_t newNumBlocks = 0;
   vdseMemBitmap* pBitmap;
   enum ObjectIdentifier* identifier;

   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( requestedBlocks > 0 );
   VDS_INV_CONDITION( g_pBaseAddr != NULL );

   errcode = vdscTryAcquireProcessLock( &pAlloc->memObj.lock, 
                                        getpid(),
                                        LOCK_TIMEOUT );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return NULL;
   }
   
   pBitmap = GET_PTR( pAlloc->bitmapOffset, vdseMemBitmap );
   
   pNode = FindBuffer( pAlloc, requestedBlocks, &pContext->errorHandler );
   if ( pNode != NULL )
   {
      newNumBlocks = pNode->numBuffers - requestedBlocks;
      if ( newNumBlocks == 0 )
      {
         /* Remove the node from the list */
         vdseLinkedListRemoveItem( &pAlloc->freeList, &pNode->node );
      }
      else
      {
         pNewNode = (vdseFreeBufferNode*)
                    ((unsigned char*) pNode + (requestedBlocks << VDSE_BLOCK_SHIFT));
         pNewNode->numBuffers = newNumBlocks;
         vdseLinkedListReplaceItem( &pAlloc->freeList, 
                                    &pNode->node, 
                                    &pNewNode->node );
         
         /* Reset the vdseEndBlock struct */
         vdseEndBlockSet( SET_OFFSET(pNewNode), 
                          newNumBlocks, 
                          false,
                          vdseMemAllocLastBlock( pAlloc,
                                                 SET_OFFSET(pNewNode),
                                                 newNumBlocks ) );
      }
      /* Set the first bytes to "allocated" and set the endBlock of the
       * newly allocated blocks.
       */
      identifier = (enum ObjectIdentifier *) pNode;
      *identifier = VDSE_IDENT_ALLOCATED;
      vdseEndBlockSet( SET_OFFSET(pNode), 
                       requestedBlocks, 
                       false,
                       vdseMemAllocLastBlock( pAlloc,
                                              SET_OFFSET(pNode),
                                              requestedBlocks ) );
      
      pAlloc->totalAllocBlocks += requestedBlocks;   
      pAlloc->numMallocCalls++;

      /* Set the bitmap */
      vdseSetBufferAllocated( pBitmap, SET_OFFSET(pNode), 
                              requestedBlocks << VDSE_BLOCK_SHIFT );
   }
   vdscReleaseProcessLock( &pAlloc->memObj.lock );

   return (unsigned char *)pNode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Free ptr, the memory is returned to the pool. */

void vdseFreeBlocks( vdseMemAlloc*       pAlloc,
                     unsigned char *     ptr, 
                     size_t              numBlocks,
                     vdseSessionContext* pContext )
{
   int errcode = 0;
   vdseFreeBufferNode* otherNode, *previousNode = NULL, *newNode = NULL;
   bool otherBufferisFree = false;
   vdseMemBitmap* pBitmap;
   vdseEndBlockGroup* endBlock;
   bool isInLimbo;
   enum ObjectIdentifier ident;
   vdseFreeBlock* pFreeHeader;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc   != NULL );
   VDS_PRE_CONDITION( ptr      != NULL );
   VDS_PRE_CONDITION( numBlocks > 0 );

   errcode = vdseLock( &pAlloc->memObj, SET_OFFSET(pAlloc), pContext );
   if ( errcode != 0 )
   {
      /* 
       * Potential race conditions here if the memory is accessed beyond
       * this point. Setting the field isInLimbo and VDSE_IDENT_LIMBO
       * must be done after the memory barrier (to make sure they are the
       * last operations done on this piece of memory).
       */
      pFreeHeader = (vdseFreeBlock*) ptr;
      endBlock = (vdseEndBlockGroup *)
         (ptr + (numBlocks << VDSE_BLOCK_SHIFT) - VDSE_ALLOCATION_UNIT);

      pFreeHeader->numBlocks = numBlocks;
      
      vdscWriteMemoryBarrier();
      pFreeHeader->identifier = VDSE_IDENT_LIMBO;
      endBlock->isInLimbo = true;

      return;
   }

   pBitmap = GET_PTR( pAlloc->bitmapOffset, vdseMemBitmap );
   newNode = (vdseFreeBufferNode*)ptr;
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
   do
   {
      endBlock = (vdseEndBlockGroup*)((unsigned char*)newNode-VDSE_ALLOCATION_UNIT);
      isInLimbo = endBlock->isInLimbo;
      
      /* 
       * Impose a load (read) memory barrier before reading the location
       * of the start of the page group, etc.
       */
      vdscReadMemoryBarrier();

      previousNode = GET_PTR( endBlock->firstBlockOffset, vdseFreeBufferNode );
      otherBufferisFree = vdseIsBufferFree( pBitmap, endBlock->firstBlockOffset );

      if ( isInLimbo )
      {
         previousNode->numBuffers = newNode->numBuffers + endBlock->numBlocks;
         newNode = previousNode;
      }
      else if ( otherBufferisFree )
      {
         /*
          * It might be better to remove this check later when the system
          * is all working and stable.
          */
         VDS_INV_CONDITION( previousNode->numBuffers == endBlock->numBlocks );
      
         /*
          * The previous node is already in our list of free blocks. We remove
          * it from the list (at the end we put back, in the list, the 
          * consolidated group).
          */
         previousNode->numBuffers += newNode->numBuffers;
         vdseLinkedListRemoveItem( &pAlloc->freeList, 
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
    *     the VDSE_IDENT_LIMBO identifier.
    *
    * We use a while loop to test if the current group would not be the last
    * group before doing any data processing beyond the shared memory boundary.
    */

   otherNode = (vdseFreeBufferNode*)
      ((unsigned char*)newNode + (newNode->numBuffers << VDSE_BLOCK_SHIFT) );
   pFreeHeader = (vdseFreeBlock*) otherNode;   
   /* 
    * At this point enBlock is set to the end of the current buffer, not to
    * the end of the next one (otherNode) we are looking at.
    */
   endBlock = (vdseEndBlockGroup*)((unsigned char*)otherNode-VDSE_ALLOCATION_UNIT);
      
   while ( ! endBlock->lastBlock )
   {
      otherBufferisFree = vdseIsBufferFree( pBitmap, SET_OFFSET(otherNode) );
      if ( otherBufferisFree )
      {
         newNode->numBuffers += otherNode->numBuffers;
         vdseLinkedListRemoveItem( &pAlloc->freeList, 
                                   &otherNode->node );
      }
      else
      {
         ident = pFreeHeader->identifier;

         /* 
          * Impose a load (read) memory barrier before reading the number
          * of blocks  in this group, etc.
          */
         vdscReadMemoryBarrier();
         
         if ( ident == VDSE_IDENT_LIMBO )
            newNode->numBuffers += pFreeHeader->numBlocks;
         else
            break;
      }

      otherNode = (vdseFreeBufferNode*)
         ((unsigned char*)newNode + (newNode->numBuffers << VDSE_BLOCK_SHIFT) );
      pFreeHeader = (vdseFreeBlock*) otherNode;   
      /* 
       * At this point enBlock is set to the end of the current consolidated
       * buffer, not to the end of the next one (otherNode) we'll be looking
       * at in the next iteration.
       */
      endBlock = (vdseEndBlockGroup*)((unsigned char*)otherNode-VDSE_ALLOCATION_UNIT);
   }
   /*
    * All consolidation done. Complete the job by putting the group in the
    * free list and setting the bitmap.
    */
   vdseLinkedListPutLast( &pAlloc->freeList, &newNode->node );
                             
   pAlloc->totalAllocBlocks -= numBlocks;   
   pAlloc->numFreeCalls++;

   /* Set the bitmap */
   vdseSetBufferFree( pBitmap, SET_OFFSET(newNode), newNode->numBuffers << VDSE_BLOCK_SHIFT );

   vdseEndBlockSet( SET_OFFSET(newNode), 
                    newNode->numBuffers, 
                    false, /* limbo flag */
                    vdseMemAllocLastBlock( pAlloc,
                                           SET_OFFSET(newNode),
                                           newNode->numBuffers ));
   
   vdseUnlock( &pAlloc->memObj, pContext );

   return;
}
  
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMemAllocClose( vdseMemAlloc*       pAlloc,
                        vdseSessionContext* pContext )
{
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc   != NULL );

   pAlloc->totalAllocBlocks = 0;
   pAlloc->numMallocCalls  = 0;
   pAlloc->numFreeCalls    = 0;
   pAlloc->totalLength     = 0;
//   pAlloc-> bitmapLength   = 0;
//   pAlloc->freeList.bh.prevfree = 0;
//   pAlloc->freeList.bh.bsize    = 0;

//   g_pBaseAddr = NULL;

}

#if 0

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Reset statistics after a crash */
void vdseMemAllocResetStats( vdseMemAlloc*    pAlloc,
                             vdscErrorHandler* pError )
{
   struct bfhead *b = NULL, *bn = NULL;

   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL, pError );

   pAlloc->totalAlloc = 0;
   pAlloc->numMalloc = 0;
   pAlloc->numFree = 0;
  
   /* Loop on each buffer */

   b = GET_PTR( pAlloc->poolOffset, struct bfhead );
   bufsize_T size = b->bh.bsize;
   
   while ( size != ESENT )
   {
      if ( size > 0 ) /* free buffer */
      {
         /* Calculate where the next buffer is */
         bn =  BFH(((unsigned char *) b) + b->bh.bsize);
      }
      else /* used buffer */
      {
         /* Increment pAlloc->numMalloc so that the difference between  */
         /* # of malloc and # of free == the number of allocated buffers! */
         pAlloc->numMalloc++; 
         
         pAlloc->totalAlloc-= size;
         
         /* Calculate where the next buffer is */
         bn =  BFH(((unsigned char *) b) - b->bh.bsize);         
      }
      b = bn;
      size = b->bh.bsize;
   }

   /* Add the dummy buffer to the sum of allocated space */
   pAlloc->totalAlloc += sizeof(struct bhead);
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseMemAllocStats( vdseMemAlloc*       pAlloc,
                             size_t *            pCurrentAllocated,
                             size_t *            pTotalFree,
                             size_t *            pMaxFree,
                             size_t *            pNumberOfMallocs,
                             size_t *            pNumberOfFrees,
                             vdseSessionContext* pContext )
{
   int errcode = 0;

   VDS_PRE_CONDITION( pContext          != NULL );
   VDS_PRE_CONDITION( pAlloc            != NULL );
   VDS_PRE_CONDITION( pCurrentAllocated != NULL );
   VDS_PRE_CONDITION( pTotalFree        != NULL );
   VDS_PRE_CONDITION( pMaxFree          != NULL );
   VDS_PRE_CONDITION( pNumberOfMallocs  != NULL );
   VDS_PRE_CONDITION( pNumberOfFrees    != NULL );

   errcode = vdscTryAcquireProcessLock( &pAlloc->memObj.lock, 
                                        getpid(),
                                        LOCK_TIMEOUT );
   if ( errcode == 0 )
   {
      *pNumberOfMallocs  = pAlloc->numMallocCalls;
      *pNumberOfFrees    = pAlloc->numFreeCalls;
      *pCurrentAllocated = pAlloc->totalAllocBlocks << VDSE_BLOCK_SHIFT;
      *pTotalFree        = 0;
      *pMaxFree          = 0;

/*      struct bfhead * b = GET_FLINK( &pAlloc->freeList );
      while (b != &pAlloc->freeList)
      {
         VDS_ASSERT_NORETURN( b->bh.bsize > 0, pError );
         *pTotalFree += b->bh.bsize;
         if (b->bh.bsize > *pMaxFree)
            *pMaxFree = b->bh.bsize;
         b = GET_FLINK( b );
      }
      */
      vdscReleaseProcessLock( &pAlloc->memObj.lock );

      return VDS_OK;
   }

   return VDS_ENGINE_BUSY;
}

#if 0
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMemAllocValidate( vdseMemAlloc*     pAlloc,
                          bool              verbose,
                          vdscErrorHandler* pError )
{
   fprintf( stderr, "Validate ok !\n" );
   return 0;
}

#endif
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

