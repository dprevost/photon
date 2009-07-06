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

#include "Common/Common.h"
#include <signal.h>
#include "Quasar/Quasar.h"
#include "Common/ErrorHandler.h"
#include "Quasar/quasarErrorHandler.h"

// This should be more than enough...
#define LINE_MAX_LEN (2*PATH_MAX)

qsrQuasar * g_pQSR = NULL;

psocErrMsgHandle g_qsrErrorHandle = -1;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrSetSigHandler();

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void sigterm_handler( int s )
{
   /*
    * We need to turn a flag on, to indicate it is time to shutdown
    */
   g_pQSR->controlWord |= QSR_SHUTDOWN_REQUEST;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void sighup_handler( int s )
{
   /*
    * Nothing yet for SIGHUP
    */
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void sigpipe_handler( int s )
{
   /*
    * SIGPIPE might occur when a socket connection is terminated (or is
    * it when it is terminated and we try to access it anyway?).
    * I need to check this... 
    */
   fprintf( stderr, "sigpipe_handler was called \n" );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int qsrGetErrorMsg( int errnum, char *msg, unsigned int msgLength )
{
   const char * theMsg;

   PSO_PRE_CONDITION( msg != NULL );

   theMsg = qsr_ErrorMessage( errnum );

   if ( theMsg == NULL ) return -1;
   if ( strlen(theMsg) >= msgLength ) return -1;

   strcpy( msg, theMsg );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrQuasarInit( qsrQuasar * pQuasar )
{
   PSO_PRE_CONDITION( pQuasar != NULL );

   pQuasar->pMemoryAddress = NULL;
   pQuasar->controlWord = 0;
   pQuasar->verifyMemOnly = false;

   qsrLogMsgInit( &pQuasar->log, PROG_NAME );

   memset( &pQuasar->params, 0, sizeof pQuasar->params );

   if ( ! psocInitErrorDefs() ) {
      fprintf( stderr, "Internal error in psocInitErrorDefs()\n" );
   }
   psocInitErrorHandler( &pQuasar->errorHandler );
   
   g_qsrErrorHandle = psocAddErrorMsgHandler( "Quasar", qsrGetErrorMsg );
   if ( g_qsrErrorHandle == PSOC_NO_ERRHANDLER ) {
      fprintf( stderr, "Error registring the error handler for Quasar errors\n" );
      fprintf( stderr, "The problem might be a lack of memory\n" );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrQuasarFini( qsrQuasar * pQuasar )
{
   PSO_PRE_CONDITION( pQuasar != NULL );

   psocFiniErrorHandler( &pQuasar->errorHandler );
   psocFiniErrorDefs();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
//
// The remaining functions are sorted in alphabetic order
// 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if !defined ( WIN32 )
bool qsrDaemon( qsrQuasar * pQuasar )
{
   pid_t pid = 0;
   int errcode;
   
   PSO_PRE_CONDITION( pQuasar != NULL );

   /*
    * Before becoming a daemon, we test the shared memory directory to make sure it
    * is valid - this is, likely, the most probable failure of the whole
    * process and it is much easier to report an error at this stage than 
    * to report it using syslog() or the Win32 equivalent.
    * [the underlying point is that we will chdir to that directory later
    * instead of "/" like standard daemons]
    *
    * We do it in two steps to help "end-users" recover from errors.
    */
   errcode = access( pQuasar->params.memLocation, F_OK );
   if ( errcode != 0 ) {
      fprintf( stderr, "Invalid directory for Photon shared memory, error = %d\n", 
               qsrLastError() );
      return false;
   }
   
   errcode = access( pQuasar->params.memLocation, R_OK | W_OK | X_OK );
   if ( errcode != 0 ) {
      fprintf( stderr, "Invalid file permissions on the %s%d\n", 
               "Photon directory, error = ",
               qsrLastError() );
      return false;
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
    *    is the location (directory) of the shared memory files - this does guarantee
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
      fprintf( stderr, "Fork failed, error = %d\n", qsrLastError() );
      return false;
   }
   
   // We are the parent
   if ( pid != 0 ) exit(0);

   // Set the log object to sent messages to the log facility of the OS 
   // instead of sending them to stderr.
   qsrStartUsingLogger( &pQuasar->log );
   
   errcode = setsid();
   if ( errcode == -1 ) {
      // The only way setsid() can fail is if we are already a group process
      // leader (our group ID == our pid). But this test does not cost 
      // anything and may detect attempts at "enhancing" the code ...
      qsrSendMessage( &pQuasar->log, QSR_ERROR,
                         "setsid() error in Daemon() (errno = %d)",
                         errno );
      return false;
   }
   pid = fork();

   if ( pid == -1 ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR,
                         "Fork error in Daemon() (errno = %d)",
                         errno );
      return false;
   }
   
   // We are the parent
   if ( pid != 0 ) exit(0);

   errcode = chdir( pQuasar->params.memLocation );
   if ( errcode != 0 ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR,
                         "chdir() error in Daemon() (errno = %d)",
                         errno );
      return false;
   }

   umask( 0 );

   close( 0 );
   close( 1 );
   close( 2 );
   
   qsrSendMessage( &pQuasar->log, QSR_INFO, "Photon Quasar initialized as a daemon" );   
   return true;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrHelp( const char * progName )
{
   PSO_PRE_CONDITION( progName != NULL );

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined ( WIN32 )
bool qsrInstall( qsrQuasar * pQuasar )
{
   SC_HANDLE   hService;
   SC_HANDLE   hManager;
   char progPath[PATH_MAX];
   int errcode;
   HKEY hKey;

   PSO_PRE_CONDITION( pQuasar != NULL );

   memset( progPath, 0, PATH_MAX );
   if ( GetModuleFileName( NULL, progPath, PATH_MAX ) == 0 ) {
      fprintf( stderr, "GetModuleFileName error = %d\n", GetLastError() );
      return false;
   }

   hManager = OpenSCManager( NULL,
                             NULL,
                             SC_MANAGER_ALL_ACCESS );
   if ( hManager == NULL ) {
      fprintf( stderr, "OpenSCManager error = %d\n", GetLastError() );
      return false;
   }
    
   hService = CreateService( hManager,
                             "quasar",
                             "Photon Quasar",
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
      return false;
   }

   CloseServiceHandle(hService);
   CloseServiceHandle(hManager);

   /*
    * Now that the service is created, we need to save into the registry
    * all pertinent information that will be needed for the proper 
    * running of this service. This include the location of the shared-memory
    * directory, the server address (as used by client apps), etc.
    */

   errcode = RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                           "SYSTEM\\CurrentControlSet\\Services\\quasar",
                           0,
                           KEY_SET_VALUE,
                           &hKey );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegOpenKeyEx error = %d\n", GetLastError() );
      return false;
   }
   
   errcode = RegSetValueEx( hKey, 
                            PSO_LOCATION,
                            0, 
                            REG_SZ,
                            (LPBYTE)pQuasar->params.memLocation, 
                            strlen(pQuasar->params.memLocation)+1 );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return false;
   }

   errcode = RegSetValueEx( hKey, 
                            PSO_QSRADDRESS,
                            0, 
                            REG_SZ,
                            (LPBYTE)pQuasar->params.qsrAddress,
                            strlen(pQuasar->params.qsrAddress)+1 );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return false;
   }

   errcode = RegSetValueEx( hKey, 
                            PSO_MEMSIZE,
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&pQuasar->params.memorySizekb,
                            sizeof(size_t) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return false;
   }

   errcode = RegSetValueEx( hKey, 
                            PSO_USE_LOG,
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&pQuasar->params.logOn,
                            sizeof(bool) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return false;
   }

   errcode = RegSetValueEx( hKey, 
                            PSO_FILEPERMS,
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&pQuasar->params.filePerms,
                            sizeof(int) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return false;
   }

   errcode = RegSetValueEx( hKey, 
                            PSO_DIRPERMS, 
                            0, 
                            REG_DWORD, 
                            (LPBYTE)&pQuasar->params.dirPerms,
                            sizeof(int) );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegSetValueEx error = %d\n", GetLastError() );
      RegCloseKey( hKey );
      return false;
   }

   RegCloseKey( hKey );
   return true;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrQuasarReadConfig( qsrQuasar * pQuasar, const char* cfgname )
{
   int len;
   bool rc;
   
   PSO_PRE_CONDITION( pQuasar != NULL );
   PSO_PRE_CONDITION( cfgname != NULL );

   rc = qsrReadConfig( cfgname, &pQuasar->params, 0, &pQuasar->errorHandler );
   PSO_POST_CONDITION( rc == true || rc == false );
   if ( ! rc ) {
      memset( pQuasar->errorMsg, 0, QSR_MSG_LEN );
      sprintf( pQuasar->errorMsg, "%s%d%s",
                  "Config error, error code = ", 
                  psocGetLastError( &pQuasar->errorHandler ),
                  ", error message = " );
      len = strlen(pQuasar->errorMsg);
      psocGetErrorMsg( &pQuasar->errorHandler, &pQuasar->errorMsg[len], QSR_MSG_LEN-len );
   }

   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined ( WIN32 )
bool qsrReadRegistry( qsrQuasar * pQuasar )
{
   int errcode;
   HKEY hKey;
   unsigned long length;
   
   PSO_PRE_CONDITION( pQuasar != NULL );

   errcode = RegOpenKeyEx( HKEY_LOCAL_MACHINE, 
                           "SYSTEM\\CurrentControlSet\\Services\\quasar",
                           0,
                           KEY_QUERY_VALUE,
                           &hKey );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegOpenKeyEx error = %d", 
                         GetLastError() );
      return false;
   }

   length = PATH_MAX;
   errcode = RegQueryValueEx( hKey, 
                              PSO_LOCATION,
                              NULL, 
                              NULL, 
                              (LPBYTE)pQuasar->params.memLocation, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return false;
   }

   length = sizeof(size_t);
   errcode = RegQueryValueEx( hKey, 
                              PSO_MEMSIZE, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&pQuasar->params.memorySizekb, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return false;
   }

   length = PATH_MAX;
   errcode = RegQueryValueEx( hKey, 
                              PSO_QSRADDRESS, 
                              NULL, 
                              NULL, 
                              (LPBYTE)pQuasar->params.qsrAddress,
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return false;
   }

   length = sizeof(bool);
   errcode = RegQueryValueEx( hKey, 
                              PSO_USE_LOG, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&pQuasar->params.logOn, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return false;
   }

   length = sizeof(int);
   errcode = RegQueryValueEx( hKey, 
                              PSO_FILEPERMS, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&pQuasar->params.filePerms, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return false;
   }

   length = sizeof(int);
   errcode = RegQueryValueEx( hKey, 
                              PSO_DIRPERMS, 
                              NULL, 
                              NULL, 
                              (LPBYTE)&pQuasar->params.dirPerms, 
                              &length );
   if ( errcode != ERROR_SUCCESS ) {
      qsrSendMessage( &pQuasar->log, QSR_ERROR, 
                         "RegQueryValueEx error = %d", 
                         GetLastError() );
      return false;
   }

   return true;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrRun()
{
   bool rc;
   
#if defined ( WIN32 )
   bool deallocQSR = false;
   if ( g_pQSR == NULL ) {
      // This condition is possible if run is called by the NT SCM (Service
      // Control Manager) instead of being called from main().
      g_pQSR = (qsrQuasar*) malloc( sizeof(qsrQuasar) );

      // A failure here is very unlikely - however since we need the 
      // quasar object to log errors... the only way to alert of a
      // problem is by forcing a crash!!!
      assert( g_pQSR != NULL );

      qsrQuasarInit( g_pQSR );

      deallocQSR = true;
      
      // Set the log object to sent messages to the log facility of the OS 
      // instead of sending them to stderr.
      qsrStartUsingLogger( &g_pQSR->log );
         
      // We have our object but it is not properly initialized - we need
      // to access the registry (the NT service equivalent of calling 
      // ReadConfig() from main().
      rc = qsrReadRegistry( g_pQSR );
      PSO_POST_CONDITION( rc == true || rc == false );
      if ( ! rc ) {
         qsrSendMessage( &g_pQSR->log, QSR_ERROR, 
                          "ReadRegistry failed - aborting..." );
         return;
      }
   }
#endif
   
   PSO_PRE_CONDITION( g_pQSR != NULL );

   rc = qsrSetSigHandler();
   PSO_POST_CONDITION( rc == true || rc == false );
   if ( ! rc ) {
      qsrSendMessage( &g_pQSR->log, 
                       QSR_ERROR,
                       "Signal Handler Installation error %d",
                       qsrLastError() );
      return;
   }

   rc = qsrPrepareConnection( &g_pQSR->acceptor, g_pQSR );
   PSO_POST_CONDITION( rc == true || rc == false );
   if ( ! rc ) {
      qsrSendMessage( &g_pQSR->log, QSR_ERROR,
         "Error in PrepareConnection() - aborting..." );
      return;
   }
   
   qsrWaitForConnections( &g_pQSR->acceptor );
   
#if defined ( WIN32 )
   if ( deallocQSR ) {
      qsrQuasarFini( g_pQSR );
      free( g_pQSR );
      g_pQSR = NULL;
   }
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool qsrSetSigHandler()
{
#if defined(WIN32)
   PSO_PRE_CONDITION( g_pQSR != NULL );

   signal( SIGINT,  sigterm_handler );
   signal( SIGTERM, sigterm_handler );

#else
   int errcode = 0;
   sigset_t old_set, new_set;
   struct sigaction action;
   
   PSO_PRE_CONDITION( g_pQSR != NULL );

   /*
    * Get the current process mask (meaning the list of signals which are
    * currently blocked)
    */
   sigemptyset( &new_set );

   errcode = sigprocmask( SIG_BLOCK, &new_set, &old_set ); 
   if ( errcode != 0 ) {
      qsrSendMessage( &g_pQSR->log, QSR_ERROR, "%s%d\n",
         "sigprocmask error, errno = ", errno );
      return false;
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
      qsrSendMessage( &g_pQSR->log, QSR_ERROR, "%s%d\n",
         "sigprocmask error, errno = ", errno );
      return false;
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
      qsrSendMessage( &g_pQSR->log, QSR_ERROR, "%s%d\n",
         "sigaction (SIGTERM) error, errno = ", errno );
      return false;
   }
   errcode = sigaction( SIGINT,  &action, NULL );
   if ( errcode != 0 ) {
      qsrSendMessage( &g_pQSR->log, QSR_ERROR, "%s%d\n",
         "sigaction (SIGINT) error, errno = ", errno );
      return false;
   }
   action.sa_handler = sighup_handler;
   errcode = sigaction( SIGHUP, &action, NULL );
   if ( errcode != 0 ) {
      qsrSendMessage( &g_pQSR->log, QSR_ERROR, "%s%d\n",
         "sigaction (SIGHUP) error, errno = ", errno );
      return false;
   }

   sigemptyset( &action.sa_mask );   
   action.sa_handler = sigpipe_handler;
   errcode = sigaction( SIGPIPE, &action, NULL );
   if ( errcode != 0 ) {
      qsrSendMessage( &g_pQSR->log, QSR_ERROR, "%s%d\n",
         "sigaction (SIGPIPE) error, errno = ", errno );
      return false;
   }

#endif
   return true;
}
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if defined ( WIN32 )
void qsrUninstall( qsrQuasar * pQuasar )
{
   SC_HANDLE   hService;
   SC_HANDLE   hManager;
   int errcode;
   HKEY hKey;
   SERVICE_STATUS status;
   
   PSO_PRE_CONDITION( pQuasar != NULL );

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
                           "SYSTEM\\CurrentControlSet\\Services\\quasar",
                           0,
                           KEY_SET_VALUE,
                           &hKey );
   if ( errcode != ERROR_SUCCESS ) {
      fprintf( stderr, "RegOpenKeyEx error = %d\n", GetLastError() );
   }
   else {
      errcode = RegDeleteValue( hKey, PSO_LOCATION );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, PSO_QSRADDRESS );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, PSO_MEMSIZE );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey,  PSO_USE_LOG);
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, PSO_FILEPERMS );
      if ( errcode != ERROR_SUCCESS ) {
         fprintf( stderr, "RegDeleteValue error = %d\n", GetLastError() );
      }
      errcode = RegDeleteValue( hKey, PSO_DIRPERMS );
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
                           "qsrc",
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

