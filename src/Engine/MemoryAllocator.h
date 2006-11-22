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
#include "LinkedList.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDSC_MEM_ALLOC_SIGNATURE 0x1

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
   size_t totalAllocPages;   

   /** Number of malloc calls */
   size_t    numMallocCalls;

   /** Number of free calls */
   size_t    numFreeCalls;

   /**
    *  Total size of the shared memory.
    */
   size_t totalLength;

   /** Structure used to hold the list of free buffers. */
   vdseLinkedList freeList;
   
   size_t bitmapLength;
   unsigned char bitmap[1];
   
} vdseMemAlloc;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Initialize the vdseMemAlloc struct. The second argument is the start of
 * the shared memory itself. 
 *
 * This function should only be called by the watchdog (it might move there
 * eventually). Reason: when a program access the VDS, the allocator is 
 * already there, initialized and all.
 */
enum vdsErrors 
vdseMemAllocInit( vdseMemAlloc*     pAlloc,
                  unsigned char*    pBaseAddress, 
                  size_t            length,
                  vdscErrorHandler* pError );

/**
 * 
 */
void* vdseMallocPages( vdseMemAlloc*     pAlloc,
                       size_t            numPages,
                       vdscErrorHandler* pError );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Free ptr, the memory is returned to the pool. */
int vdseFreePages( vdseMemAlloc*     pAlloc,
                   void *            ptr, 
                   size_t            numPages,
                   vdscErrorHandler* pError );

void vdseMemAllocClose( vdseMemAlloc*     pAlloc,
                        vdscErrorHandler* pError );

/** Returns status and statistics from the memory allocator. Note 
 *  that the number of mallocs/frees are not based on a 64 bits 
 *  integer on 32 bits machine - these numbers might loop around.
 */
vdsErrors vdseMemAllocStats( vdseMemAlloc*     pAlloc,
                             size_t *          pCurrentAllocated,
                             size_t *          pTotalFree,
                             size_t *          pMaxFree,
                             size_t *          pNumberOfMallocs,
                             size_t *          pNumberOfFrees,
                             vdscErrorHandler* pError  );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Reset statistics after a crash. */
void vdseMemAllocResetStats( vdseMemAlloc*    pAlloc,
                             vdscErrorHandler* pError );


/** Validate/test the content of the memory pool. */
int vdseMemAllocValidate( vdseMemAlloc*     pAlloc,
                          bool              verbose,
                          vdscErrorHandler* pError );

#endif
                          
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSE_MEMORY_ALLOCATOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
