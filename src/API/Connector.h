/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework) Library.
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

#ifndef VDSA_CONNECTOR_H
#define VDSA_CONNECTOR_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#if defined (WIN32)
#  define FD_SETSIZE 100
#  include <Winsock2.h>
#endif

#include "API/WatchdogCommon.h"
#include "Common/ErrorHandler.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined (WIN32) 
#  define VDS_SOCKET SOCKET
#  define VDS_INVALID_SOCKET NULL
#else
#  define VDS_SOCKET  int
#  define VDS_INVALID_SOCKET -1
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 *  This object enables the client application to connect to the watchdog
 *  and to request the information it need to access the VDS.
 *
 *  This object encapsulates the IPC mechanism uses between the apps and
 *  the watchdog. I've looked at different possibilities, for example 
 *  named pipes but in the end, sockets are more universal than other 
 *  mechanisms so the choice was not difficult to make.
 *
 *  However, to maintain the possibility of using a different mechanism,
 *  the interface must be "universal" (using a char* as the address of 
 *  the watchdog instead of something more specific like an unsigned short 
 *  (port number)).
 *
 */

typedef struct vdsaConnector
{
   /** Our socket handle */
   VDS_SOCKET socketFD;

#if defined (WIN32)
   /** Win32 only - if true, WSACleanup must be called by the destructor. */
   bool cleanupNeeded;
#endif

} vdsaConnector;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Establish a connection between the client and the watchdog.
 * 
 * This method also retrieves the information necessary for 
 * establishing a link to the VDS (file name, etc.).
 *
 * \param address The address of the watchdog (currently this is just
 *                the tcp/ip port formatted as a string).
 * \param pAnswer A pointer to a structure containing all relevant
 *               information needed to establish a link to the VDS.
 * \return A VDS error code.
 */
int vdsaConnect( vdsaConnector    * pConnector,
                 const char       * address,
                 struct WDOutput  * pAnswer,
                 vdscErrorHandler * errorHandler );

/** Break the connection with the watchdog. */
void vdsaDisconnect( vdsaConnector    * pConnector,
                     vdscErrorHandler * errorHandler );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_CONNECTOR_H */
