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

#ifndef VDSE_PROCESS_MANAGER_H
#define VDSE_PROCESS_MANAGER_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/LinkedList.h"
#include "Engine/Process.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * The CleanupManager and associated objects are used for handling
 * crashes (process crashes, mainly). They keep track of the current status
 * and operations done by each process and by doing so enables the watchdog
 * to cleanup if a process die unexpectedly: undo the current transaction(s),
 * closed objects (->decrease access counter), unlocked objects, etc.
 *
 */
typedef struct vdseProcessManager
{
   /** Always first */
   struct vdseMemObject memObject;

   /** Our linked lists of cleanup processes objects. */
   vdseLinkedList listOfProcesses;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

} vdseProcMgr;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdseProcMgrInit( vdseProcMgr        * pManager,
                     vdseSessionContext * pContext );

/*
void vdseProcMgrFini( vdseProcessManager * pManager,
                      vdseSessionContext * pContext );
*/

int vdseProcMgrAddProcess( vdseProcMgr        * pManager,
                           pid_t                pid, 
                           vdseProcess       ** ppCleanupProcess,
                           vdseSessionContext * pContext );

int vdseProcMgrFindProcess( vdseProcMgr        * pManager,
                            pid_t                pid, 
                            vdseProcess       ** ppCleanupProcess,
                            vdseSessionContext * pContext );

int vdseProcMgrRemoveProcess( vdseProcMgr        * pManager,
                              vdseProcess        * pCleanupProcess,
                              vdseSessionContext * pContext );

/* Used by the crash recovery process */
void vdseProcMgrReset( vdseProcMgr        * pManager,
                       vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_PROCESS_MANAGER_H */
