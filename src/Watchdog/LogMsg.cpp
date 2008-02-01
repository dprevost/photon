/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if ! defined(WIN32)
#  include <syslog.h>
#endif
#include "LogMsg.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswLogMsg::vdswLogMsg( const char* progName )
   : m_useLog ( true )
#if defined ( WIN32 )
      , m_handle ( NULL )
#endif
{
#if defined ( WIN32 )
   m_handle = RegisterEventSource( NULL, progName );
#else
   openlog( progName, LOG_PID, LOG_USER );
#endif
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswLogMsg::~vdswLogMsg()
{
#if defined ( WIN32 )
   if ( m_handle != NULL ) {
      DeregisterEventSource( m_handle );
      m_handle = NULL;
   }
#else
   closelog();
#endif
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if defined ( WIN32 )
int vdswLogMsg::Install( const char * progName, 
                         const char * msgPathName,
                         int          numCategories )
{
   HKEY hKey; 
   char buffer[MAX_PATH]; 
   int eventType;
   
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void 
vdswLogMsg::SendMessage( enum wdMsgSeverity severity,
                        const char* format, 
                        ... )
{
   char message[VDS_MAX_MSG_LOG];
   va_list args;

   va_start( args, format );
#if HAVE_VSNPRINTF  /* safer but does not seem to be present on all 
                       platforms */
   vsnprintf( message, VDS_MAX_MSG_LOG, format, args );
#else
   vsprintf ( message, format, args );
#endif
   va_end( args );

   if ( m_useLog ) {
#ifdef WIN32
      ReportEvent( m_handle, 
                   severity, 
                   NULL,
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
      case WD_INFO:
         fprintf( stderr, "Severity: INFO\nMessage: %s\n", message );
         break;
      case WD_WARNING:
         fprintf( stderr, "Severity: WARNING\nMessage: %s\n", message );
         break;
      case WD_ERROR:
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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if defined ( WIN32 )
int vdswLogMsg::Uninstall( const char* progName )
{
   HKEY hKey; 
   char buffer[MAX_PATH]; 

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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

