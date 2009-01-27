/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSO_QUASAR_H
#define PSO_QUASAR_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/ProcessLock.h"
#include "Quasar/Acceptor.h"
#include "Quasar/ShMemHandler.h"
#include "Quasar/LogMsg.h"
#include "Quasar/Config.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PROG_NAME "quasar"

#define QSR_SHUTDOWN_REQUEST 0X001

#define QSR_MSG_LEN 512

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration(s)
struct psonMemoryHeader;

extern psocErrMsgHandle g_qsrErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * These define are still use by the win32 code (registry) but I'm not
 * sure if this will be use when I decide to implement NT services, one
 * of these days.
 *
 * They also do not always match the values of the config.
 */
#define PSO_LOCATION  "mem_location"
#define PSO_QSRADDRESS "quasar_address"
#define PSO_MEMSIZE   "mem_size"
#define PSO_USE_LOG   "LogTransaction"
#define PSO_FILEPERMS "file_access"
#define PSO_DIRPERMS  "DirectoryPermissions"

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

struct qsrQuasar
{
   
   /// Configuration parameters (as read from Windows registry or from 
   /// the config file.
   struct ConfigParams params;

   struct psonMemoryHeader * pMemoryAddress;
   
   /// Lock to control synchronization (needed on Win32 only?)
   /// todo - check this + initialize lock
   // PSO_ProcessLock lock;
   
   /// Control word (or status flag) that determines the current status
   /// of the server (running, suspended, shutting down, etc. ).
   unsigned int controlWord;
   
   /// Listen to connection requests
   struct qsrAcceptor acceptor;

   /// Send messages to system log facility once stderr is not available
   struct qsrLogMsg  log;
   
   struct qsrHandler handler;

   psocErrorHandler errorHandler;

   bool verifyMemOnly;
   
   char errorMsg[QSR_MSG_LEN];
};

typedef struct qsrQuasar qsrQuasar;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

extern qsrQuasar * g_pQSR;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void qsrHandleAbnormalTermination( qsrQuasar * pQuasar,
                                   pid_t       pid )
{
   qsrHandleCrash( &g_pQSR->handler, pid );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrQuasarInit( qsrQuasar * pQuasar );

void qsrQuasarFini( qsrQuasar * pQuasar );

#if defined ( WIN32 )

bool qsrInstall( qsrQuasar * pQuasar );

void qsrUninstall( qsrQuasar * pQuasar );

#else
bool qsrDaemon( qsrQuasar * pQuasar );
#endif

bool qsrQuasarReadConfig( qsrQuasar * pQuasar, const char* cfgname );   
   
void qsrHelp( const char* progName );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline 
int qsrLastError() {
#if defined ( WIN32 )
   return GetLastError();
#else
   return errno;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This function is going to validate the shared memory or create it if it does 
 *  not exist, cleanup any leftovers from previous runs, possibly run
 *  the CrashRecovery (eventually), possibly create a backup copy, etc.
 *  It will also initialize all POSIX semaphores (if we used them on
 *  that platform). This is done before becoming a daemon, on Unix/linux,
 *  or if run as a command line process on Windows so that errors are 
 *  reported to the terminal (easier to debug/see problems immediately
 *  than having to search in the EventLog/syslog facility).
 */
static inline
bool qsrInitializeMem( qsrQuasar * pQuasar ) {
   return qsrHandlerInit( &pQuasar->handler,
                          &pQuasar->params, 
                          &pQuasar->pMemoryAddress, 
                          pQuasar->verifyMemOnly );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrRun();
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* PSO_QUASAR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

