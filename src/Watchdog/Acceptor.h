/* -*- C++ -*- */
/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#ifndef VDS_ACCEPTOR_H
#define VDS_ACCEPTOR_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#if defined (WIN32)
#  define FD_SETSIZE 100
#  include <Winsock2.h>
#endif

#include "WatchdogCommon.h"
#include "ErrorHandler.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if defined (WIN32) 
#  define VDS_SOCKET SOCKET
#  define VDS_INVALID_SOCKET NULL
#else
#  define VDS_SOCKET  int
#  define VDS_INVALID_SOCKET -1
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/* 
 * Not really a dispatch table but...
 */
struct VdsDispatch
{
   VDS_SOCKET socketId;
   pid_t      pid;
   size_t     dataToBeWritten;
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// Forward declaration
class VdsWatchdog;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/**
 *  This class enables the watchdog to answer connection request from 
 *  applications and to send the information they need to access the VDS.
 *
 *  This class encapsulates the IPC mechanism uses between the apps and
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

class VdsAcceptor
{
public:

   VdsAcceptor();

   ~VdsAcceptor();

   int PrepareConnection( VdsWatchdog* pWatchdog );

   void WaitForConnections();

   vdscErrorHandler errorHandler;
   
private:

   VDS_SOCKET m_socketFD;

   VdsWatchdog* m_pWatchdog;
   
   VdsDispatch m_dispatch[FD_SETSIZE];

   struct WDOutput m_answer;

#if defined (WIN32)
   /// If true, WSACleanup must be called by the destructor
   bool m_cleanupNeeded;
#endif

   int GetSockError()
   {
#if defined (WIN32)
      return WSAGetLastError();
#else
      return errno;
#endif
   }

   int Accept();
   
   void Send( int indice );
   
   void Receive( int indice );

   void HandleAbnormalTermination( pid_t pid );

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDS_ACCEPTOR_H */
