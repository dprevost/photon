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

#ifndef PSN_PROCESS_MANAGER_H
#define PSN_PROCESS_MANAGER_H

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
struct psnProcessManager
{
   /** Always first */
   struct psnMemObject memObject;

   /** Our linked lists of cleanup processes objects. */
   psnLinkedList listOfProcesses;

   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnProcessManager psnProcMgr;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool psnProcMgrInit( psnProcMgr        * pManager,
                      psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnProcMgrAddProcess( psnProcMgr        * pManager,
                            pid_t                pid, 
                            psnProcess       ** ppCleanupProcess,
                            psnSessionContext * pContext );

/*
 * This function is for the watchdog - to recover from crashes in
 * real time.
 */
VDSF_ENGINE_EXPORT
bool psnProcMgrFindProcess( psnProcMgr        * pManager,
                             pid_t                pid, 
                             psnProcess       ** ppCleanupProcess,
                             psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnProcMgrRemoveProcess( psnProcMgr        * pManager,
                               psnProcess        * pCleanupProcess,
                               psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_PROCESS_MANAGER_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

