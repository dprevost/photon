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

#ifndef PSOA_CONNECTOR_H
#define PSOA_CONNECTOR_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#if defined (WIN32)
#  include <Winsock2.h>
#endif

#include "API/WatchdogCommon.h"
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (WIN32) 
#  define PSO_SOCKET SOCKET
#  define PSO_INVALID_SOCKET INVALID_SOCKET
#else
#  define PSO_SOCKET  int
#  define PSO_INVALID_SOCKET -1
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This object enables the client application to connect to quasar
 *  and to request the information it need to access the shared memory.
 *
 *  This object encapsulates the IPC mechanism uses between the apps and
 *  quasar. I've looked at different possibilities, for example 
 *  named pipes but in the end, sockets are more universal than other 
 *  mechanisms so the choice was not difficult to make.
 *
 *  However, to maintain the possibility of using a different mechanism,
 *  the interface must be "universal" (using a char* as the address of 
 *  the server instead of something more specific like an unsigned short 
 *  (port number)).
 *
 */

typedef struct psoaConnector
{
   /** Our socket handle */
   PSO_SOCKET socketFD;

#if defined (WIN32)
   /** Win32 only - if true, WSACleanup must be called by the destructor. */
   bool cleanupNeeded;
#endif

} psoaConnector;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Establish a connection between the client and the server.
 * 
 * This method also retrieves the information necessary for 
 * establishing a link to the shared memory (file name, etc.).
 *
 * \param address The address of quasar (currently this is just
 *                the tcp/ip port formatted as a string).
 * \param pAnswer A pointer to a structure containing all relevant
 *               information needed to establish a link to the shared memory.
 * \return A Photon error code.
 */
int psoaConnect( psoaConnector    * pConnector,
                 const char       * address,
                 struct WDOutput  * pAnswer,
                 psocErrorHandler * errorHandler );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Break the connection with quasar. */
void psoaDisconnect( psoaConnector    * pConnector,
                     psocErrorHandler * errorHandler );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_CONNECTOR_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

