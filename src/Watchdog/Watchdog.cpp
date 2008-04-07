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

#include "Common/Common.h"
#include <signal.h>
#include "Watchdog/Watchdog.h"
#include "Common/ErrorHandler.h"
#if HAVE_NEW
#  include <new>
#else
#  include <new.h>
#endif

// This should be more than enough...
#define LINE_MAX_LEN (2*PATH_MAX)

vdswWatchdog* vdswWatchdog::g_pWD = NULL;

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

BEGIN_C_DECLS

RETSIGTYPE sigterm_handler( int s )
{
   /*
    * We need to turn a flag on, to indicate it is time to shutdown
    */
   vdswWatchdog::g_pWD->m_controlWord |= WD_SHUTDOWN_REQUEST;
}

RETSIGTYPE sighup_handler( int s )
{
   /*
    * Nothing yet for SIGHUP
    */
}

RETSIGTYPE sigpipe_handler( int s )
{
   /*
    * SIGPIPE might occur when a socket connection is terminated (or is
    * it when it is terminated and we try to access it anyway?).
    * I need to check this... 
    */
   fprintf( stderr, "sigpipe_handler was called \n" );
}

END_C_DECLS

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswWatchdog::vdswWatchdog()
   : m_pMemoryAddress ( NULL      ),
     m_controlWord    ( 0         ),
     m_log            ( PROG_NAME ),
     m_verifyVDSOnly  ( false     )
{
   memset( &m_params, 0, sizeof m_params );

   vdscInitErrorDefs();
   vdscInitErrorHandler( &m_errorHandler );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

vdswWatchdog::~vdswWatchdog()
{
   vdscFiniErrorHandler( &m_errorHandler );
   vdscFiniErrorDefs();
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
//
// The remaining functions are sorted in alphabetic order
// 
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if !defined ( WIN32 )
int vdswWatchdog::Daemon()
{
   pid_t pid = 0;
   int errcode;
   
   /*
    * Before becoming a daemon, we test the VDS directory to make sure it
    * is valid - this is, likely, the most probable failure of the whole
    * process and it is much easier to report an error at this stage than 
    * to report it using syslog() or the Win32 equivalent.
    * [the underlying point is that we will chdir to that directory later
    * instead of "/" like standard daemons]
    *
    * We do it in two steps to help "end-users" recover from errors.
    */
   errcode = access( m_params.wdLocation, F_OK );
   if ( errcode != 0 ) {
      fprintf( stderr, "Invalid directory for VDS, error = %d\n", 
               LastError() );
      return -1;
   }
   
   errcode = access( m_params.wdLocation, R_OK | W_OK | X_OK );
   if ( errcode != 0 ) {
      fprintf( stderr, "Invalid file permissions on the %s%d\n", 
               "VDS directory, error = ",
               LastError() );
      return -1;
   }
   
   /*
    * We follow the standard procedures to become a daemon:
    *
    * 1) fork so that we are sure that we are not a process group leader 
    *    (needed to become a session leader)
    * 2) become session leader (and with no terminal associated with the 
    *    session)
    * 3) fork again so that we are not the session leader and cannot acquire
    *    a terminal in the future (terminals are associated with session
    *    leaders)
    * 4) chdir to appropriate location (the appropriate location in our case 
    *    is the location (directory) of the VDS files - this does guarantee
    *    that the underlying file system cannot be unmounted without 
    *    properly terminating the daemon first [at least under normal
    *    conditions of operations]
    * 5) change umask to zero (to make sure we take full control of file 
    *    permissions, just in case)
    * 6) close all file handles (stdin, stdout and stderr)
    *
    */

   pid = fork();

   if ( pid == -1 ) {
      fprintf( stderr, "Fork failed, error = %d\n", LastError() );
      return -1;
   }
   
   // We are the parent
   if ( pid != 0 ) exit(0);

   // Set the log object to sent messages to the log facility of the OS 
   // instead of sending them to stderr.
   m_log.StartUsingLogger();
   
   errcode = setsid();
   if ( errcode == -1 ) {
      // The only way setsid() can fail is if we are already a group process
      // leader (our group ID == our pid). But this test does not cost 
      // anything and may detect attempts at "enhancing" the code ...
      m_log.SendMessage( WD_ERROR,
                         "setsid() error in Daemon() (errno = %d)",
                         errno );
      return -1;
   }
   pid = fork();

   if ( pid == -1 ) {
      m_log.SendMessage( WD_ERROR,
                         "Fork error in Daemon() (errno = %d)",
                         errno );
      return -1;
   }
   
   // We are the parent
   if ( pid != 0 ) exit(0);

   errcode = chdir( m_params.wdLocation );
   if ( errcode != 0 ) {
      m_log.SendMessage( WD_ERROR,
                         "chdir() error in Daemon() (errno = %d)",
                         errno );
      return -1;
   }

   umask( 0 );

   close( 0 );
   close( 1 );
   close( 2 );
   
   m_log.SendMessage( WD_INFO, "VDSF Watchdog initialized as a daemon" );   
   return 0;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void vdswWatchdog::Help( const char* progName ) const
{
   fprintf( stderr, "Usage: %s [options] config_file \n", progName );
   fprintf( stderr, "Options:\n" );
   
   fprintf( stderr, "  -h, --help             %s\n", 
            "Display the current information, then exit." );
   fprintf( stderr, "  -i, --install          %s\n", 
            "Install the program as a NT service (Windows only)." );
   fprintf( stderr, "  -u, --uninstall        %s\n", 
            "Uninstall the program as a NT service (Windows only)." );
   fprintf( stderr, "                         %s\n", 
            "[the config file is not used for this option]" );
   fprintf( stderr, "  -d, --daemon           %s\n", 
            "Run the program as a Unix daemon (Unix/linux only)." );
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if defined ( WIN32 )
int vdswWatchdog::Install()
{
   SC_HANDLE   hService;
   SC_HANDLE   hManager;
   char progPath[PATH_MAX];
   int errcode;
   HKEY hKey;

   memset( progPath, 0, PATH_MAX );
   if ( GetModuleFileName( NULL, progPath, PATH_MAX ) == 0 ) {
      fprintf( stderr, "GetModuleFileName error = %d\n", GetLastError() );
      return -1;
   }

   hManager = OpenSCManager( NULL,
                             NULL,
                             SC_MANAGER_ALL_ACCESS );
   if ( hManager == NULL ) {
      fprintf( stderr, "OpenSCManager error = %d\n", GetLastError() );
      return -1;
   }
    
   hService = CreateService( hManager,
                             "vdswd",
                             "VDSF Watchdog",
                             SERVICE_ALL_ACCESS,
                             SERVICE_WIN32_OWN_PROCESS,
                             SERVICE_DEMAND_START,
                             SERVICE_ERROR_NORMAL,
                             progPath,
                             NULL,
                             NULL,
                             NULL,
                             NULL,
                             NULL );
   if ( hService == NULL ) {
      fprintf( stderr, "CreateService error = %d\n", GetLastError() );
      CloseServiceHandle(hManager);
      return -1;
   }

   CloseServiceHandle(hService);
   CloseServiceHandle(hManager);

   /*
    * Now that the service is created, we need to save into the registry
    * all pertinent information that will be needed for the proper 
    * running of this service. This include the location of of the VDS
    * directory, the Watchdog address (as used by client apps), etc.
    */

   errcode = RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                           "SYSTEM\\CurrentControlSet\\Services\\vdswd",
                           NULL,
                           KEY_SET_VALUE,
                           &hKey );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegOpenKeyEx error = %d\n", GetLastError() );
      return -1;
   }
   
   errcode = RegSetValueEx( hKey, 
                            VDS_LOCATION,
                            0, 
                            REG_SZ,
                            (LPBYTE)m_params.wdLocation, 
                            strlen(m_params.wdLocation)+1 );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return -1;
   }

   errcode = RegSetValueEx( hKey, 
                            VDS_WDADDRESS,
                            0, 
                            REG_SZ,
                            (LPBYTE)m_params.wdAddress,
                            strlen(m_params.wdAddress)+1 );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return -1;
   }

   errcode = RegSetValueEx( hKey, 
                            VDS_MEMSIZE,
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&m_params.memorySizekb,
                            sizeof(size_t) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return -1;
   }

   errcode = RegSetValueEx( hKey, 
                            VDS_USE_LOG,
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&m_params.logOn,
                            sizeof(bool) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return -1;
   }

   errcode = RegSetValueEx( hKey, 
                            VDS_FILEPERMS,
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&m_params.filePerms,
                            sizeof(int) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return -1;
   }

   errcode = RegSetValueEx( hKey, 
                            VDS_DIRPERMS, 
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&m_params.dirPerms,
                            sizeof(int) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return -1;
   }

   RegCloseKey( hKey );
   return 0;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdswWatchdog::ReadConfig( const char* cfgname )
{
   int errcode;
   const char* missing;

   errcode = vdscReadConfig( cfgname, &m_params, &missing, &m_errorHandler );
   if ( errcode != 0 ) {
      memset( m_errorMsg, 0, WD_MSG_LEN );
      if ( missing == NULL ) {
         sprintf( m_errorMsg, "%s%d%s",
                  "Error reading config file, error code = ", 
                  vdscGetLastError( &m_errorHandler ),
                  ", error message = " );
         vdscGetErrorMsg( &m_errorHandler, m_errorMsg, WD_MSG_LEN );
      }  
      else {
         sprintf( m_errorMsg, "%s%s",
                  missing,
                  " parameter is missing from config file" );
      }
   }
   return errcode;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if defined ( WIN32 )
int vdswWatchdog::ReadRegistry()
{
   int errcode;
   HKEY hKey;
   unsigned long length;
   
   errcode = RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                           "SYSTEM\\CurrentControlSet\\Services\\vdswd",
                           NULL,
                           KEY_QUERY_VALUE,
                           &hKey );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegOpenKeyEx error = %d", 
                         GetLastError() );
      return -1;
   }

   length = PATH_MAX;
   errcode = RegQueryValueEx( hKey, 
                              VDS_LOCATION,
                              NULL, 
                              NULL, 
                              (LPBYTE)m_params.wdLocation, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return -1;
   }

   length = sizeof(size_t);
   errcode = RegQueryValueEx( hKey, 
                              VDS_MEMSIZE, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&m_params.memorySizekb, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return -1;
   }

   length = PATH_MAX;
   errcode = RegQueryValueEx( hKey, 
                              VDS_WDADDRESS, 
                              NULL, 
                              NULL, 
                              (LPBYTE)m_params.wdAddress,
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return -1;
   }

   length = sizeof(bool);
   errcode = RegQueryValueEx( hKey, 
                              VDS_USE_LOG, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&m_params.logOn, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return -1;
   }

   length = sizeof(int);
   errcode = RegQueryValueEx( hKey, 
                              VDS_FILEPERMS, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&m_params.filePerms, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return -1;
   }

   length = sizeof(int);
   errcode = RegQueryValueEx( hKey, 
                              VDS_DIRPERMS, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&m_params.dirPerms, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      m_log.SendMessage( WD_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return -1;
   }

   return 0;
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

// A reminder - this function is static
void vdswWatchdog::Run()
{
   int errcode = 0;
   
#if defined ( WIN32 )
   bool deallocWD = false;
   if ( g_pWD == NULL ) {
      // This condition is possible if run is called by the NT SCM (Service
      // Control Manager) instead of being called from main().
      g_pWD = (vdswWatchdog*) malloc( sizeof(vdswWatchdog) );

      // A failure here is very unlikely - however since we need the 
      // watchdog object to log errors... the only way to alert of a
      // problem is by forcing a crash!!!
      assert( g_pWD != NULL );

      g_pWD = new (g_pWD) vdswWatchdog;

      deallocWD = true;
      
      // Set the log object to sent messages to the log facility of the OS 
      // instead of sending them to stderr.
      g_pWD->m_log.StartUsingLogger();
         
      // We have our object but it is not properly initialized - we need
      // to access the registry (the NT service equivalent of calling 
      // ReadConfig() from main().
      errcode = g_pWD->ReadRegistry();
      if ( errcode != 0 ) {
         g_pWD->m_log.SendMessage( WD_ERROR, 
                                   "ReadRegistry failed - aborting..." );
         return;
      }
   }
#endif
   
   errcode = g_pWD->SetSigHandler();
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR,
                                "Signal Handler Installation error %d",
                                g_pWD->LastError() );
      return;
   }

   errcode = g_pWD->m_acceptor.PrepareConnection( g_pWD );
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR,
         "Error in PrepareConnection() - aborting..." );
      return;
   }
   
   g_pWD->m_acceptor.WaitForConnections();
   
#if defined ( WIN32 )
   if ( deallocWD ) {
      g_pWD->~vdswWatchdog();
      free( g_pWD );
      g_pWD = NULL;
   }
#endif
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int vdswWatchdog::SetSigHandler()
{
#if defined(WIN32)
   signal( SIGINT,  sigterm_handler );
   signal( SIGTERM, sigterm_handler );

#else
   int errcode = 0;
   sigset_t old_set, new_set;
   struct sigaction action;
   
   /*
    * Get the current process mask (meaning the list of signals which are
    * currently blocked)
    */
   sigemptyset( &new_set );

   errcode = sigprocmask( SIG_BLOCK, &new_set, &old_set ); 
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR, "%s%d\n",
         "sigprocmask error, errno = ", errno );
      return -1;
   }
   /*
    * Remove the signals we want from old_set and add to it the signal we  
    * don't care for. This is probably a useless task since the signals of 
    * interest to us are most likely already unblocked. But since this
    * code should be portable and since we never know how some platform
    * might be setup... lets go through the loops anyway ;-)
    *
    * [Note that we are not adding signals to the mask at this point]
    */

   sigdelset( &old_set, SIGPIPE );
   sigdelset( &old_set, SIGHUP  );
   sigdelset( &old_set, SIGINT );
   sigdelset( &old_set, SIGTERM );

   /*
    *  Set the new process mask
    */
   errcode = sigprocmask( SIG_SETMASK, &old_set, &new_set );
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR, "%s%d\n",
         "sigprocmask error, errno = ", errno );
      return -1;
   }

   /*
    * Time to setup our signal handlers. Note that we block SIGPIPE from
    * interrupting most handlers.
    */

   sigemptyset( &action.sa_mask );
   sigaddset( &action.sa_mask, SIGPIPE );
   action.sa_flags = SA_RESTART;

   action.sa_handler = sigterm_handler;
   errcode = sigaction( SIGTERM, &action, NULL );
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR, "%s%d\n",
         "sigaction (SIGTERM) error, errno = ", errno );
      return -1;
   }
   errcode = sigaction( SIGINT,  &action, NULL );
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR, "%s%d\n",
         "sigaction (SIGINT) error, errno = ", errno );
      return -1;
   }
   action.sa_handler = sighup_handler;
   errcode = sigaction( SIGHUP, &action, NULL );
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR, "%s%d\n",
         "sigaction (SIGHUP) error, errno = ", errno );
      return -1;
   }

   sigemptyset( &action.sa_mask );   
   action.sa_handler = sigpipe_handler;
   errcode = sigaction( SIGPIPE, &action, NULL );
   if ( errcode != 0 ) {
      g_pWD->m_log.SendMessage( WD_ERROR, "%s%d\n",
         "sigaction (SIGPIPE) error, errno = ", errno );
      return -1;
   }

#endif
   return 0;
}
   
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#if defined ( WIN32 )
void vdswWatchdog::Uninstall()
{
   SC_HANDLE   hService;
   SC_HANDLE   hManager;
   int errcode;
   HKEY hKey;
   SERVICE_STATUS status;
   
   fprintf( stderr, "%s\n%s\n%s\n%s\n%s\n",
            "WARNING - Some errors while uninstalling might not be real",
            "errors if the service was not properly initialized in the",
            "first place. Examine the messages carefully.",
            "The uninstallation process will attempt to continue ",
            "when ever possible." );

   /*
    * We first remove the values added under the registry key of our 
    * service.
    */
   errcode = RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                           "SYSTEM\\CurrentControlSet\\Services\\vdswd",
                           NULL,
                           KEY_SET_VALUE,
                           &hKey );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegOpenKeyEx error = %d\n", GetLastError() );
   }
   else {
      errcode = RegDeleteValue( hKey, VDS_LOCATION );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, VDS_WDADDRESS );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, VDS_MEMSIZE );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey,  VDS_USE_LOG);
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, VDS_FILEPERMS );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, VDS_DIRPERMS );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
   }
   
   /*
    * Remove the service itself. Since the service might be running,
    * we might need to order it to stop (and of course, we have to wait 
    * until it is stopped).
    */

   hManager = OpenSCManager( NULL,
                             NULL,
                             SC_MANAGER_ALL_ACCESS );
   if ( hManager == NULL ) {
      // Nothing more can be done at this point...
      fprintf( stderr, "OpenSCManager error = %d\n", GetLastError() );
      return;
   }

   hService = OpenService( hManager,
                           "vdswc",
                           SERVICE_ALL_ACCESS );
   if ( hService == NULL ) {
      // Nothing more can be done at this point...
      fprintf( stderr, "OpenService error = %d\n", GetLastError() );
      CloseServiceHandle(hManager);
      return;
   }
   
   errcode = QueryServiceStatus( hService, &status );
   if ( errcode == 0 ) {
      fprintf( stderr, "QueryServiceStatus error = %d\n", GetLastError() );
      fprintf( stderr, "Will attempt to remove the service anyway...\n" );
   }  

   if ( errcode != 0 && status.dwCurrentState != SERVICE_STOPPED ) {
      // stop that service (or at least try)
      errcode = ControlService( hService, SERVICE_CONTROL_STOP, &status );
      if ( errcode == 0 ) {
         fprintf( stderr, "ControlService (stop) error = %d\n", 
                  GetLastError() );
         fprintf( stderr, "Will attempt to remove the service anyway...\n" );
      }
      else {
         int loop = 0;
         do  {
            
            Sleep( 1000 ); // reminder: this is in millisecs (I always forget)
            
            errcode = QueryServiceStatus( hService, &status );
            
            /*
             * The service at this point is either STOPPED or the STOP is
             * pending. We wait until it stops to be PENDING (and we test
             * for an error later) or for a maximum of 30 secs.
             */
            loop++;
            
         } while ( errcode != 0 && loop < 30 );
         
         if ( status.dwCurrentState != SERVICE_STOPPED ) {
            fprintf( stderr, "It seems the service was not stopped...\n");
            fprintf( stderr, "Will attempt to remove the service anyway...\n");
         }
      }
   }
   errcode = DeleteService( hService );
   if ( errcode == 0 ) {
      fprintf( stderr, "DeleteService error = %d\n", GetLastError() );
   }
   
   CloseServiceHandle( hService );
   CloseServiceHandle( hManager );
}
#endif

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

