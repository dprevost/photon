/* -*- c++ -*- */
/*
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

#ifndef VDSE_ENGINE_H
#define VDSE_ENGINE_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common.h"
#include "ProcessLock.h"
#include <vdsf/vdsErrors.h>
#include <vdsf/vdsCommon.h>

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * List of possible "type" identifiers for all objects allocated
 * in shared memory. Each of these objects inherits from BaseObject
 * which sets the object identifier (as part of the constructor).
 * This is used, in part, by the crash recovery process
 * to validate/invalidate recovered buffers.
 */

enum ObjectIdentifier
{
   IDENT_FIRST           = 0x34220101,

   IDENT_FOLDER          = 0x34220102,
   IDENT_QUEUE           = 0x34220103,
   IDENT_DUMMY_NODE      = 0x34220104,    /** linked list dummy node */
   IDENT_OBJECT_CONTEXT  = 0x34220105,
   IDENT_TRANSACTION_OPS = 0x34220106,
   IDENT_QUEUE_ELEMENT   = 0x34220107,
   IDENT_TRANSACTION     = 0x34220108,
   IDENT_CLEAN_PROCESS   = 0x34220109,
   IDENT_CLEAN_SESSION   = 0x3422010a,
   IDENT_HASH_LIST       = 0x3422010b,
   IDENT_TRANSACTION_MGR = 0x3422010c,
   IDENT_CLEANUP_MGR     = 0x3422010d,
   IDENT_HASH_ARRAY      = 0x3422010e,
   IDENT_OBJ_DESC        = 0x34220110,
   IDENT_ROW_DESC        = 0x34220111,

   IDENT_LAST            = 0x34220112,

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
