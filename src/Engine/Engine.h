/* -*- c++ -*- */
/*
 * Copyright (C) 2006, 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common.h"
#include "ProcessLock.h"
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsCommon.h>
#include "ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined(WIN32)
#  ifdef ENGINE_EXPORTS
#    define VDSF_ENGINE_EXPORT __declspec ( dllexport )
#  else
#    define VDSF_ENGINE_EXPORT __declspec ( dllimport )
#  endif
#else
#  define VDSF_ENGINE_EXPORT
#endif

#ifdef __cplusplus
#  if HAVE_NEW
#    include <new>
#  else
#    if HAVE_NEW_H
#      include <new.h>
#    endif
#  endif
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 *  The base address of the shared memory as seen for each process (each 
 *  process having their own copy of this global). This pointer is used
 *  everywhere to recover the real pointer addresses from our offsets
 */
VDSF_ENGINE_EXPORT
extern unsigned char* g_pBaseAddr;

extern vdscErrMsgHandle g_vdsErrorHandle;

#define SET_OFFSET(ptr) ( (ptrdiff_t) ( (unsigned char*)(ptr) - \
       g_pBaseAddr ) )

/* Only use this macro when you know, for a fact, that the offset cannot
 * be the NULL_OFFSET (for example, in the LinkedList class, the links
 * are never set to NULL_OFFSET...). 
 */
#define GET_PTR(off,class) ( (class*) (           \
       (unsigned char*) g_pBaseAddr + (ptrdiff_t) (off) ))

#define SET_PTR(target,offset,type)  \
   if ( offset == NULL_OFFSET ) \
      target = NULL; \
   else \
      target = (type*) (           \
         (unsigned char*) g_pBaseAddr + (ptrdiff_t) offset );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * We cannot use "[*]_t" as this is reserved. But _T is ok, as far as I
 * know...
 */
typedef unsigned int transaction_T;

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
  * the offset for NULL_OFFSET !!! 
  */
#define NULL_OFFSET ( (ptrdiff_t) -1 )

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define MAX_KEY_LENGTH    1024

#define LOCK_TIMEOUT 10000 /* in milliseconds */

/** Memory allocation will be done as multiples of VDSE_ALLOCATION_UNIT. */
#define VDSE_ALLOCATION_UNIT 16

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * List of possible "type" identifiers for all objects allocated
 * in shared memory. Each of these objects "inherits" from MemoryObject
 * which sets the object identifier (as part of the initializer).
 * This is used, in part, by the crash recovery process
 * to validate/invalidate recovered buffers.
 */

enum ObjectIdentifier
{
   VDSE_IDENT_CLEAR           = 0xccaaffee,
   VDSE_IDENT_FIRST           = 0x34220100,

   VDSE_IDENT_ALLOCATOR       = 0x34220101,
   VDSE_IDENT_FOLDER          = 0x34220102,
   VDSE_IDENT_QUEUE           = 0x34220103,
   VDSE_IDENT_DUMMY_NODE      = 0x34220104,    /** linked list dummy node */
   VDSE_IDENT_OBJECT_CONTEXT  = 0x34220105,
   VDSE_IDENT_TRANSACTION_OPS = 0x34220106,
   VDSE_IDENT_QUEUE_ELEMENT   = 0x34220107,
   VDSE_IDENT_TRANSACTION     = 0x34220108,
   VDSE_IDENT_CLEAN_PROCESS   = 0x34220109,
   VDSE_IDENT_CLEAN_SESSION   = 0x3422010a,
   VDSE_IDENT_HASH_LIST       = 0x3422010b,
   VDSE_IDENT_TRANSACTION_MGR = 0x3422010c,
   VDSE_IDENT_CLEANUP_MGR     = 0x3422010d,
   VDSE_IDENT_HASH_ARRAY      = 0x3422010e,
   VDSE_IDENT_OBJ_DESC        = 0x34220110,
   VDSE_IDENT_ROW_DESC        = 0x34220111,

   VDSE_IDENT_LAST            = 0x34220112,

   /** 
    * Identify a destroyed object. Special identifier used in the 
    * destructor of the class BaseObject to identify that the 
    * destructor was called for "this"
    * 
    * It might never amount to anything... but it might become
    * useful for debugging (and it costs nothing to implement).
    */
   IDENT_DESTROY         = 0x4300227c   

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_ENGINE_H */
