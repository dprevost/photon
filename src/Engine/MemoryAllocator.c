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

/*  Header in directly allocated buffers  */

#define BH(p)	((struct bhead *) (p))
#define BFH(p)	((struct bfhead *) (p))

/*  Minimum allocation quantum: */

#define QLSIZE  ((bufsize_T)(sizeof(struct qlinks)))
#define SIZEQ   ((pAlloc->sizeQuant > QLSIZE) ? pAlloc->sizeQuant : QLSIZE)

/* End sentinel: value placed in bsize field of dummy block delimiting 
 * end of pool block.  The most negative number which will  fit  in  a 
 * bufsize_T, defined in a way that the compiler will accept. 
 * On 32bit architecture: -2147483648 or 0x80000000.
 */

#define ESENT   ((bufsize_T) (-(((1L << (sizeof(bufsize_T) * 8 - 2)) - 1) * 2) - 2))

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdsErrors 
vdseMemAllocInit( vdseMemAlloc*     pAlloc,
                  void*             pBaseAddress,
                  unsigned char*    buffer, 
                  size_t            length,
                  vdscErrorHandler* pError )
{
   enum vdsErrors errcode;

   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( pBaseAddress != NULL );
   VDS_PRE_CONDITION( buffer != NULL );
   VDS_PRE_CONDITION( length > 0 );

   errcode = vdseMemObjectInit( &pAlloc->memObj,                         
                                VDSE_IDENT_ALLOCATOR,
                                sizeof( struct vdseMemAlloc),
                                1 );
   if ( errcode != VDS_OK )
      return errcode;
   
   pAlloc->totalAlloc    = 0;
   pAlloc->numMalloc     = 0;
   pAlloc->numFree       = 0;
   pAlloc->poolLength    = 0;
   pAlloc->poolOffset    = NULL_OFFSET;
   pAlloc->sizeQuant     = 8;
   
   pAlloc->freeList.bh.prevfree = 0;
   pAlloc->freeList.bh.bsize    = 0;

   /* Must be initialized before being use in SET_LINK */
   g_pBaseAddr = (unsigned char*) pBaseAddress;

   pAlloc->freeList.ql.flink = SET_LINK( &pAlloc->freeList );
   pAlloc->freeList.ql.blink = SET_LINK( &pAlloc->freeList );

//   err =  vdscInitProcessLock( &pAlloc->lock );
//   if ( err != 0 )
//      return VDS_NOT_ENOUGH_RESOURCES;

   /* Can this failed? */
//   vdseMemAllocbpool( pAlloc, buffer, length, pError );

//   pAlloc->initialized = VDSC_MEM_ALLOC_SIGNATURE;

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0

/* Allocate <nbytes>, but don't give them any initial value. */
void* vdseMemAllocbget( vdseMemAlloc*    pAlloc,
                        bufsize_T        requestedSize,
                        vdscErrorHandler* pError )
{
   bufsize_T size = requestedSize;
   struct bfhead *b;
   void *buf;

   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_RETURN( pAlloc != NULL,    pError, NULL );
   VDS_ASSERT_RETURN( requestedSize > 0, pError, NULL );

   if (size < SIZEQ) 
   {                 /* Need at least room for the */
      size = SIZEQ;            /*    queue links.  */
   }
   size = (size + (pAlloc->sizeQuant - 1)) & (~(pAlloc->sizeQuant - 1));

   size += sizeof(struct bhead);     /* Add overhead in allocated buffer
                                         to size required. */
   
   b = GET_FLINK( &pAlloc->freeList );

   /* Scan the free list searching for the first buffer big enough
      to hold the requested size buffer. */

   while (b != &pAlloc->freeList) 
   {
      if ((bufsize_T) b->bh.bsize >= size) 
      {
         /* Buffer  is big enough to satisfy  the request.  Allocate it
            to the caller.  We must decide whether the buffer is  large
            enough  to  split  into  the part given to the caller and a
            free buffer that remains on the free list, or  whether  the
            entire  buffer  should  be  removed   from the free list and
            given to the caller in its entirety.   We  only  split  the
            buffer if enough room remains for a header plus the minimum
            quantum of allocation. */

         if ((b->bh.bsize - size) > (int) (SIZEQ + (sizeof(struct bhead)))) 
         {
            /* ba is the new allocated buffer */
            /* b is the remaining free buffer */
            /* bn is the following buffer  */
            struct bhead *ba, *bn;

            ba = BH(((unsigned char *) b) + (b->bh.bsize - size));
            bn = BH(((unsigned char *) ba) + size);
            VDS_ASSERT_RETURN( bn->prevfree == b->bh.bsize, pError, NULL );
            
            /* Subtract size from length of free block. */
            b->bh.bsize -= size;
            /* Link allocated buffer to the previous free buffer. */
            ba->prevfree = b->bh.bsize;
            /* Plug negative size into user buffer. */
            ba->bsize = -(bufsize_T) size;
            /* Mark buffer after this one not preceded by free block. */
            bn->prevfree = 0;

            pAlloc->totalAlloc += size;
            pAlloc->numMalloc++;        /* Increment number of bget() calls */

            buf = (void *) ((((unsigned char *) ba) + sizeof(struct bhead)));
            
            return buf;
         } 
         else 
         {
            /* The buffer isn't big enough to split.  Give  the  whole
               shebang to the caller and remove it from the free list. */

            struct bhead *ba;

            ba = BH(((unsigned char *) b) + b->bh.bsize);
            VDS_ASSERT_RETURN( ba->prevfree == b->bh.bsize, pError, NULL );

            VDS_ASSERT_RETURN( 
               GET_BLINK(b)->ql.flink == SET_LINK(b),
               pError,
               NULL );
            VDS_ASSERT_RETURN( 
               GET_FLINK(b)->ql.blink == SET_LINK(b),
               pError,
               NULL );

/*            b->ql.blink->ql.flink = b->ql.flink; */
            GET_BLINK(b)->ql.flink = b->ql.flink;
            GET_FLINK(b)->ql.blink = b->ql.blink;
/*            b->ql.flink->ql.blink = b->ql.blink; */

            pAlloc->totalAlloc += b->bh.bsize;
            pAlloc->numMalloc++;        /* Increment number of bget() calls */

            /* Negate size to mark buffer allocated. */
            b->bh.bsize = -(b->bh.bsize);

            /* Zero the back pointer in the next buffer in memory
               to indicate that this buffer is allocated. */
            ba->prevfree = 0;

            /* Give user buffer starting at queue links. */
            buf =  (void *) &(b->ql);

            return buf;
         }
      }
      b = GET_FLINK(b); /* Link to next buffer */
   }

   return NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMemAllocbpool( vdseMemAlloc*    pAlloc,
                        unsigned char*   buf, 
                        bufsize_T        len,
                        vdscErrorHandler* pError )
{
   struct bfhead *b = BFH(buf);
   struct bhead *bn;
 
   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc != NULL, pError );
   VDS_ASSERT_NORETURN( buf    != NULL, pError );

   pAlloc->poolOffset = SET_OFFSET(b);
   len &= ~(pAlloc->sizeQuant - 1);
   pAlloc->poolLength = len;

   /* Since the block is initially occupied by a single free  buffer,
      it  had   better   not  be  (much) larger than the largest buffer
      whose size we can store in bhead.bsize. */

   VDS_ASSERT_NORETURN( len - sizeof(struct bhead) <= -((bufsize_T) ESENT + 1),
                        pError );

   /* Clear  the  backpointer at  the start of the block to indicate that
      there  is  no  free  block  prior  to  this   one.    That   blocks
      recombination when the first block in memory is released. */

   b->bh.prevfree = 0;

   /* Chain the new block to the free list. */

   VDS_ASSERT_NORETURN( GET_BLINK(&pAlloc->freeList)->ql.flink == 
                        SET_LINK(&pAlloc->freeList),
                        pError );
   VDS_ASSERT_NORETURN( GET_FLINK(&pAlloc->freeList)->ql.blink == 
                        SET_LINK(&pAlloc->freeList),
                        pError );

   b->ql.flink = SET_LINK( &pAlloc->freeList ); /* &pAlloc->freeList */
   b->ql.blink = pAlloc->freeList.ql.blink;
   pAlloc->freeList.ql.blink = SET_LINK( b );
   GET_BLINK(b)->ql.flink = SET_LINK( b );

   /* Create a dummy allocated buffer at the end of the pool.   This dummy
      buffer is seen when a buffer at the end of the pool is released and
      blocks  recombination  of  the last buffer with the dummy buffer at
      the end.  The length in the dummy buffer  is  set  to  the  largest
      negative  number  to  denote  the  end  of  the pool for diagnostic
      routines (this specific value is  not  counted  on  by  the  actual
      allocation and release functions). */

   len -= sizeof(struct bhead);
   b->bh.bsize = (bufsize_T) len;
#ifdef FREE_WIPE
   memset(((unsigned char *) b) + sizeof(struct bfhead), 0x55,
          (size_t) (len - sizeof(struct bfhead)));
#endif
   bn = BH(((unsigned char *) b) + len);
   bn->prevfree = (bufsize_T) len;
   /* Definition of ESENT assumes two's complement! */
   VDS_ASSERT_NORETURN( (~0) == -1, pError );
   bn->bsize = ESENT;

   /* Add the dummy buffer to the sum of allocated space */
   pAlloc->totalAlloc = sizeof(struct bhead);
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseMemAllocbrel( vdseMemAlloc*    pAlloc,
                       void*            buf,
                       vdscErrorHandler* pError )
{
   struct bfhead *b, *bn;

   VDS_ASSERT( pError != NULL );
   VDS_ASSERT_NORETURN( pAlloc != NULL, pError );

   b = BFH(((unsigned char *) buf) - sizeof(struct bhead));

   VDS_ASSERT_NORETURN( buf != NULL, pError );
   if ( buf == NULL )
      return;

   pAlloc->numFree++;               /* Increment number of brel() calls */

   /* Buffer size must be negative, indicating that the buffer is allocated. */
   VDS_ASSERT_NORETURN( b->bh.bsize < 0, pError );

   /* Back pointer in next buffer must be zero, indicating the same thing: */
   VDS_ASSERT_NORETURN( BH((unsigned char *) b - b->bh.bsize)->prevfree == 0,
                        pError );

   pAlloc->totalAlloc += b->bh.bsize;
   VDS_ASSERT_NORETURN( pAlloc->totalAlloc >= 0, pError );

   /* If the back link is nonzero, the previous buffer is free.  */
   
   if (b->bh.prevfree != 0) 
   {
      /* The previous buffer is free.  Consolidate this buffer  with   it
         by  adding  the  length  of   this  buffer  to the previous free
         buffer.  Note that we subtract the size  in   the  buffer  being
         released,  since  it's  negative to indicate that the buffer is
         allocated. */

      register bufsize_T size = b->bh.bsize;

      /* Make the previous buffer the one we're working on. */
/*
      VDS_ASSERT_NORETURN(
         BH((unsigned char *) b - b->bh.prevfree)->bsize == b->bh.prevfree,
         pError ); 
*/
      if ( BH((unsigned char *) b - b->bh.prevfree)->bsize != b->bh.prevfree)
      {
         fprintf( stderr, " b , bh, etc... %p %p %ld %ld %ld\n", 
                  b, 
                  BH((unsigned char *) b - b->bh.prevfree),
                  BH((unsigned char *) b - b->bh.prevfree)->bsize,
                  b->bh.prevfree,
                  b->bh.bsize );
         VDS_ASSERT_NORETURN( 0, pError );
      }
      
      b = BFH(((unsigned char *) b) - b->bh.prevfree);
      b->bh.bsize -= size;
   } 
   else 
   {
      /* The previous buffer is allocated.  Insert this buffer
         on the free list as an isolated free block. */

      VDS_ASSERT_NORETURN( GET_BLINK(&pAlloc->freeList)->ql.flink == 
                           SET_LINK(&pAlloc->freeList),
                           pError );
      VDS_ASSERT_NORETURN( GET_FLINK(&pAlloc->freeList)->ql.blink == 
                           SET_LINK(&pAlloc->freeList),
                           pError );
/*        b->ql.flink = &pAlloc->freeList; */
/*        b->ql.blink = pAlloc->freeList.ql.blink; */
/*        pAlloc->freeList.ql.blink = b; */
/*        b->ql.blink->ql.flink = b; */

      b->ql.flink = SET_LINK( &pAlloc->freeList );
      b->ql.blink = pAlloc->freeList.ql.blink;
      pAlloc->freeList.ql.blink = SET_LINK( b );
      GET_BLINK(b)->ql.flink = SET_LINK( b );
      b->bh.bsize = -b->bh.bsize;
   }

   /* Now we look at the next buffer in memory, located by advancing from
      the  start  of  this  buffer  by its size, to see if that buffer is
      free.  If it is, we combine  this  buffer  with   the  next  one   in
      memory, dechaining the second buffer from the free list. */

   bn =  BFH(((unsigned char *) b) + b->bh.bsize);
   if (bn->bh.bsize > 0) 
   {

      /* The buffer is free.   Remove it from the free list and add
         its size to that of our buffer. */

      VDS_ASSERT_NORETURN(
         BH((unsigned char *) bn + bn->bh.bsize)->prevfree == bn->bh.bsize,
         pError );
/*      VDS_ASSERT_NORETURN( bn->ql.blink->ql.flink == bn, pError ); */
/*      VDS_ASSERT_NORETURN( bn->ql.flink->ql.blink == bn, pError ); */
      GET_BLINK( bn )->ql.flink = bn->ql.flink;
      GET_FLINK( bn )->ql.blink = bn->ql.blink;
/*        bn->ql.blink->ql.flink = bn->ql.flink; */
/*        bn->ql.flink->ql.blink = bn->ql.blink; */
      b->bh.bsize += bn->bh.bsize;

      /* Finally,  advance  to   the   buffer   that   follows   the  newly
         consolidated free block.  We must set its  backpointer  to  the
         head  of  the  consolidated free block.  We know the next block
         must be an allocated block because the process of recombination
         guarantees  that  two  free   blocks will never be contiguous in
         memory.  */

      bn = BFH(((unsigned char *) b) + b->bh.bsize);
   }
#ifdef FREE_WIPE
   memset(((unsigned char *) b) + sizeof(struct bfhead), 0x55,
          (size_t) (b->bh.bsize - sizeof(struct bfhead)));
#endif
   VDS_ASSERT_NORETURN( bn->bh.bsize < 0, pError );

   /* The next buffer is allocated.  Set the backpointer in it  to  point
      to this buffer; the previous free buffer in memory. */

   bn->bh.prevfree = b->bh.bsize;
}

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

