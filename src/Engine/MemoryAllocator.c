/*
 * Code from the BGET allocator (http://www.fourmilab.ch/bget/) was used
 * to develop this MemoryAllocator class.
 *
 * Designed and implemented in April of 1972 by John Walker, based on the
 * Case Algol OPRO$ algorithm implemented in 1966.
 *
 * Reimplemented in 1975 by John Walker for the Interdata 70.
 * Reimplemented in 1977 by John Walker for the Marinchip 9900.
 * Reimplemented in 1982 by Duff Kurland for the Intel 8080.
 *
 * Portable C version implemented in September of 1990 by an older, wiser
 * instance of the original implementor.
 *
 * Souped up and/or weighed down  slightly  shortly  thereafter  by  Greg
 * Lutz.
 *
 * AMIX  edition, including the new compaction call-back option, prepared
 * by John Walker in July of 1992.
 *
 * Bug in built-in test program fixed, ANSI compiler warnings eradicated,
 * buffer pool validator  implemented,  and  guaranteed  repeatable  test
 * added by John Walker in October of 1995.
 *
 * --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
 *
 * To quote the website:
 * "BGET is in the public domain. You can do anything you like with it."
 *
 * The original code still follow this license - of course!
 *
 * However, modifications to the original code are covered by this 
 * copyright:
 *
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#if 0
#  define FREE_WIPE    1         /* Wipe free buffers to a guaranteed
                                    pattern of garbage to trip up
                                    miscreants who attempt to use
                                    pointers into released buffers. */
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDSE_FREENODE_SIGNATURE 0x1fe76914

typedef struct vdseFreeBufferNode
{
   /* The linked list itself */
   vdseLinkNode node;
   
   /* The number of pages associate with each member of the list. */
   size_t numPages;
   
   int initialize;
   
} vdseFreeBufferNode;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool vdseIsPageFree( vdseMemAlloc*  pAlloc,
                     unsigned char* ptr )
{
   ptrdiff_t offset = ptr - g_pBaseAddr;
   size_t byte, bit;
   
   if ( offset < 0 || offset > pAlloc->totalLength )
      return false;
   
   byte = offset / PAGESIZE >> 3; // / 8;
//   bit = offset % 8;
   bit = offset & 7;
//   pAlloc->bitmap[byte] ;
//   ptrdiff_t
   
   return ( (pAlloc->bitmap[byte] & (unsigned char)(1 << bit)) == 0 );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
void vdseSetPagesAllocated( vdseMemAlloc*  pAlloc,
                            unsigned char* ptr,
                            size_t         numPages )
{
   ptrdiff_t offset = ptr - g_pBaseAddr;
   size_t byte, bit, i;
   
   byte = offset / PAGESIZE >> 3; // / 8;
//   bit = offset % 8;
   bit = offset & 7;
//   pAlloc->bitmap[byte] ;
//   ptrdiff_t
   for ( i = 0; i < numPages; ++i )
   {
      /* Setting the bit to one */
      pAlloc->bitmap[byte] |= (unsigned char)(1 << bit);
      bit++;
      if ( bit == 8 )
      {
         bit = 0;
         byte++;
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
void vdseSetPagesFree( vdseMemAlloc*  pAlloc,
                       unsigned char* ptr,
                       size_t         numPages )
{
   ptrdiff_t offset = ptr - g_pBaseAddr;
   size_t byte, bit, i;
   
   byte = offset / PAGESIZE >> 3; // / 8;
//   bit = offset % 8;
   bit = offset & 7;
//   pAlloc->bitmap[byte] ;
//   ptrdiff_t
   for ( i = 0; i < numPages; ++i )
   {
      /* Setting the bit to zero */
      pAlloc->bitmap[byte] &= (unsigned char)(~(1 << bit));
      bit++;
      if ( bit == 8 )
      {
         bit = 0;
         byte++;
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors 
vdseMemAllocInit( vdseMemAlloc*     pAlloc,
                  unsigned char*    pBaseAddress,
                  size_t            length,
                  vdscErrorHandler* pError )
{
   enum vdsErrors errcode;
   vdseFreeBufferNode* pNode;
   size_t neededPages, neededBytes;
   unsigned char* ptr;
   
   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( pBaseAddress != NULL );
   VDS_PRE_CONDITION( length >= 3*PAGESIZE );
   VDS_INV_CONDITION( g_pBaseAddr != NULL );

   /* We truncate it to amultiple of PAGESIZE */
   length = length / PAGESIZE * PAGESIZE;
   pAlloc->bitmapLength = (length/PAGESIZE - 1) / 8 + 1;
   
   /* How many pages do we need for the allocator */
   /* Note: "-1" because vdseMemAlloc already has bitmap[1] */
   neededBytes = pAlloc->bitmapLength + sizeof( struct vdseMemAlloc) - 1;
   /* Align it on VDSE_MEM_ALIGNMENT bytes boundary */
   neededBytes = ( (neededBytes - 1) / VDSE_MEM_ALIGNMENT + 1 ) * 
                 VDSE_MEM_ALIGNMENT;
   /* Always leave space for the navigator struct */
   neededBytes += sizeof(vdsePageNavig);
   neededPages = (neededBytes - 1)/PAGESIZE + 1;

   errcode = vdseMemObjectInit( &pAlloc->memObj,                         
                                VDSE_IDENT_ALLOCATOR,
                                pAlloc->bitmapLength + sizeof( struct vdseMemAlloc) - 1,
                                neededPages );
   if ( errcode != VDS_OK )
      return errcode;
   
   /* The overall header and the memory allocator itself */
   pAlloc->totalAllocPages = neededPages+1; 
   pAlloc->numMallocCalls  = 0;
   pAlloc->numFreeCalls    = 0;
   pAlloc->totalLength     = length;

   vdseLinkedListInit( &pAlloc->freeList );
   
   /* Now put the rest of the free pages in our free list */
   pNode = (vdseFreeBufferNode*)(pBaseAddress + (neededPages+1) * PAGESIZE);
   pNode->numPages = length / PAGESIZE - (neededPages+1);
   pNode->initialize = VDSE_FREENODE_SIGNATURE;
   vdseLinkedListPutFirst( &pAlloc->freeList, &pNode->node );
   
   /*
    * Put the offset of the first free page on the last free page. This
    * makes it simpler/faster to rejoin groups of free pages. But only 
    * if the number of pages in the group > 1.
    */
   if ( pNode->numPages > 1 )
   {
      ptr = pBaseAddress + length - PAGESIZE; 
      *((ptrdiff_t *)ptr) = (neededPages+1) * PAGESIZE;
   }
   /* Set the bitmap */
   memset( pAlloc->bitmap, 0, pAlloc->bitmapLength );
   vdseSetPagesAllocated( pAlloc, pBaseAddress, neededPages+1 );

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
   size_t diff = (size_t) -1;
   size_t numPages;
   vdseLinkNode *oldNode = NULL;
   vdseLinkNode *currentNode = NULL;
   vdseLinkNode *bestNode = NULL;
   enum ListErrors errcode;

   /** \todo Move the test for unsigned size_t to autoconf */ 
   VDS_INV_CONDITION( (size_t)-1 > (size_t) 0 );
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

   numPages = ((vdseFreeBufferNode*)oldNode)->numPages;
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
      numPages = ((vdseFreeBufferNode*)currentNode)->numPages;
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
      numPages = ((vdseFreeBufferNode*)currentNode)->numPages;
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
void * vdseMalloc( vdseMemAlloc*     pAlloc,
                   size_t            requestedPages,
                   vdscErrorHandler* pError )
{
   vdseFreeBufferNode* pNode = NULL;
   vdseFreeBufferNode* pNewNode = NULL;
   int errcode = 0;
   size_t newNumPages = 0;
   unsigned char* ptr;
   
   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( requestedPages > 0 );
   VDS_INV_CONDITION( g_pBaseAddr != NULL );

   errcode = vdscTryAcquireProcessLock( &pAlloc->memObj.lock, 
                                        getpid(),
                                        LONG_LOCK_TIMEOUT );
   if ( errcode != VDS_OK )
   {
      vdscSetError( pError, g_vdsErrorHandle, errcode );
      return NULL;
   }
   
   pNode = FindBuffer( pAlloc, requestedPages, pError );
   if ( pNode != NULL )
   {
      newNumPages = pNode->numPages - requestedPages;
      if ( newNumPages == 0 )
      {
         /* Remove the node from the list */
         vdseLinkedListRemoveItem( &pAlloc->freeList, &pNode->node );
      }
      else
      {
         pNewNode = (vdseFreeBufferNode*)
                    ((unsigned char*) pNode + (requestedPages*PAGESIZE));
         pNewNode->numPages = newNumPages;
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
             ptr = (unsigned char*) pNewNode + (newNumPages-1) * PAGESIZE; 
             *((ptrdiff_t *)ptr) = SET_OFFSET(pNewNode);
          }
      }

      pAlloc->totalAllocPages += requestedPages;   
      pAlloc->numMallocCalls++;

      /* Set the bitmap */
      vdseSetPagesAllocated( pAlloc, (unsigned char*)pNode, requestedPages );
   }
   vdscReleaseProcessLock( &pAlloc->memObj.lock );

   return (void *)pNode;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Free ptr, the memory is returned to the pool. */
static inline
int vdseFree( vdseMemAlloc*     pAlloc,
              void *            ptr, 
              size_t            numPages,
              vdscErrorHandler* pError )
{
   int errcode = 0;
   vdseFreeBufferNode* otherNode;
   bool otherBufferisFree = false;
   unsigned char* p;
   ptrdiff_t offset;
   
   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( ptr    != NULL );

   errcode = vdscTryAcquireProcessLock( &pAlloc->memObj.lock, getpid(), LONG_LOCK_TIMEOUT );
         if ( errcode != VDS_OK )
   {
      vdscSetError( pError, g_vdsErrorHandle, errcode );
      return -1;
   }

   /* 
    * Check if the page before the current group-of-pages-to-be-released
    * is in the freeList or not.
    */
   p = (unsigned char*)ptr - PAGESIZE;
   otherBufferisFree = vdseIsPageFree( pAlloc, p );
   if ( otherBufferisFree )
   {
      /* Find the start of that free group of pages */
      if ( vdseIsPageFree( pAlloc, (unsigned char*)ptr - 2*PAGESIZE ) )
      {
         /* The free group has more than one page */
         offset = *((ptrdiff_t*)p);
         p = GET_PTR(offset, unsigned char);
      }
      ((vdseFreeBufferNode*)p)->numPages += numPages;
   }
   else
   {
      /*
       * We make p the node, it could be the current group of pages or
       * the previous one.
       */
      p = (unsigned char*)ptr;
      ((vdseFreeBufferNode*)p)->numPages = numPages;
      ((vdseFreeBufferNode*)p)->initialize = VDSE_FREENODE_SIGNATURE;
   }

   /* 
    * Check if the page after the current group-of-pages-to-be-released
    * is in the freeList or not.
    */
   otherNode = (vdseFreeBufferNode*)((unsigned char*)ptr + numPages*PAGESIZE);
   otherBufferisFree = vdseIsPageFree( pAlloc, (unsigned char*) otherNode );
   if ( otherBufferisFree )
   {
      ((vdseFreeBufferNode*)p)->numPages += otherNode->numPages;
      vdseLinkedListReplaceItem( &pAlloc->freeList, 
                                 &otherNode->node, 
                                 &((vdseFreeBufferNode*)p)->node );
      memset( otherNode, 0, sizeof(vdseFreeBufferNode) );
   }

   pAlloc->totalAllocPages -= numPages;   
   pAlloc->numFreeCalls++;

   /* Set the bitmap */
   vdseSetPagesFree( pAlloc, ptr, numPages );
   
   /*
    * Put the offset of the first free page on the last free page.
    * This makes it simpler/faster to rejoin groups of free pages. But 
    * only if the number of pages in the group > 1.
    */
   if ( ((vdseFreeBufferNode*)p)->numPages > 1 )
   {
      /* Warning - we reuse ptr here */
       ptr = p + (numPages-1) * PAGESIZE; 
       *((ptrdiff_t *)ptr) = SET_OFFSET(p);
   }
    
   vdscReleaseProcessLock( &pAlloc->memObj.lock );

   return 0;
}
  
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
                   
#if 0

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMemAllocClose( vdseMemAlloc*    pAlloc,
                        vdscErrorHandler* pError )
{
   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc != NULL, pError );

   pAlloc->totalAlloc    = 0;
   pAlloc->numMalloc     = 0;
   pAlloc->numFree       = 0;
   pAlloc->poolLength    = 0;
   pAlloc->poolOffset    = NULL_OFFSET;
   pAlloc->sizeQuant     = 8;
   
   pAlloc->freeList.bh.prevfree = 0;
   pAlloc->freeList.bh.bsize    = 0;

   g_pBaseAddr = NULL;

   pAlloc->freeList.ql.flink = NULL_OFFSET;
   pAlloc->freeList.ql.blink = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Free all the floating buffers */
void vdseMemAllocFreeTheseBuffers( vdseMemAlloc*    pAlloc,
                                   char**           pArrayBuffers, 
                                   size_t           numOfAllocatedBuffer,
                                   vdscErrorHandler* pError )
{
   size_t sum = 0;
   struct bfhead *b = NULL;
   size_t i, size;
   
   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc        != NULL, pError );
   VDS_ASSERT_NORETURN( pArrayBuffers != NULL, pError );
   if ( pAlloc->numMalloc > pAlloc->numFree )
      size = pAlloc->numMalloc - pAlloc->numFree;
   else
      size = (size_t)-1 - pAlloc->numFree + pAlloc->numMalloc;
   VDS_ASSERT_NORETURN( numOfAllocatedBuffer == size, pError );
   
   for ( i = 0; i < numOfAllocatedBuffer; ++i )
   {
      if ( pArrayBuffers[i] != NULL )
      {
         b = BFH(((unsigned char *)pArrayBuffers[i] ) - sizeof(struct bhead));
         /* Verify if a buffer is allocated or free. */
         if ( ! vdseMemAllocIsBufferFree( pAlloc, pArrayBuffers[i], pError ) )
            vdseMemAllocbrel( pAlloc, pArrayBuffers[i], pError );
         else
/*              fprintf( stderr, " pArrayBuffer[%d]  = %p size = %ld\n", i,  */
/*                       pArrayBuffers[i], */
/*                       b->bh.bsize  ); */
            sum++;
      }
   }
   fprintf( stderr, "FreeTheseBuffers: %d %d\n", sum, numOfAllocatedBuffer );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Populate an array of all allocated buffers (to recover all floating */
/* buffers). */
void 
vdseMemAllocGetAllocatedBuffers( vdseMemAlloc*    pAlloc,
                                 char**           pArrayBuffers, 
                                 size_t           numOfAllocatedBuffer,
                                 vdscErrorHandler* pError )
{
   struct bfhead *b = NULL, *bn = NULL;
   size_t i = 0;
   bufsize_T size;
   
   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc        != NULL, pError );
   VDS_ASSERT_NORETURN( pArrayBuffers != NULL, pError );
   if ( pAlloc->numMalloc > pAlloc->numFree )
      size = pAlloc->numMalloc - pAlloc->numFree;
   else
      size = (size_t)-1 - pAlloc->numFree + pAlloc->numMalloc;
   VDS_ASSERT_NORETURN( numOfAllocatedBuffer == size, pError );
   
   /* Loop on each buffer */

   b = GET_PTR( pAlloc->poolOffset, struct bfhead );
                  size = b->bh.bsize;
   
   while ( size != ESENT )
   {
      if ( size > 0 ) /* free buffer */
      {
         /* Calculate where the next buffer is */
         bn =  BFH(((unsigned char *) b) + b->bh.bsize);
      }
      else /* used buffer */
      {
         if ( i == numOfAllocatedBuffer )
         {
            fprintf( stderr, "GetAllocatedBuffers overflow!!!\n" );
            return;
         }
         pArrayBuffers[i] = (char*) &(b->ql);
         i++;

         /* Calculate where the next buffer is */
         bn =  BFH(((unsigned char *) b) - b->bh.bsize);         
      }
      b = bn;
      size = b->bh.bsize;
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Verify if a buffer is allocated or free. */
bool
vdseMemAllocIsBufferFree( vdseMemAlloc*    pAlloc,
                          void*            buffer,
                          vdscErrorHandler* pError )
{
   struct bfhead *pBuf;

   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc != NULL, pError );
   VDS_ASSERT_NORETURN( buffer != NULL, pError );

   pBuf = BFH(((unsigned char *) buffer) - sizeof(struct bhead));

   /* Since free buffers can be consolidated with either the one */
   /* before or the one after (if they are free themselves), the sole */
   /* indication of "bh.bsize >= 0" is not enough to decide if a buffer */
   /* is free or not. */

   /* In fact the "buffer" might now be a part of another buffer which */
   /* itself is part of another buffer... */

   /* What we need to do is walk the list of all known blocks to search  */
   /* for the given buffer - if it not there it ain't allocated! */
   /* And if it is there, the test "bh.bsize >= 0" will test for us if it */ 
   /* free or not */

   /* Sadly, this is a bit slow... */

   struct bfhead *b, *bn;

   b = GET_PTR( pAlloc->poolOffset, struct bfhead );
   bufsize_T size = b->bh.bsize;
   
   while ( size != ESENT )
   {
      if ( b == pBuf )
      {
         if ( size > 0 )
            return true;
         return false;
      }

      if ( size > 0 ) /* free buffer */
         bn =  BFH(((unsigned char *) b) + b->bh.bsize);
      else
         bn =  BFH(((unsigned char *) b) - b->bh.bsize);

      b = bn;
      size = b->bh.bsize;
   }

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Reset statistics after a crash */
void vdseMemAllocResetStats( vdseMemAlloc*    pAlloc,
                             vdscErrorHandler* pError )
{
   struct bfhead *b = NULL, *bn = NULL;

   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc != NULL, pError );

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseMemAllocStats( vdseMemAlloc*    pAlloc,
                             bufsize_T *      pCurrentAllocated,
                             bufsize_T *      pTotalFree,
                             bufsize_T *      pMaxFree,
                             size_t *         pNumberOfMallocs,
                             size_t *         pNumberOfFrees,
                             vdscErrorHandler* pError )
{
   int errcode = 0;

   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc            != NULL, pError );
   VDS_ASSERT_NORETURN( pCurrentAllocated != NULL, pError );
   VDS_ASSERT_NORETURN( pTotalFree        != NULL, pError );
   VDS_ASSERT_NORETURN( pMaxFree          != NULL, pError );
   VDS_ASSERT_NORETURN( pNumberOfMallocs  != NULL, pError );
   VDS_ASSERT_NORETURN( pNumberOfFrees    != NULL, pError );

   errcode = vdscTryAcquireProcessLock( &pAlloc->lock, 
//                                        pError->pid, 
                                        getpid(),
                                        LONG_LOCK_TIMEOUT );
   if ( errcode == 0 )
   {
      *pNumberOfMallocs  = pAlloc->numMalloc;
      *pNumberOfFrees    = pAlloc->numFree;
      *pCurrentAllocated = pAlloc->totalAlloc;
      *pTotalFree        = 0;
      *pMaxFree          = -1;

      struct bfhead * b = GET_FLINK( &pAlloc->freeList );
      while (b != &pAlloc->freeList)
      {
         VDS_ASSERT_NORETURN( b->bh.bsize > 0, pError );
         *pTotalFree += b->bh.bsize;
         if (b->bh.bsize > *pMaxFree)
            *pMaxFree = b->bh.bsize;
         b = GET_FLINK( b );
      }
      vdscReleaseProcessLock( &pAlloc->lock );

      return VDS_OK;
   }

   return VDS_ENGINE_BUSY;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseMemAllocValidate( vdseMemAlloc*     pAlloc,
                          bool              verbose,
                          vdscErrorHandler* pError )
{
   struct bfhead *b = NULL, *bn = NULL;

   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_RETURN( pAlloc != NULL, pError, VDS_INTERNAL_ERROR );

   b = &pAlloc->freeList;
   fprintf( stderr, "%p %10ld %10ld %10ld %10d %10d\n", 
            b, 
            (long)((unsigned char*)b-g_pBaseAddr),
            b->bh.prevfree, 
            b->bh.bsize,
            b->ql.flink, 
            b->ql.blink );

   /* Start with the linked list */
   b = GET_FLINK( &pAlloc->freeList );
   while (b != &pAlloc->freeList) 
   {
/*
      VDS_ASSERT_RETURN( GET_BLINK(b)->ql.flink == SET_LINK(b),
                         pError, VDS_INTERNAL_ERROR );
      VDS_ASSERT_RETURN( GET_FLINK(b)->ql.blink == SET_LINK(b), 
                         pError, VDS_INTERNAL_ERROR );
      VDS_ASSERT_RETURN( b->bh.bsize >= 0, pError, VDS_INTERNAL_ERROR );
      VDS_ASSERT_RETURN( b->bh.prevfree == 0, pError, VDS_INTERNAL_ERROR );
*/
      if ( GET_BLINK(b)->ql.flink != SET_LINK(b) )
         return -1;
      if ( GET_FLINK(b)->ql.blink != SET_LINK(b) )
         return -2;
      if ( b->bh.bsize < 0 )
         return -3;
      if ( b->bh.prevfree != 0 )
         return -4;

      b = GET_FLINK(b);        /* Link to next buffer */
   }

   /* Loop on each buffer */

   b = GET_PTR( pAlloc->poolOffset, struct bfhead );
   bufsize_T size = b->bh.bsize;
   
   while ( size != ESENT )
   {
      if ( verbose)
      {
         if ( size > 0 ) /* free buffer */
            fprintf( stderr, "%p %10ld %10ld %10ld %10d %10d\n", 
                     b, 
                     (long)((unsigned char*)b-g_pBaseAddr),
                     b->bh.prevfree, 
                     size,
                     b->ql.flink, 
                     b->ql.blink );
         else
            fprintf( stderr, "%p %10ld %10ld %10ld\n", 
                     b, 
                     (long)((unsigned char*)b-g_pBaseAddr),
                     b->bh.prevfree, 
                     size );
      }
      
      if (b->bh.prevfree != 0) 
         if ( BH((unsigned char *) b - b->bh.prevfree)->bsize != b->bh.prevfree )
         {
            fprintf( stderr, " z3 %p %p %ld %ld\n", b, bn, size, 
                     bn->bh.prevfree );
            return -5;
         }
      
      if ( size > 0 ) /* free buffer */
      {
         bn =  BFH(((unsigned char *) b) + b->bh.bsize);
/*           VDS_ASSERT_RETURN( bn->bh.prevfree == b->bh.bsize, 
                                pError, VDS_INTERNAL_ERROR ); */
         if ( bn->bh.prevfree != b->bh.bsize )
         {
            fprintf( stderr, " z1 %p %p %ld %ld \n", b, bn, size, 
                    bn->bh.prevfree );
            return -6;
         }
         /* 2 free consecutive buffers... */
/*       VDS_ASSERT_RETURN( bn->bh.bsize < 0, pError, VDS_INTERNAL_ERROR ); */
         if ( bn->bh.bsize >= 0 ) /* 2 free consecutive buffers... */
            
         {
            fprintf( stderr, " z2 %p %p %ld %ld\n", b, bn, size, bn->bh.prevfree );
            return -7;
         }
      }
      else
      {
         bn =  BFH(((unsigned char *) b) - b->bh.bsize);
/*       VDS_ASSERT_RETURN( bn->bh.prevfree == 0, pError, VDS_INTERNAL_ERROR );
*/
         if ( bn->bh.prevfree != 0 )
         {
            fprintf( stderr, " z3 %p %p %ld %ld\n", b, bn, size, bn->bh.prevfree );
            return -8;
         }
      }
      b = bn;
      size = b->bh.bsize;
   }

   fprintf( stderr, "Validate ok !\n" );
   return 0;
}

#endif
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

