/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_SESSION_H
#define PSON_SESSION_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/LinkNode.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"

struct psonTx;
struct psonCursor;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The context information for each object accessed by a session is 
 * kept in a doubly linked list (for fast access). 
 */
struct psonObjectContext
{
   /** offset to the object pointer in memory */
   ptrdiff_t offset;

   /** object type */
   enum psoObjectType type;

   /**
    * Pointer to the object as allocated in the heap of the process.
    * We cannot use the real type since the Nucleus does not know about
    * the API objects.
    */
   void * pCommonObject; 
   
   /** Our node in the linked list of the psonSession object. */
   psonLinkNode node;
      
};

typedef struct psonObjectContext psonObjectContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The context information for each object accessed by a session is 
 * kept in a doubly linked list (for fast access). 
 */
struct psonCursorContext
{
   /** offset to the cursor object in memory */
   ptrdiff_t offset;

   struct psonCursor * cursor;
   
   /** Our node in the linked list of the psonSession object. */
   psonLinkNode node;
      
};

typedef struct psonCursorContext psonCursorContext;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This object is part of a group of objects which are used for handling 
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the server
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
typedef struct psonSession
{
   /** Always first */
   struct psonMemObject memObject;

   /** This object is part of the linked list of the psonProcess */
   psonLinkNode node;

   /** Pointer to the psoaSession. To be used by the process object when
       it cleans up its sessions. */
   void * pApiSession;
   
   /** Pointer to our transaction object */
   struct psonTx * pTransaction;
   
   /** Our own internal list of objects accessed by the current session */
   psonLinkedList listOfObjects;

   /** Our own internal list of cursors accessed by the current session */
   psonLinkedList listOfCursors;

   /** our current list of locks */
   ptrdiff_t lockOffsets[PSON_MAX_LOCK_DEPTH];

   /** number of locks we are holding */
   int numLocks;
   
   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

} psonSession;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonSessionAddObj( psonSession        * pSession,
                        ptrdiff_t            objOffset, 
                        enum psoObjectType   objType, 
                        void               * pCommonObject,
                        psonObjectContext ** ppObject,
                        psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSessionCloseCursor( psonSession        * pSession,
                             psonCursorContext  * pObject,
                             psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonSessionFini( psonSession        * pSession,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSessionInit( psonSession        * pSession,
                      void               * pApiSession,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSessionOpenCursor( psonSession        * pSession,
                            psonCursorContext ** ppCursor,
                            psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonSessionRemoveObj( psonSession        * pSession,
                          psonObjectContext  * pObject,
                          psonSessionContext * pContext );

/* Lock and Unlock must be used before calling this function */
PHOTON_ENGINE_EXPORT
bool psonSessionRemoveFirst( psonSession        * pSession,
                             psonSessionContext * pContext );

/* Lock and Unlock must be used before calling this function */
PHOTON_ENGINE_EXPORT
bool psonSessionGetFirst( psonSession        * pSession,
                          psonObjectContext ** ppObject,
                          psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_SESSION_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

