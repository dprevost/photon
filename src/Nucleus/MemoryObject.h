/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef PSN_MEMORY_OBJECT_H
#define PSN_MEMORY_OBJECT_H

#include "Nucleus/Engine.h"
#include "Nucleus/LinkNode.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/BlockGroup.h"

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
 * The psnBlockGroup struct is NOT included in this struct since it
 * contains a variable array size. The psnBlockGroup struct should be
 * put at the end of the container that owns a psnMemObject.
 */
struct psnMemObject
{
   /** Type of memory object */
   psnMemObjIdent objType;
   
   /** The lock... obviously */
   pscProcessLock lock;

   /** Total number of blocks for the current object */
   size_t totalBlocks;
   
   psnLinkedList listBlockGroup;
   
};

typedef struct psnMemObject psnMemObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
enum psoErrors 
psnMemObjectInit( psnMemObject   * pMemObj,
                   psnMemObjIdent   objType,
                   psnBlockGroup  * pGroup,
                   size_t            numBlocks );

VDSF_ENGINE_EXPORT
enum psoErrors 
psnMemObjectFini( psnMemObject      * pMemObj,
                   psnAllocTypeEnum    allocType,
                   psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
unsigned char* psnMalloc( psnMemObject      * pMemObj,
                           size_t               numBytes,
                           psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnFree( psnMemObject      * pMemObj,
               unsigned char      * ptr, 
               size_t               numBytes,
               psnSessionContext * pContext );

static inline
bool psnLock( psnMemObject      * pMemObj,
               psnSessionContext * pContext )
{
   bool ok;

   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );
   
   if ( pContext->lockOffsets != NULL ) {
      psnSessionAddLock( pContext, SET_OFFSET( pMemObj ) );
   }
   
   ok = pscTryAcquireProcessLock ( &pMemObj->lock,
                                    pContext->pidLocker,
                                    LOCK_TIMEOUT );
   PSO_POST_CONDITION( ok == true || ok == false );
   
   return ok;
}

static inline
void psnLockNoFailure( psnMemObject      * pMemObj,
                        psnSessionContext * pContext )
{
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   if ( pContext->lockOffsets != NULL ) {
      psnSessionAddLock( pContext, SET_OFFSET( pMemObj ) );
   }
   
   pscAcquireProcessLock ( &pMemObj->lock, LOCK_TIMEOUT );
}

VDSF_ENGINE_EXPORT
void psnMemObjectStatus( psnMemObject * pMemObject, 
                          psoObjStatus  * pStatus );

static inline
void psnUnlock( psnMemObject      * pMemObj,
                 psnSessionContext * pContext  )
{
   PSO_PRE_CONDITION( pMemObj  != NULL );
   PSO_PRE_CONDITION( pContext != NULL );

   if ( pContext->lockOffsets != NULL ) {
      psnSessionRemoveLock( pContext, SET_OFFSET( pMemObj ) );
   }
   
   pscReleaseProcessLock ( &pMemObj->lock );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_MEMORY_OBJECT_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

