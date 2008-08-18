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

#ifndef VDSA_PROCESS_H
#define VDSA_PROCESS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Engine/Engine.h"
#include "Engine/SessionContext.h"
#include <vdsf/vdsCommon.h>
#include "API/Connector.h"
#include "Common/ThreadLock.h"
#include "Common/MemoryFile.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declarations */
struct vdseMemoryHeader;
struct vdseProcess;
struct vdseObjectContext;
struct vdsProxyObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdsaProcess
{
   vdsaObjetType type;

   /** Pointer to the header of the VDS memory. */
   struct vdseMemoryHeader* pHeader;

   /** 
    * Pointer to our own cleanup object in the VDS. This object is used by 
    * the CleanupManager to hold process-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    */
   struct vdseProcess* pCleanup;

   /** The pid of the process is stored in this member. */
   vds_lock_T lockValue;

   /** The name of the directory for log files (used by transactions). */
   char logDirName[PATH_MAX];

   /** This object encapsulates the task of talking with the watchdog. */
   vdsaConnector connector;
   
   pscMemoryFile memoryFile;
   
} vdsaProcess;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_API_EXPORT
extern vdsaProcess *  g_pProcessInstance;

VDSF_API_EXPORT
extern bool           g_protectionIsNeeded;

/** 
 * This global mutex is needed for opening and closing sessions in 
 * a multi-threaded environment.
 */
extern pscThreadLock g_ProcessMutex;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_API_EXPORT
int vdsaProcessInit( vdsaProcess *pProcess,
                     const char  *wdAddress );

VDSF_API_EXPORT
void vdsaProcessFini();

VDSF_API_EXPORT
bool AreWeTerminated();

VDSF_API_EXPORT
int vdsaOpenVDS( vdsaProcess        * process,
                 const char         * memoryFileName,
                 size_t               memorySizekb,
                 vdseSessionContext * pSession );
                        
VDSF_API_EXPORT
void vdsaCloseVDS( vdsaProcess        * process,
                   vdseSessionContext * pSession );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef __cplusplus
static 
#endif
__inline
bool vdsaProcessLock()
{
   bool ok = true;
   
   if ( g_protectionIsNeeded ) {
      ok = pscTryAcquireThreadLock( &g_ProcessMutex, LOCK_TIMEOUT );
      VDS_POST_CONDITION( ok == true || ok == false );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Unlock the current process. */
#ifndef __cplusplus
static 
#endif
__inline
void vdsaProcessUnlock()
{
   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &g_ProcessMutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

