/*
 * Originally, code from the BGET allocator (http://www.fourmilab.ch/bget/) 
 * was used to develop the first version of the MemoryAllocator class. The
 * main modification was to replaced all pointers with offsets (we used 
 * offsets since the address of the shared memory can vary from process to
 * process - offsets to the start of the shared memory eliminates this issue).
 *
 * However, the complete rewrite of the allocator means that most of the code
 * was changed. There are still some ideas of bget in there (the linked
 * list of free buffers) but I'm not sure how would be able to recognize the 
 * bget code in there.
 *
 * -------------------------------------
 *
 * This is their (the BGET authors) original comments (in the .c file):
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
 * If there are still bits and pieces of code in this module belonging 
 * to the original bget code they still follow this license - of course!
 *
 * However, additions and modifications to the original code are covered 
 * by this copyright:
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

#ifndef VDSE_MEMORY_ALLOCATOR_H
#define VDSE_MEMORY_ALLOCATOR_H

#include "Engine.h"
#include "ErrorHandler.h"
#include "MemoryObject.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define SET_OFFSET(ptr) ( (ptrdiff_t) ( (unsigned char*)(ptr) - \
       g_pBaseAddr ) )

/* Only use this macro when you know, for a fact, that the offset cannot
 * be the NULL_OFFSET (for example, in the LinkedList class, the links
 * are never set to NULL_OFFSET...). 
 */
#define GET_PTR(off,class) ( (class*) (           \
       (unsigned char*) g_pBaseAddr + (ptrdiff_t) off ))

#define SET_PTR(target,offset,type)  \
   if ( offset == NULL_OFFSET ) \
      target = NULL; \
   else \
      target = (type*) (           \
         (unsigned char*) g_pBaseAddr + (ptrdiff_t) offset );

/* These next macros are only used inside this class and in the crash
 * recovery procedure (which is why they are in the .h file) 
 */

#define GET_FLINK(p) ( (struct bfhead *) \
   (g_pBaseAddr+(p)->ql.flink ) )

#define GET_BLINK(p) ( (struct bfhead *) \
   (g_pBaseAddr+(p)->ql.blink ) )

#define SET_LINK(p)  ( (size_t) \
   ( (unsigned char*)(p) - g_pBaseAddr ) )

#define NUM_OF_BITS_IN_BYTE 8

#if ! defined( LONG_LOCK_TIMEOUT)
#  define LONG_LOCK_TIMEOUT (100*LOCK_TIMEOUT)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef long bufsize_T;

struct qlinks {
   /** Forward link */
   ptrdiff_t flink;

   /** Backward link */
   ptrdiff_t blink;	      
};

/** Header in allocated and free buffers */
struct bhead 
{
   /** Relative link back to previous free buffer in memory or 0 if
       previous buffer is allocated.	*/
   bufsize_T prevfree;

   /** Buffer size: positive if free, negative if allocated. */
   bufsize_T bsize;		      
};

struct bfhead 
{
   /** Common allocated/free header */
   struct bhead bh;

   /** Links on free list */
   struct qlinks ql;
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseMemAlloc
{
   /**
    * The vdseMemAlloc is itself a memory object and "inherits" from
    * the memObj structure.
    */
   vdseMemObject memObj;
   
   /** Total space currently allocated */
   bufsize_T totalAlloc;   

   /** Number of bget() calls */
   size_t    numMalloc;

   /** Number of brel() calls */
   size_t    numFree;

   /**
    *  Total size of the buffer pool (aka the shared memory under control 
    *  of bget/this class).
    */
   bufsize_T poolLength;

   /** offset of the pool with respect to the whole shared memory "mmaped" */
   ptrdiff_t poolOffset;   

   vdscProcessLock lock;

   /** Structure used by bget */
   struct bfhead freeList;   

   /** 
    *  Buffer allocation size quantum. All buffers allocated are a
    *  multiple of this size.  This MUST be a power of two.  
    */
   bufsize_T sizeQuant;

} vdseMemAlloc;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** The malloc function of the bget allocator. */
void* vdseMemAllocbget ( vdseMemAlloc*    pAlloc,
                         bufsize_T        requestedSize,
                         vdscErrorHandler* pError );

/** The init function of the bget allocator. */
void vdseMemAllocbpool( vdseMemAlloc*    pAlloc,
                        unsigned char*   buf, 
                        bufsize_T        len,
                        vdscErrorHandler* pError );

/** The free function of the bget allocator. */
void  vdseMemAllocbrel ( vdseMemAlloc*    pAlloc,
                         void*            ptr,
                         vdscErrorHandler* pError );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Free ptr, the memory is returned to the pool. */
static inline
void vdseFree( vdseMemAlloc*    pAlloc,
               void *           ptr, 
               vdscErrorHandler* pError )
{
   int errcode = 0;

   VDS_ASSERT( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( ptr    != NULL );

   errcode = vdscTryAcquireProcessLock( &pAlloc->lock, getpid(), LONG_LOCK_TIMEOUT );
   if ( errcode == 0 )
   {
      vdseMemAllocbrel( pAlloc, ptr, pError );
      
      vdscReleaseProcessLock( &pAlloc->lock );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Allocate nbytes from our pool. The memory is not initialized. */
static inline
void *vdseMalloc( vdseMemAlloc*    pAlloc,
                  size_t           numBytes, 
                  vdscErrorHandler* pError )
{
   void* ptr = 0;
   int errcode = 0;

   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc != NULL );
   VDS_PRE_CONDITION( numBytes > 0 );

   errcode = vdscTryAcquireProcessLock( &pAlloc->lock, 
//                                 pError->pid, 
                                 getpid(),
                                 LONG_LOCK_TIMEOUT );
   if ( errcode == 0 )
   {
      ptr = vdseMemAllocbget( pAlloc, numBytes, pError );

      vdscReleaseProcessLock( &pAlloc->lock );
   }

   return ptr;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Close/end the use of the memory allocator.  */
void vdseMemAllocClose( vdseMemAlloc*    pAlloc,
                        vdscErrorHandler* pError );

/** Free all the floating buffers, if any */
void
vdseMemAllocFreeTheseBuffers( vdseMemAlloc*    pAlloc,
                              char**           pArrayBuffers, 
                              size_t           nNumOfAllocatedBuffer,
                              vdscErrorHandler* pError );

/** Populate an array of all allocated buffers (to recover all floating
 *  buffers). 
 */
void
vdseMemAllocGetAllocatedBuffers( vdseMemAlloc*    pAlloc,
                                 char**           pArrayBuffers, 
                                 size_t           nNumOfAllocatedBuffer,
                                 vdscErrorHandler* pError );

/** Initialize the vdseMemAlloc struct. The second argument is the start of
 *  the shared memory itself, the third indicates where the memory pool 
 *  starts (the start of the shared memory + the VDS header). 
 */
enum vdsErrors vdseMemAllocInit( vdseMemAlloc*    pAlloc,
                                 void*            pBaseAddress,
                                 unsigned char*   buffer, 
                                 size_t           length,
                                 vdscErrorHandler* pError );

/** Verify if a buffer is allocated or free. This function should 
 *  only be used to recover after a crash. Returns true if 
 *  the buffer is free, false otherwise.
 */
bool vdseMemAllocIsBufferFree( vdseMemAlloc*    pAlloc,
                          void*            buffer,
                          vdscErrorHandler* pError );

/** Reset statistics after a crash. */
void vdseMemAllocResetStats( vdseMemAlloc*    pAlloc,
                             vdscErrorHandler* pError );

/** Do we need this? */
static inline
void vdseSetBaseAddress( vdseMemAlloc*    pAlloc,
                         void*            pBaseAddress,
                         vdscErrorHandler* pError )
{
   VDS_PRE_CONDITION( pError != NULL );
   VDS_PRE_CONDITION( pAlloc       != NULL );
   VDS_PRE_CONDITION( pBaseAddress != NULL );
   
   g_pBaseAddr = (unsigned char*) pBaseAddress;
}

/** Returns status and statistics from the memory allocator. Note 
 *  that the number of mallocs/frees are not based on a 64 bits 
 *  integer on 32 bits machine - these numbers might loop around.
 */
vdsErrors vdseMemAllocStats( vdseMemAlloc*     pAlloc,
                             bufsize_T *       pCurrentAllocated,
                             bufsize_T *       pTotalFree,
                             bufsize_T *       pMaxFree,
                             size_t *          pNumberOfMallocs,
                             size_t *          pNumberOfFrees,
                             vdscErrorHandler* pError  );

/** Validate/test the content of the memory pool. */
int vdseMemAllocValidate( vdseMemAlloc*     pAlloc,
                          bool              verbose,
                          vdscErrorHandler* pError );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSE_MEMORY_ALLOCATOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
