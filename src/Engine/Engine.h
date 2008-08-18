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

#ifndef VDSE_ENGINE_H
#define VDSE_ENGINE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsCommon.h>
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  ifdef BUILD_VDSF_ENGINE
#    define VDSF_ENGINE_EXPORT __declspec ( dllexport )
#  else
#    define VDSF_ENGINE_EXPORT __declspec ( dllimport )
#  endif
#else
#  define VDSF_ENGINE_EXPORT
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * How do you identify NULL values using offsets instead of pointers? 
 * Tricky...
 * You cannot use the trick SET_OFFSET(NULL) because this set the value
 * to -base_address - which is only valid for a given process. 
 * You cannot use the value "0" because it is a valid offset. 
 * However the offset -1 (or 0xffffffff on 32 bits systems) can never
 * be reached (if ptrdiff_t is always the same number of bits as
 * a pointer - which is guaranteed by definition ...).
 */

 /* MAJOR concern: you cannot check if a reconstructed pointer is
  * NULL or not - this makes no sense anymore. You have to test 
  * the offset for VDSE_NULL_OFFSET !!! 
  */
#define VDSE_NULL_OFFSET ( (ptrdiff_t) -1 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 *  The base address of the shared memory as seen for each process (each 
 *  process having their own copy of this global). This pointer is used
 *  everywhere to recover the real pointer addresses from our offsets
 */
VDSF_ENGINE_EXPORT
extern unsigned char* g_pBaseAddr;

VDSF_ENGINE_EXPORT
extern pscErrMsgHandle g_vdsErrorHandle;

#define SET_OFFSET(ptr) ( (ptrdiff_t) ( (unsigned char*)(ptr) - \
       g_pBaseAddr ) )

/* Only use this macro when you know, for a fact, that the offset cannot
 * be the VDSE_NULL_OFFSET (for example, in the LinkedList "class", the links
 * are never set to VDSE_NULL_OFFSET...). 
 */
#define GET_PTR_FAST(off,class) ( (class*) (           \
       (unsigned char*) g_pBaseAddr + (ptrdiff_t) (off) ))

#define GET_PTR_OPT(target,offset,type) { \
   target = (type*) ( (unsigned char*) g_pBaseAddr + (ptrdiff_t) offset ); \
}

#define GET_PTR_DBG(target,offset,type) { \
   ptrdiff_t off = offset; \
   VDS_INV_CONDITION( off != 0 ); \
   VDS_INV_CONDITION( off != VDSE_NULL_OFFSET ); \
   target = (type*) ( (unsigned char*) g_pBaseAddr + (ptrdiff_t) off ); \
}

//#define GET_PTR(target,offset,type) GET_PTR_OPT(target,offset,type)
#define GET_PTR(target,offset,type) GET_PTR_DBG(target,offset,type)

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define LOCK_TIMEOUT 10000 /* in milliseconds */

#define VDSE_MAX_LOCK_DEPTH 3

/** Pages allocations will be done as multiples of VDSE_BLOCK_SIZE. */
#if ! defined(VDSE_BLOCK_SIZE)
#  define VDSE_BLOCK_SIZE   8192
#  define VDSE_BLOCK_SHIFT    13
#endif

/** Memory allocation will be done as multiples of VDSE_ALLOCATION_UNIT. */
#if SIZEOF_VOID_P == 4
#  define VDSE_ALLOCATION_UNIT 16
#elif SIZEOF_VOID_P == 8
#  define VDSE_ALLOCATION_UNIT 32
#else
#  error "Without a known SIZEOF_VOID_P (4 or 8) I cannot calculate VDSE_ALLOCATION_UNIT"
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * List of possible "type" identifiers for all objects allocated
 * in shared memory. Each of these objects "inherits" from MemoryObject
 * which sets the object identifier (as part of the initializer).
 * This is used, in part, by the crash recovery process
 * to validate/invalidate recovered buffers.
 *
 * Note: all the identifiers are odd numbers to distinguish them from
 * stray pointers/offsets (for debugging, if needed).
 */

enum vdseMemObjIdentifier
{
   /**
    * Special identifiers - it is set by the allocator when a group of blocks
    * is allocated (under lock). An "allocated" group with VDSE_IDENT_LIMBO
    * is a group in limbo while the same group with VDSE_IDENT_ALLOCATED
    * is not.
    */
   VDSE_IDENT_CLEAR           = 0xc1ea9001,
   VDSE_IDENT_LIMBO           = 0xccaaffee,
   VDSE_IDENT_ALLOCATED       = 0xeeffaacc,
   
   VDSE_IDENT_FIRST           = 0x34220101,
   VDSE_IDENT_ALLOCATOR       = 0x34220103,

   VDSE_IDENT_FOLDER          = 0x34220105,
   VDSE_IDENT_HASH_MAP        = 0x34220107,
   VDSE_IDENT_PROCESS         = 0x34220109,
   VDSE_IDENT_PROCESS_MGR     = 0x3422010b,
   VDSE_IDENT_QUEUE           = 0x3422010d,
   VDSE_IDENT_SESSION         = 0x3422010f,
   VDSE_IDENT_TRANSACTION     = 0x34220111,
   VDSE_IDENT_MAP             = 0x34220113,

   VDSE_IDENT_LAST            = 0x34220125

};

typedef enum vdseMemObjIdentifier vdseMemObjIdent;

#define VDSE_IDENT_PAGE_GROUP   0x80000000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This enum allows to count the number of objects and the number of 
 * extensions (additional groups of blocks added to an object). 
 */
enum vdseAllocTypeEnum
{
   /** When allocating/freeing the initial group of blocks of an API object */ 
   VDSE_ALLOC_API_OBJ,
   
   /** Any other group of blocks */
   VDSE_ALLOC_ANY
   
};

typedef enum vdseAllocTypeEnum vdseAllocTypeEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_ENGINE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

