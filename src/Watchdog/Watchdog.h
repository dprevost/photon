/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef VDS_WATCHDOG_H
#define VDS_WATCHDOG_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include "Acceptor.h"
#include "VDSHandler.h"
#include "LogMsg.h"
#include "Config.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PROG_NAME "vdswd"

#define WD_SHUTDOWN_REQUEST 0X001

#define WD_MSG_LEN 512

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration(s)
struct psnMemoryHeader;

extern pscErrMsgHandle g_wdErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define VDS_LOCATION  "VDSLocation"
#define VDS_WDADDRESS "WatchdogAddress"
#define VDS_MEMSIZE   "MemorySize"
#define VDS_USE_LOG   "LogTransaction"
#define VDS_FILEPERMS "FilePermissions"
#define VDS_DIRPERMS  "DirectoryPermissions"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  We use this class as a workaround for the fundamental differences between 
 *  NT services and becoming a Unix daemon.
 *
 *  One of these differences is of course that NT services do not use
 *  main() - instead, callbacks functions have to be provided (at least one 
 *  to provide the service, one to provide control). 
 *  [this implies at least two threads on NT -> we need to synchronize 
 *  access to shared data].
 *
 *  Note: this synchronisation is not implemented yet...
 */

struct vdswWatchdog
{
   
   /// Configuration parameters (as read from Windows registry or from 
   /// the config file.
   struct ConfigParams params;

   struct psnMemoryHeader * pMemoryAddress;
   
   /// Lock to control synchronization (needed on Win32 only?)
   /// todo - check this + initialize lock
   // VDS_ProcessLock lock;
   
   /// Control word (or status flag) that determines the current status
   /// of the watchdog (running, suspended, shutting down, etc. ).
   unsigned int controlWord;
   
   /// Listen to connection requests
   struct vdswAcceptor acceptor;

   /// Send messages to system log facility once stderr is not available
   struct vdswLogMsg  log;
   
   struct vdswHandler vds;

   pscErrorHandler errorHandler;

   bool verifyVDSOnly;
   
   char errorMsg[WD_MSG_LEN];
};

typedef struct vdswWatchdog vdswWatchdog;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

extern vdswWatchdog * g_pWD;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdswHandleAbnormalTermination( vdswWatchdog * pWatchdog,
                                    pid_t          pid )
{
   vdswHandleCrash( &g_pWD->vds, pid );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswWatchdogInit( vdswWatchdog * pWatchdog );

void vdswWatchdogFini( vdswWatchdog * pWatchdog );

#if defined ( WIN32 )

bool vdswInstall( vdswWatchdog * pWatchdog );

void vdswUninstall( vdswWatchdog * pWatchdog );

#else
bool vdswDaemon( vdswWatchdog * pWatchdog );
#endif

bool vdswWatchdogReadConfig( vdswWatchdog * pWatchdog, const char* cfgname );   
   
void vdswHelp( const char* progName );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
int vdswLastError() {
#if defined ( WIN32 )
   return GetLastError();
#else
   return errno;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This function is going to validate the VDS or create it if it does 
 *  not exist, cleanup any leftovers from previous runs, possibly run
 *  the CrashRecovery (eventually), possibly create a backup copy, etc.
 *  It will also initialize all POSIX semaphores (if we used them on
 *  that platform). This is done before becoming a daemon, on Unix/linux,
 *  or if run as a command line process on Windows so that errors are 
 *  reported to the terminal (easier to debug/see problems immediately
 *  than having to search in the EventLog/syslog facility).
 */
static inline
bool vdswInitializeVDS( vdswWatchdog * pWatchdog ) {
   return vdswHandlerInit( &pWatchdog->vds,
                           &pWatchdog->params, 
                           &pWatchdog->pMemoryAddress, 
                           pWatchdog->verifyVDSOnly );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswRun();
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDS_WATCHDOG_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

