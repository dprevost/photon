/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef QSR_LOGMSG_H
#define QSR_LOGMSG_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#if ! defined ( WIN32 )
#include <syslog.h>
#endif

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define PSO_MAX_MSG_LOG 1024

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/**
 *  The qsrLogMsg class is a wrapper for the differences between different
 *  platform-specific event-logging mechanisms (well... so far this means
 *  EventLog on Windows and syslog on Unix/linux).
 *
 *  Note that this is not considered a critical part of the work of the 
 *  daemon. Which means that we do not want a failure (to log a message, 
 *  for example) to disturb a production environment.
 */

struct qsrLogMsg
{

   /// True if we are a daemon or an NT service, false otherwise
   bool useLog;

#if defined ( WIN32 )
   HANDLE handle;
#endif

   char * name;
};

typedef struct qsrLogMsg qsrLogMsg;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int qsrLogMsgInit( qsrLogMsg * pLog,
                    const char * progName );
   
void qsrLogMsgFini( qsrLogMsg * pLog );
   
void qsrStartUsingLogger( qsrLogMsg * pLog );
   
void qsrSendMessage( qsrLogMsg         * pLog,
                      enum wdMsgSeverity   severity,
                      const char         * format, 
                      ... );

#if defined ( WIN32 )
   /**
    *  Install the necessary stuff (registry keys, message file, etc.).
    *  This is not mandatory but it makes it easier to examine events
    *  using EventViewer.
    */
int qsrLogMsgInstall( qsrLogMsg * pLog,
                       const char * progName, 
                       const char * msgPathName,
                       int          dwNum );

int qsrLogMsgUninstall( qsrLogMsg * pLog,
                         const char * progName );
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

#endif /* QSR_LOGMSG_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

