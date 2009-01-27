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

#if ! defined(WIN32)
#  include <syslog.h>
#endif
#include "LogMsg.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int qsrLogMsgInit( qsrLogMsg * pLog,
                    const char * progName )
{
   int len = strlen( progName );

   PSO_PRE_CONDITION( pLog     != NULL );
   PSO_PRE_CONDITION( progName != NULL );

   pLog->useLog = false;
#if defined ( WIN32 )
   pLog->handle = NULL;
#endif
   pLog->name = calloc( len+1, 1 );
   if ( pLog->name == NULL ) return -1;
   
   strcpy( pLog->name, progName );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrLogMsgFini( qsrLogMsg * pLog )
{
   PSO_PRE_CONDITION( pLog != NULL );

   if (pLog->useLog) {
#if defined ( WIN32 )
      if ( pLog->handle != NULL ) {
         DeregisterEventSource( pLog->handle );
         pLog->handle = NULL;
      }
#else
      closelog();
#endif
   }
   if ( pLog->name ) free(pLog->name);
   pLog->name = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined ( WIN32 )
int qsrLogMsgInstall( qsrLogMsg * pLog,
                       const char * progName, 
                       const char * msgPathName,
                       int          numCategories )
{
   HKEY hKey; 
   char buffer[MAX_PATH]; 
   int eventType;
   
   PSO_PRE_CONDITION( progName    != NULL );
   PSO_PRE_CONDITION( msgPathName != NULL );

   // Install a new subkey to the EventLog service (under "Application")
   sprintf( buffer, 
            "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\%s",
            progName );
   if ( RegCreateKey( HKEY_LOCAL_MACHINE, buffer, &hKey ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error creating key for EventLog (error = %d)\n",
               GetLastError() ); 
      return -1;
   } 

   if ( RegSetValueEx( hKey,
                       "EventMessageFile",
                       0,
                       REG_EXPAND_SZ,
                       (BYTE*) msgPathName,
                       strlen(msgPathName)+1 ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error setting key value for EventLog (error = %d)\n",
               GetLastError() ); 
      RegCloseKey( hKey ); 
      return -1;
   }
 
   // We will support the three basic event types although, for the moment,
   // I guess only the ERROR one is of interest. 
   eventType = EVENTLOG_ERROR_TYPE       | 
               EVENTLOG_WARNING_TYPE     | 
               EVENTLOG_INFORMATION_TYPE; 
 
   if ( RegSetValueEx( hKey,
                       "TypesSupported",
                       0,
                       REG_DWORD,
                       (BYTE*) &eventType,
                       sizeof(int) ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error setting key value for EventLog (error = %d)\n",
               GetLastError() ); 
      RegCloseKey( hKey ); 
      return -1;
   } 

   if ( RegSetValueEx( hKey,
                       "CategoryMessageFile",
                       0,
                       REG_EXPAND_SZ,
                       (BYTE*) msgPathName,
                       strlen(msgPathName)+1 ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error setting key value for EventLog (error = %d)\n",
               GetLastError() ); 
      RegCloseKey( hKey ); 
      return -1;
   }
 
   if ( RegSetValueEx( hKey,
                       "CategoryCount",
                       0,
                       REG_DWORD,
                       (BYTE*) &numCategories,
                       sizeof(int) ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error setting key value for EventLog (error = %d)\n",
               GetLastError() ); 
      RegCloseKey( hKey ); 
      return -1;
   }

   RegCloseKey( hKey ); 
   return 0;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrSendMessage( qsrLogMsg           * pLog,
                     enum qsrMsgSeverity   severity,
                     const char          * format, 
                     ... )
{
   char message[PSO_MAX_MSG_LOG];
   va_list args;

   PSO_PRE_CONDITION( pLog   != NULL );
   PSO_PRE_CONDITION( format != NULL );

   va_start( args, format );
#if HAVE_VSNPRINTF  /* safer but does not seem to be present on all 
                       platforms */
   vsnprintf( message, PSO_MAX_MSG_LOG, format, args );
#else
   vsprintf ( message, format, args );
#endif
   va_end( args );

   if ( pLog->useLog ) {
#ifdef WIN32
      ReportEvent( pLog->handle, 
                   (WORD)severity, 
                   0,
                   0, 0, 0, 
                   strlen(message), 
                   NULL, 
                   message );
#else
      syslog( severity, "%s", message );
#endif
   }
   else {
      switch ( severity ) {
      case QSR_INFO:
         fprintf( stderr, "Severity: INFO\nMessage: %s\n", message );
         break;
      case QSR_WARNING:
         fprintf( stderr, "Severity: WARNING\nMessage: %s\n", message );
         break;
      case QSR_ERROR:
         fprintf( stderr, "Severity: ERROR\nMessage: %s\n", message );
         break;
      default:
         // We should not go here... but this is an "auxiliary" service
         // and an error here should at this stage is not really important
         fprintf( stderr, "Severity: UNKNOWN\nMessage: %s\n", message );
         break;
      }
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrStartUsingLogger( qsrLogMsg * pLog )
{
   PSO_PRE_CONDITION( pLog != NULL );

   pLog->useLog = true;
#if defined ( WIN32 )
   pLog->handle = RegisterEventSource( NULL, pLog->name );
#else
   openlog( pLog->name, LOG_PID, LOG_USER );
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined ( WIN32 )
int qsrLogMsgUninstall( qsrLogMsg * pLog, const char * progName )
{
   HKEY hKey; 
   char buffer[MAX_PATH]; 

   PSO_PRE_CONDITION( progName != NULL );

   /*
    * To delete a registry key, you first need to open the key immediately
    * above it. 
    */

   sprintf( buffer, 
            "SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application" );
   
   if ( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                      buffer,
                      0,
                      KEY_ALL_ACCESS,
                      &hKey ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error opening key for EventLog (error = %d)\n",
               GetLastError() ); 
      return -1;
   }

   if ( RegDeleteKey( hKey, progName ) != ERROR_SUCCESS ) {
      fprintf( stderr, "Error deleting key for EventLog (error = %d)\n",
               GetLastError() ); 
      RegCloseKey( hKey );
      return -1;
   }
   
   RegCloseKey( hKey ); 
   return 0;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

