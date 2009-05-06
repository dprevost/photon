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

#ifndef PSON_ENGINE_H
#define PSON_ENGINE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include <photon/psoErrors.h>
#include <photon/psoCommon.h>
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  ifdef BUILD_PHOTON_ENGINE
#    define PHOTON_ENGINE_EXPORT __declspec ( dllexport )
#  else
#    define PHOTON_ENGINE_EXPORT __declspec ( dllimport )
#  endif
#else
#  define PHOTON_ENGINE_EXPORT
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
  * the offset for PSON_NULL_OFFSET !!! 
  */
#define PSON_NULL_OFFSET ( (ptrdiff_t) -1 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 *  The base address of the shared memory as seen for each process (each 
 *  process having their own copy of this global). This pointer is used
 *  everywhere to recover the real pointer addresses from our offsets
 */
PHOTON_ENGINE_EXPORT
extern unsigned char* g_pBaseAddr;

PHOTON_ENGINE_EXPORT
extern psocErrMsgHandle g_psoErrorHandle;

#define SET_OFFSET(ptr) ( (ptrdiff_t) ( (unsigned char*)(ptr) - \
       g_pBaseAddr ) )

/* Only use this macro when you know, for a fact, that the offset cannot
 * be the PSON_NULL_OFFSET (for example, in the LinkedList "class", the links
 * are never set to PSON_NULL_OFFSET...). 
 */
#define GET_PTR_FAST(off,class) ( (class*) (           \
       (unsigned char*) g_pBaseAddr + (ptrdiff_t) (off) ))

#define GET_PTR_OPT(target,offset,type) { \
   target = (type*) ( (unsigned char*) g_pBaseAddr + (ptrdiff_t) offset ); \
}

#define GET_PTR_DBG(target,offset,type) { \
   ptrdiff_t off = offset; \
   PSO_INV_CONDITION( off != 0 ); \
   PSO_INV_CONDITION( off != PSON_NULL_OFFSET ); \
   target = (type*) ( (unsigned char*) g_pBaseAddr + (ptrdiff_t) off ); \
}

#if defined(USE_DBC)
#  define GET_PTR(target,offset,type) GET_PTR_DBG(target,offset,type)
#else
#  define GET_PTR(target,offset,type) GET_PTR_OPT(target,offset,type)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSON_LOCK_TIMEOUT 10000 /* in milliseconds */

#define PSON_MAX_LOCK_DEPTH 3

/** Pages allocations will be done as multiples of PSON_BLOCK_SIZE. */
#if ! defined(PSON_BLOCK_SIZE)
#  define PSON_BLOCK_SIZE   8192
#  define PSON_BLOCK_SHIFT    13
#endif

/** Memory allocation will be done as multiples of PSON_ALLOCATION_UNIT. */
#if SIZEOF_VOID_P == 4
#  define PSON_ALLOCATION_UNIT 16
#elif SIZEOF_VOID_P == 8
#  define PSON_ALLOCATION_UNIT 32
#else
#  error "Without a known SIZEOF_VOID_P (4 or 8) I cannot calculate PSON_ALLOCATION_UNIT"
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

enum psonMemObjIdentifier
{
   /**
    * Special identifiers - it is set by the allocator when a group of blocks
    * is allocated (under lock). An "allocated" group with PSON_IDENT_LIMBO
    * is a group in limbo while the same group with PSON_IDENT_ALLOCATED
    * is not.
    */
   PSON_IDENT_CLEAR           = 0xc1ea9001,
   PSON_IDENT_LIMBO           = 0xccaaffee,
   PSON_IDENT_ALLOCATED       = 0xeeffaacc,
   
   PSON_IDENT_FIRST           = 0x34220101,
   PSON_IDENT_ALLOCATOR       = 0x34220103,

   PSON_IDENT_FOLDER          = 0x34220105,
   PSON_IDENT_HASH_MAP        = 0x34220107,
   PSON_IDENT_PROCESS         = 0x34220109,
   PSON_IDENT_PROCESS_MGR     = 0x3422010b,
   PSON_IDENT_QUEUE           = 0x3422010d,
   PSON_IDENT_SESSION         = 0x3422010f,
   PSON_IDENT_TRANSACTION     = 0x34220111,
   PSON_IDENT_MAP             = 0x34220113,
   PSON_IDENT_CURSOR          = 0x34220115,
   
   PSON_IDENT_LAST            = 0x34220125

};

typedef enum psonMemObjIdentifier psonMemObjIdent;

#define PSON_IDENT_PAGE_GROUP   0x80000000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonItemTypeEnum
{
   PSON_FIRST_ITEM,
   PSON_HASH_ITEM,
   PSON_HASH_TX_ITEM,
   PSON_LAST_ITEM
};

typedef enum psonItemTypeEnum psonItemTypeEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This enum allows to count the number of objects and the number of 
 * extensions (additional groups of blocks added to an object). 
 */
enum psonAllocTypeEnum
{
   /** When allocating/freeing the initial group of blocks of an API object */ 
   PSON_ALLOC_API_OBJ,
   
   /** Any other group of blocks */
   PSON_ALLOC_ANY
   
};

typedef enum psonAllocTypeEnum psonAllocTypeEnum;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_ENGINE_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

