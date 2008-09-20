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

#ifndef PSOQ_ACCEPTOR_H
#define PSOQ_ACCEPTOR_H

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
struct psoqDispatch
{
   PSO_SOCKET socketId;
   pid_t      pid;
   size_t     dataToBeWritten;
};

typedef struct psoqDispatch psoqDispatch;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

// Forward declaration
struct psoqWatchdog;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This struct enables the watchdog to answer connection request from 
 *  applications and to send the information they need to access the shared 
 *  memory.
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

struct psoqAcceptor
{
   psocErrorHandler errorHandler;
   
   PSO_SOCKET socketFD;

   struct psoqWatchdog * pWatchdog;
   
   struct psoqDispatch dispatch[FD_SETSIZE];

   struct WDOutput answer;

#if defined (WIN32)
   /** If true, WSACleanup must be called by the destructor */
   bool cleanupNeeded;
#endif
};

typedef struct psoqAcceptor psoqAcceptor;

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

void psoqAcceptorInit( psoqAcceptor * pAcceptor );

void psoqAcceptorFini( psoqAcceptor * pAcceptor );

bool psoqPrepareConnection( psoqAcceptor        * pAcceptor,
                            struct psoqWatchdog * pWatchdog );

void psoqWaitForConnections( psoqAcceptor * pAcceptor );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* PSOQ_ACCEPTOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

