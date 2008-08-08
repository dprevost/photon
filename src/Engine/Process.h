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

#ifndef VDSE_PROCESS_H
#define VDSE_PROCESS_H

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
struct vdseProcess
{
   /** Always first */
   struct vdseMemObject memObject;

   vdseLinkNode node;
   
   pid_t pid;

   vdseLinkedList listOfSessions;

   bool processIsTerminating;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

};

typedef struct vdseProcess vdseProcess;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool vdseProcessInit( vdseProcess        * pProcess,
                      pid_t                pid,
                      vdseSessionContext * pContext );
   
VDSF_ENGINE_EXPORT
void vdseProcessFini( vdseProcess        * pProcess,
                      vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseProcessAddSession( vdseProcess        * pProcess,
                            void               * pApiSession,
                            vdseSession       ** pSession,
                            vdseSessionContext * pContext );

/*
 * Takes a lock on the current object. Not on the vdseSession itself. 
 */
VDSF_ENGINE_EXPORT
bool vdseProcessRemoveSession( vdseProcess        * pProcess,
                               vdseSession        * pSession,
                               vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseProcessGetFirstSession( vdseProcess        * pProcess,
                                 vdseSession       ** ppSession,
                                 vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool vdseProcessGetNextSession( vdseProcess        * pProcess,
                                vdseSession        * pCurrent,
                                vdseSession       ** ppNext,
                                vdseSessionContext * pContext );

static inline
void vdseProcessNoMoreSessionAllowed( vdseProcess        * pProcess,
                                      vdseSessionContext * pContext )
{
   if ( vdseLock( &pProcess->memObject, pContext ) ) {
      pProcess->processIsTerminating = true;
      vdseUnlock( &pProcess->memObject, pContext );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_PROCESS_CLEANUP_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

