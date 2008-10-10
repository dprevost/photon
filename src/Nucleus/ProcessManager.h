/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef PSON_PROCESS_MANAGER_H
#define PSON_PROCESS_MANAGER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/Process.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The CleanupManager and associated objects are used for handling
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the watchdog
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
struct psonProcessManager
{
   /** Always first */
   struct psonMemObject memObject;

   /** Our linked lists of cleanup processes objects. */
   psonLinkedList listOfProcesses;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonProcessManager psonProcMgr;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonProcMgrInit( psonProcMgr        * pManager,
                      psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonProcMgrAddProcess( psonProcMgr        * pManager,
                            pid_t                pid, 
                            psonProcess       ** ppCleanupProcess,
                            psonSessionContext * pContext );

/*
 * This function is for the watchdog - to recover from crashes in
 * real time.
 */
PHOTON_ENGINE_EXPORT
bool psonProcMgrFindProcess( psonProcMgr        * pManager,
                             pid_t                pid, 
                             psonProcess       ** ppCleanupProcess,
                             psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonProcMgrRemoveProcess( psonProcMgr        * pManager,
                               psonProcess        * pCleanupProcess,
                               psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_PROCESS_MANAGER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

