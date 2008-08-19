/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSN_SESSION_H
#define PSN_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/LinkNode.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"

struct psnTx;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The context information for each object accessed by a session is 
 * kept in a doubly linked list (for fast access). 
 */
struct psnObjectContext
{
   /** offset to the object pointer in memory */
   ptrdiff_t offset;

   /** object type */
   enum vdsObjectType type;

   /**
    * Pointer to the object as allocated in the heap of the process.
    * We cannot use the real type since the Nucleus does not know about
    * the API objects.
    */
   void * pCommonObject; 
   
   /** Our node in the linked list of the psnSession object. */
   psnLinkNode node;
      
};

typedef struct psnObjectContext psnObjectContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This object is part of a group of objects which are used for handling 
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the watchdog
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
typedef struct psnSession
{
   /** Always first */
   struct psnMemObject memObject;

   /** This object is part of the linked list of the psnProcess */
   psnLinkNode node;

   /** Pointer to the vdsaSession. To be used by the process object when
       it cleans up its sessions. */
   void * pApiSession;
   
   /** Pointer to our transaction object */
   struct psnTx * pTransaction;
   
   /** Our own internal list of objects accessed by the current session */
   psnLinkedList listOfObjects;

   /** our current list of locks */
   ptrdiff_t lockOffsets[PSN_MAX_LOCK_DEPTH];

   /** number of locks we are holding */
   int numLocks;
   
   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

} psnSession;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool psnSessionInit( psnSession        * pSession,
                      void               * pApiSession,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnSessionFini( psnSession        * pSession,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnSessionAddObj( psnSession        * pSession,
                        ptrdiff_t            objOffset, 
                        enum vdsObjectType   objType, 
                        void               * pCommonObject,
                        psnObjectContext ** ppObject,
                        psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnSessionRemoveObj( psnSession        * pSession,
                          psnObjectContext  * pObject,
                          psnSessionContext * pContext );

/* Lock and Unlock must be used before calling this function */
VDSF_ENGINE_EXPORT
bool psnSessionRemoveFirst( psnSession        * pSession,
                             psnSessionContext * pContext );

/* Lock and Unlock must be used before calling this function */
VDSF_ENGINE_EXPORT
bool psnSessionGetFirst( psnSession        * pSession,
                          psnObjectContext ** ppObject,
                          psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

