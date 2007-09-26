/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_SESSION_H
#define VDSE_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/LinkedList.h"
#include "Engine/LinkNode.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"

struct vdseTx;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The context information for each object accessed by a session is 
 * kept in a doubly linked list (for fast access). 
 */
typedef struct vdseObjectContext
{
   /** offset to the object pointer in memory */
   ptrdiff_t          offset;

   /** object type */
   enum vdsObjectType type;

   /**
    * Pointer to the object as allocated in the heap of the process.
    * We cannot use the real type since the Engine does not know about
    * the API objects.
    */
   void * pProxyObject; 
   
   /** Our node in the linked list of the vdseSession object. */
   vdseLinkNode node;
      
} vdseObjectContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This object is part of a group of objects which are used for handling 
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the watchdog
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
typedef struct vdseSession
{
   /** Always first */
   struct vdseMemObject memObject;

   /** This object is part of the linked list of the vdseProcess */
   vdseLinkNode node;

   /** Pointer to the vdsaSession. To be used by the process object when
       it cleans up its sessions. */
   void * pApiSession;
   
   /** Pointer to our transaction object */
   struct vdseTx * pTransaction;
   
   /** Our own internal list of objects accessed by the current session */
   vdseLinkedList listOfObjects;

   /** our current list of locks */
   ptrdiff_t lockOffsets[VDSE_MAX_LOCK_DEPTH];

   /** number of locks we are holding */
   int numLocks;
   
   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

} vdseSession;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseSessionInit( vdseSession        * pSession,
                     void               * pApiSession,
                     vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseSessionFini( vdseSession        * pSession,
                      vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseSessionAddObj( vdseSession        * pSession,
                       ptrdiff_t            objOffset, 
                       enum vdsObjectType   objType, 
                       void *               pProxyObject,
                       vdseObjectContext ** ppObject,
                       vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseSessionRemoveObj( vdseSession        * pSession,
                          vdseObjectContext  * pObject,
                          vdseSessionContext * pContext );

/* Lock and Unlock must be used before calling this function */
VDSF_ENGINE_EXPORT
int vdseSessionRemoveFirst( vdseSession        * pSession,
                            vdseSessionContext * pContext );

/* Lock and Unlock must be used before calling this function */
VDSF_ENGINE_EXPORT
int vdseSessionGetFirst( vdseSession        * pSession,
                         vdseObjectContext ** ppObject,
                         vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_SESSION_H */

