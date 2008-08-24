/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSA_PROCESS_H
#define PSA_PROCESS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "API/api.h"
#include "Nucleus/Engine.h"
#include "Nucleus/SessionContext.h"
#include <photon/psoCommon.h>
#include "API/Connector.h"
#include "Common/ThreadLock.h"
#include "Common/MemoryFile.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Forward declarations */
struct psnMemoryHeader;
struct psnProcess;
struct psnObjectContext;
struct psoProxyObject;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct psaProcess
{
   psaObjetType type;

   /** Pointer to the header of the VDS memory. */
   struct psnMemoryHeader* pHeader;

   /** 
    * Pointer to our own cleanup object in the VDS. This object is used by 
    * the CleanupManager to hold process-specific information that might
    * be needed in case of a crash (current state of transactions, etc.).
    */
   struct psnProcess* pCleanup;

   /** The pid of the process is stored in this member. */
   pso_lock_T lockValue;

   /** The name of the directory for log files (used by transactions). */
   char logDirName[PATH_MAX];

   /** This object encapsulates the task of talking with the watchdog. */
   psaConnector connector;
   
   pscMemoryFile memoryFile;
   
} psaProcess;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_API_EXPORT
extern psaProcess *  g_pProcessInstance;

VDSF_API_EXPORT
extern bool           g_protectionIsNeeded;

/** 
 * This global mutex is needed for opening and closing sessions in 
 * a multi-threaded environment.
 */
extern pscThreadLock g_ProcessMutex;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_API_EXPORT
int psaProcessInit( psaProcess * pProcess,
                    const char * wdAddress );

VDSF_API_EXPORT
void psaProcessFini();

VDSF_API_EXPORT
bool AreWeTerminated();

VDSF_API_EXPORT
int psaOpenVDS( psaProcess        * process,
                const char        * memoryFileName,
                size_t              memorySizekb,
                psnSessionContext * pSession );
                        
VDSF_API_EXPORT
void psaCloseVDS( psaProcess        * process,
                  psnSessionContext * pSession );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#ifndef __cplusplus
static 
#endif
__inline
bool psaProcessLock()
{
   bool ok = true;
   
   if ( g_protectionIsNeeded ) {
      ok = pscTryAcquireThreadLock( &g_ProcessMutex, LOCK_TIMEOUT );
      PSO_POST_CONDITION( ok == true || ok == false );
   }
   
   return ok;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Unlock the current process. */
#ifndef __cplusplus
static 
#endif
__inline
void psaProcessUnlock()
{
   if ( g_protectionIsNeeded ) {
      pscReleaseThreadLock( &g_ProcessMutex );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSA_PROCESS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

