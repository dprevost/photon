/* -*- c++ -*- */
/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSW_LOGMSG_H
#define VDSW_LOGMSG_H

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"
#if ! defined ( WIN32 )
#include <syslog.h>
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#define VDS_MAX_MSG_LOG 1024

enum wdMsgSeverity
{
#if defined ( WIN32 )
   WD_INFO    = EVENTLOG_INFORMATION_TYPE,
   WD_WARNING = EVENTLOG_WARNING_TYPE,
   WD_ERROR   = EVENTLOG_ERROR_TYPE
#else
   WD_INFO    = LOG_INFO,
   WD_WARNING = LOG_WARNING,
   WD_ERROR   = LOG_ERR
#endif
};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
/**
 *  The vdswLogMsg class is a wrapper for the differences between different
 *  platform-specific event-logging mechanisms (well... so far this means
 *  EventLog on Windows and syslog on Unix/linux).
 *
 *  Note that this is not considered a critical part of the work of the 
 *  daemon. Which means that we do not want a failure (to log a message, 
 *  for example) to disturb a production environment.
 */

class vdswLogMsg
{
public:

   vdswLogMsg( const char* progName );
   
   ~vdswLogMsg();   
   
   void StartUsingLogger()
   {
      m_useLog = true;
   }
   
   void SendMessage( enum wdMsgSeverity,
                     const char* format, 
                     ... );

#if defined ( WIN32 )
   /**
    *  Install the necessary stuff (registry keys, message file, etc.).
    *  This is not mandatory but it makes it easier to examine events
    *  using EventViewer.
    */
   int Install( const char * progName, 
                const char * msgPathName,
                int          dwNum );

   int Uninstall( const char* progName );
#endif

 private:

   /// True if we are a daemon or an NT service, false otherwise
   bool m_useLog;

#if defined ( WIN32 )
   HANDLE m_handle;
#endif

};

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#endif /* VDSW_LOGMSG_H */

