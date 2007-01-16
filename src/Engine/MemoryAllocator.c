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

#include "MemoryAllocator.h"

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
   size_t neededPages, neededBytes, bitmapLength;
   unsigned char* ptr;
   vdseMemBitmap* pBitmap = NULL;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc   != NULL );
   VDS_PRE_CONDITION( pBaseAddress != NULL );
   VDS_PRE_CONDITION( length >= (3 << VDSE_PAGE_SHIFT) );
   VDS_INV_CONDITION( g_pBaseAddr != NULL );

   pContext->pAllocator = (void*) pAlloc;
   /*
    * We need to calculate the following:
    *  1) the lenght of the allocator bitmap.bitmap which depends on the 
    *     total size of the sharedmemory.
    *  2) the number of pages required to hold this bitmap and wether this
    *     number of pages can also hold the alloc struct itself (let's
    *     not forget that the alloc struct has also a bitmap as part of
    *     the pageGroup struct).
    *  3) if this is too small, increment the number of pages by one.
    *
    * Not too complex.
    */

   /* Calculate the size of the bitmap of the allocator */
   bitmapLength = offsetof( vdseMemBitmap, bitmap ) + 
                  vdseGetBitmapLengthBytes( length, VDSE_PAGE_SIZE );
   /* Align it on VDSE_ALLOCATION_UNIT bytes boundary */
   bitmapLength = ( (bitmapLength - 1) / VDSE_ALLOCATION_UNIT + 1 ) * 
                 VDSE_ALLOCATION_UNIT;

   /* How many pages do we need, at a minimum, for the allocator */
   neededPages =  ((bitmapLength - 1) >> VDSE_PAGE_SHIFT) + 1;
   
   /* How many bytes do we need for the allocator */
   neededBytes = offsetof( struct vdseMemAlloc, pageGroup ) +
                 offsetof( vdsePageGroup, bitmap ) + 
                 offsetof( vdseMemBitmap, bitmap ) +
                 vdseGetBitmapLengthBytes( neededPages << VDSE_PAGE_SHIFT, 
                                           VDSE_ALLOCATION_UNIT );

   /* Align it on VDSE_ALLOCATION_UNIT bytes boundary */
   neededBytes = ( (neededBytes - 1) / VDSE_ALLOCATION_UNIT + 1 ) * 
                 VDSE_ALLOCATION_UNIT;

   /* So, enough space or not ? */
   if ( (neededPages << VDSE_PAGE_SHIFT) < (neededBytes + bitmapLength) )
      neededPages++;

   errcode = vdseMemObjectInit( &pAlloc->memObj,                         
                                VDSE_IDENT_ALLOCATOR,
                                neededPages );
   if ( errcode != VDS_OK )
      return errcode;
   
   vdsePageGroupInit( &pAlloc->pageGroup,
                      VDSE_PAGE_SIZE,
                      neededPages );

   /* Add the pageGroup to the list of groups of the memObject */
   vdseLinkedListPutFirst( &pAlloc->memObj.listPageGroup, 
                           &pAlloc->pageGroup.node );

   /* The overall header and the memory allocator itself */
   pAlloc->totalAllocPages = neededPages+1; 
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
                      VDSE_PAGE_SIZE );
   vdseSetBlocksAllocated( pBitmap, 0, (neededPages+1) << VDSE_PAGE_SHIFT );
   pAlloc->bitmapOffset = SET_OFFSET( pBitmap );
   
   /* Initialize the linked list */
   vdseLinkedListInit( &pAlloc->freeList );
   
   /* Now put the rest of the free pages in our free list */
   pNode = (vdseFreeBufferNode*)(pBaseAddress + ((neededPages+1) << VDSE_PAGE_SHIFT));
   pNode->numBlocks = (length >> VDSE_PAGE_SHIFT) - (neededPages+1);
   vdseLinkedListPutFirst( &pAlloc->freeList, &pNode->node );
   
   /*
    * Put the offset of the first free page on the last free page. This
    * makes it simpler/faster to rejoin groups of free pages. But only 
    * if the number of pages in the group > 1.
    */
   if ( pNode->numBlocks > 1 )
   {
      ptr = pBaseAddress + length - VDSE_PAGE_SIZE; 
      *((ptrdiff_t *)ptr) = (neededPages+1) << VDSE_PAGE_SHIFT;
   }
   
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
                                size_t            requestedPages,
                                vdscErrorHandler* pError )
{
   size_t i;
   /* size_t is unsigned. This is check by autoconf AC_TYPE_SIZE_T */
   size_t diff = (size_t) -1;
   size_t numPages;
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

   numPages = ((vdseFreeBufferNode*)oldNode)->numBlocks;
   if ( numPages == requestedPages )
   {
      /* Special case - perfect match */
      return (void*) oldNode;
   }
   if ( numPages > requestedPages )
   {
      if ( (numPages - requestedPages) < diff )
      {
         diff = numPages - requestedPages;
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
      numPages = ((vdseFreeBufferNode*)currentNode)->numBlocks;
      if ( numPages == requestedPages )
      {
         /* Special case - perfect match */
         return (void*) currentNode;
      }
      if ( numPages > requestedPages )
      {
         if ( (numPages - requestedPages) < diff )
         {
            diff = numPages - requestedPages;
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
      numPages = ((vdseFreeBufferNode*)currentNode)->numBlocks;
      if ( numPages >= requestedPages )
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
    * pages.
    */
   vdscSetError( pError, g_vdsErrorHandle, VDS_NOT_ENOUGH_MEMORY );

   return NULL;
}
                   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * Allocates the pages of shared memory we need.  
 */
void* vdseMallocPages( vdseMemAlloc*       pAlloc,
                       size_t              requestedPages,
                       vdseSessionContext* pContext )
{
   vdseFreeBufferNode* pNode = NULL;
   vdseFreeBufferNode* pNewNode = NULL;
   int errcode = 0;
   size_t newNumPages = 0;
   unsigned char* ptr;
   vdseMemBitmap* pBitmap;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( requestedPages > 0 );
   VDS_INV_CONDITION( g_pBaseAddr != NULL );

   errcode = vdscTryAcquireProcessLock( &pAlloc->memObj.lock, 
                                        getpid(),
                                        LONG_LOCK_TIMEOUT );
   if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return NULL;
   }
   
   pBitmap = GET_PTR( pAlloc->bitmapOffset, vdseMemBitmap );
   
   pNode = FindBuffer( pAlloc, requestedPages, &pContext->errorHandler );
   if ( pNode != NULL )
   {
      newNumPages = pNode->numBlocks - requestedPages;
      if ( newNumPages == 0 )
      {
         /* Remove the node from the list */
         vdseLinkedListRemoveItem( &pAlloc->freeList, &pNode->node );
      }
      else
      {
         pNewNode = (vdseFreeBufferNode*)
                    ((unsigned char*) pNode + (requestedPages << VDSE_PAGE_SHIFT));
         pNewNode->numBlocks = newNumPages;
         vdseLinkedListReplaceItem( &pAlloc->freeList, 
                                    &pNode->node, 
                                    &pNewNode->node );
         /*
          * Put the offset of the first free page on the last free page.
          * This makes it simpler/faster to rejoin groups of free pages. But 
          * only if the number of pages in the group > 1.
          */
          if ( newNumPages > 1 )
          {
             ptr = (unsigned char*) pNewNode + ((newNumPages-1) << VDSE_PAGE_SHIFT); 
             *((ptrdiff_t *)ptr) = SET_OFFSET(pNewNode);
          }
      }

      pAlloc->totalAllocPages += requestedPages;   
      pAlloc->numMallocCalls++;

      /* Set the bitmap */
      vdseSetBlocksAllocated( pBitmap, SET_OFFSET(pNode), 
                              requestedPages << VDSE_PAGE_SHIFT );
   }
   vdscReleaseProcessLock( &pAlloc->memObj.lock );

   return (void *)pNode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Free ptr, the memory is returned to the pool. */

int vdseFreePages( vdseMemAlloc*       pAlloc,
                   void *              ptr, 
                   size_t              numPages,
                   vdseSessionContext* pContext )
{
   int errcode = 0;
   vdseFreeBufferNode* otherNode;
   bool otherBufferisFree = false;
   unsigned char* p;
   ptrdiff_t offset;
   vdseMemBitmap* pBitmap;
   
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc   != NULL );
   VDS_PRE_CONDITION( ptr      != NULL );
   VDS_PRE_CONDITION( numPages > 0 );

   errcode = vdscTryAcquireProcessLock( &pAlloc->memObj.lock, getpid(), LONG_LOCK_TIMEOUT );
         if ( errcode != VDS_OK )
   {
      vdscSetError( &pContext->errorHandler, g_vdsErrorHandle, errcode );
      return -1;
   }

   pBitmap = GET_PTR( pAlloc->bitmapOffset, vdseMemBitmap );
   /* 
    * Check if the page before the current group-of-pages-to-be-released
    * is in the freeList or not.
    */
   p = (unsigned char*)ptr - VDSE_PAGE_SIZE;
   otherBufferisFree = vdseIsBlockFree( pBitmap, SET_OFFSET(p) );
   if ( otherBufferisFree )
   {
      /* Find the start of that free group of pages */
      if ( vdseIsBlockFree( pBitmap, 
         SET_OFFSET( (unsigned char*)ptr - (2 << VDSE_PAGE_SHIFT) ) ) )
      {
         /* The free group has more than one page */
         offset = *((ptrdiff_t*)p);
         p = GET_PTR(offset, unsigned char);
      }
      ((vdseFreeBufferNode*)p)->numBlocks += numPages;
   }
   else
   {
      /*
       * We make p the node, it could be the current group of pages or
       * the previous one.
       */
      p = (unsigned char*)ptr;
      ((vdseFreeBufferNode*)p)->numBlocks = numPages;
      vdseLinkedListPutLast( &pAlloc->freeList, 
                             &((vdseFreeBufferNode*)p)->node );
   }

   /* 
    * Check if the page after the current group-of-pages-to-be-released
    * is in the freeList or not.
    */
   otherNode = (vdseFreeBufferNode*)((unsigned char*)ptr + (numPages << VDSE_PAGE_SHIFT) );
   otherBufferisFree = vdseIsBlockFree( pBitmap, 
                                        SET_OFFSET(otherNode) );
   if ( otherBufferisFree )
   {
      ((vdseFreeBufferNode*)p)->numBlocks += otherNode->numBlocks;
      vdseLinkedListRemoveItem( &pAlloc->freeList, 
                                &otherNode->node );
      memset( otherNode, 0, sizeof(vdseFreeBufferNode) );
   }

   pAlloc->totalAllocPages -= numPages;   
   pAlloc->numFreeCalls++;

   /* Set the bitmap */
   vdseSetBlocksFree( pBitmap, SET_OFFSET(ptr), numPages << VDSE_PAGE_SHIFT );
   
   /*
    * Put the offset of the first free page on the last free page.
    * This makes it simpler/faster to rejoin groups of free pages. But 
    * only if the number of pages in the group > 1.
    */
   if ( ((vdseFreeBufferNode*)p)->numBlocks > 1 )
   {
      /* Warning - we reuse ptr here */
       ptr = p + ((((vdseFreeBufferNode*)p)->numBlocks-1) << VDSE_PAGE_SHIFT); 
       *((ptrdiff_t *)ptr) = SET_OFFSET(p);
   }
    
   vdscReleaseProcessLock( &pAlloc->memObj.lock );

   return 0;
}
  
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMemAllocClose( vdseMemAlloc*       pAlloc,
                        vdseSessionContext* pContext )
{
   VDS_PRE_CONDITION( pContext != NULL );
   VDS_PRE_CONDITION( pAlloc   != NULL );

   pAlloc->totalAllocPages = 0;
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
                                        LONG_LOCK_TIMEOUT );
   if ( errcode == 0 )
   {
      *pNumberOfMallocs  = pAlloc->numMallocCalls;
      *pNumberOfFrees    = pAlloc->numFreeCalls;
      *pCurrentAllocated = pAlloc->totalAllocPages << VDSE_PAGE_SHIFT;
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

