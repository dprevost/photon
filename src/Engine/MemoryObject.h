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

#ifndef VDSE_MEMORY_OBJECT_H
#define VDSE_MEMORY_OBJECT_H

#include "Engine.h"
#include "LinkNode.h"
#include "LinkedList.h"
#include "SessionContext.h"
#include "Engine/BlockGroup.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDSE_MEM_ALIGNMENT 4

/**
 * Memory objects include all the data containers (queues, etc.), the folders
 * and all "hidden" top level objects (the allocator, the sessions recovery
 * objects, etc.).
 *
 * This struct should always be the first member of the struct defining such
 * an object. This way, the identifier is always at the top of a block and it
 * should help debug, recover from crashes, etc. 
 *
 * The vdseBlockGroup struct is NOT included in this struct since it
 * contains a variable array size. The vdseBlockGroup struct should be
 * put at the end of the container that owns a vdseMemObject.
 */
typedef struct vdseMemObject
{
   /** Type of memory object */
   enum ObjectIdentifier objType;
   
   /** The lock... obviously */
   vdscProcessLock lock;

   /** Total number of blocks for the current object */
   size_t totalBlocks;
   
   vdseLinkedList listBlockGroup;
   
} vdseMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
enum vdsErrors 
vdseMemObjectInit( vdseMemObject*        pMemObj,
                   enum ObjectIdentifier objType,
                   vdseBlockGroup*       pGroup,
                   size_t                numBlocks );

VDSF_ENGINE_EXPORT
enum vdsErrors 
vdseMemObjectFini( vdseMemObject*      pMemObj,
                   vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
unsigned char* vdseMalloc( vdseMemObject*      pMemObj,
                           size_t              numBytes,
                           vdseSessionContext* pContext );

VDSF_ENGINE_EXPORT
void vdseFree( vdseMemObject*      pMemObj,
               unsigned char*      ptr, 
               size_t              numBytes,
               vdseSessionContext* pContext );

static inline
int vdseLock( vdseMemObject*      pMemObj,
              ptrdiff_t           objOffset,
              vdseSessionContext* pContext )
{
   pContext->lockObject = objOffset;
   
   return vdscTryAcquireProcessLock ( &pMemObj->lock,
                                      pContext->lockValue,
                                      LOCK_TIMEOUT );
}

static inline
void vdseLockNoFailure( vdseMemObject*      pMemObj,
                        ptrdiff_t           objOffset,
                        vdseSessionContext* pContext )
{
   pContext->lockObject = objOffset;
   
   vdscAcquireProcessLock ( &pMemObj->lock, LOCK_TIMEOUT );
}

static inline
void vdseUnlock( vdseMemObject*      pMemObj,
                 vdseSessionContext* pContext  )
{
   vdscReleaseProcessLock ( &pMemObj->lock );
   pContext->lockObject = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSE_MEMORY_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

