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

#ifndef PSN_PROCESS_H
#define PSN_PROCESS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/LinkedList.h"
#include "Engine/Session.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This object is part of a group of objects which are used for handling 
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the watchdog
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
struct psnProcess
{
   /** Always first */
   struct psnMemObject memObject;

   psnLinkNode node;
   
   pid_t pid;

   psnLinkedList listOfSessions;

   bool processIsTerminating;

   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnProcess psnProcess;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool psnProcessInit( psnProcess        * pProcess,
                      pid_t                pid,
                      psnSessionContext * pContext );
   
VDSF_ENGINE_EXPORT
void psnProcessFini( psnProcess        * pProcess,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnProcessAddSession( psnProcess        * pProcess,
                            void               * pApiSession,
                            psnSession       ** pSession,
                            psnSessionContext * pContext );

/*
 * Takes a lock on the current object. Not on the psnSession itself. 
 */
VDSF_ENGINE_EXPORT
bool psnProcessRemoveSession( psnProcess        * pProcess,
                               psnSession        * pSession,
                               psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnProcessGetFirstSession( psnProcess        * pProcess,
                                 psnSession       ** ppSession,
                                 psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnProcessGetNextSession( psnProcess        * pProcess,
                                psnSession        * pCurrent,
                                psnSession       ** ppNext,
                                psnSessionContext * pContext );

static inline
void psnProcessNoMoreSessionAllowed( psnProcess        * pProcess,
                                      psnSessionContext * pContext )
{
   if ( psnLock( &pProcess->memObject, pContext ) ) {
      pProcess->processIsTerminating = true;
      psnUnlock( &pProcess->memObject, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

