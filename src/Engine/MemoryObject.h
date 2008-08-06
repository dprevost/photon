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

#ifndef VDSE_MEMORY_OBJECT_H
#define VDSE_MEMORY_OBJECT_H

#include "Engine/Engine.h"
#include "Engine/LinkNode.h"
#include "Engine/LinkedList.h"
#include "Engine/SessionContext.h"
#include "Engine/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

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
struct vdseMemObject
{
   /** Type of memory object */
   vdseMemObjIdent objType;
   
   /** The lock... obviously */
   vdscProcessLock lock;

   /** Total number of blocks for the current object */
   size_t totalBlocks;
   
   vdseLinkedList listBlockGroup;
   
};

typedef struct vdseMemObject vdseMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
enum vdsErrors 
vdseMemObjectInit( vdseMemObject   * pMemObj,
                   vdseMemObjIdent   objType,
                   vdseBlockGroup  * pGroup,
                   size_t            numBlocks );

VDSF_ENGINE_EXPORT
enum vdsErrors 
vdseMemObjectFini( vdseMemObject      * pMemObj,
                   vdseAllocTypeEnum    allocType,
                   vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
unsigned char* vdseMalloc( vdseMemObject      * pMemObj,
                           size_t               numBytes,
                           vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseFree( vdseMemObject      * pMemObj,
               unsigned char      * ptr, 
               size_t               numBytes,
               vdseSessionContext * pContext );

static inline
bool vdseLock( vdseMemObject      * pMemObj,
               vdseSessionContext * pContext )
{
   bool ok;

   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );
   
   if ( pContext->lockOffsets != NULL ) {
      vdseSessionAddLock( pContext, SET_OFFSET( pMemObj ) );
   }
   
   ok = vdscTryAcquireProcessLock ( &pMemObj->lock,
                                    pContext->pidLocker,
                                    LOCK_TIMEOUT );
   VDS_POST_CONDITION( ok == true || ok == false );
   
   return ok;
}

static inline
void vdseLockNoFailure( vdseMemObject      * pMemObj,
                        vdseSessionContext * pContext )
{
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   if ( pContext->lockOffsets != NULL ) {
      vdseSessionAddLock( pContext, SET_OFFSET( pMemObj ) );
   }
   
   vdscAcquireProcessLock ( &pMemObj->lock, LOCK_TIMEOUT );
}

VDSF_ENGINE_EXPORT
void vdseMemObjectStatus( vdseMemObject * pMemObject, 
                          vdsObjStatus  * pStatus );

static inline
void vdseUnlock( vdseMemObject      * pMemObj,
                 vdseSessionContext * pContext  )
{
   VDS_PRE_CONDITION( pMemObj  != NULL );
   VDS_PRE_CONDITION( pContext != NULL );

   if ( pContext->lockOffsets != NULL ) {
      vdseSessionRemoveLock( pContext, SET_OFFSET( pMemObj ) );
   }
   
   vdscReleaseProcessLock ( &pMemObj->lock );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_MEMORY_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

