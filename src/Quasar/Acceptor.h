/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSW_ACCEPTOR_H
#define VDSW_ACCEPTOR_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#if defined (WIN32)
#  ifndef FD_SETSIZE
#    define FD_SETSIZE 100
#  endif
#  include <Winsock2.h>
#endif

#include "API/WatchdogCommon.h"
#include "Common/ErrorHandler.h"

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (WIN32) 
#  define PSO_SOCKET SOCKET
#  define PSO_INVALID_SOCKET INVALID_SOCKET
#else
#  define PSO_SOCKET  int
#  define PSO_INVALID_SOCKET -1
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * Not really a dispatch table but...
 */
struct vdswDispatch
{
   PSO_SOCKET socketId;
   pid_t      pid;
   size_t     dataToBeWritten;
};

typedef struct vdswDispatch vdswDispatch;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration
struct vdswWatchdog;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This struct enables the watchdog to answer connection request from 
 *  applications and to send the information they need to access the VDS.
 *
 *  This module encapsulates the IPC mechanism uses between the apps and
 *  the watchdog. I've looked at different possibilities, for example 
 *  named pipes but in the end, sockets are more universal than other 
 *  mechanisms so the choice was not difficult to make.
 *
 *  However, to maintain the possibility of using a different mechanism,
 *  the interface must be "universal" (example: using a char* as the address 
 *  of the watchdog instead of something more specific like an unsigned short 
 *  (port number)).
 *
 */

struct vdswAcceptor
{
   pscErrorHandler errorHandler;
   
   PSO_SOCKET socketFD;

   struct vdswWatchdog * pWatchdog;
   
   struct vdswDispatch dispatch[FD_SETSIZE];

   struct WDOutput answer;

#if defined (WIN32)
   /** If true, WSACleanup must be called by the destructor */
   bool cleanupNeeded;
#endif
};

typedef struct vdswAcceptor vdswAcceptor;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
int GetSockError() 
{
#if defined (WIN32)
   return WSAGetLastError();
#else
   return errno;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswAcceptorInit( vdswAcceptor * pAcceptor );

void vdswAcceptorFini( vdswAcceptor * pAcceptor );

bool vdswPrepareConnection( vdswAcceptor        * pAcceptor,
                            struct vdswWatchdog * pWatchdog );

void vdswWaitForConnections( vdswAcceptor * pAcceptor );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* VDSW_ACCEPTOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

