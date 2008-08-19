/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "Nucleus/LogFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors psnInitLogFile( psnLogFile*      logFile,
                           const char*       dirName,
                           void*             pSession,
                           pscErrorHandler* pError )
{
   VDS_PRE_CONDITION( pError   != NULL );
   VDS_PRE_CONDITION( logFile  != NULL );
   VDS_PRE_CONDITION( dirName  != NULL );
   VDS_PRE_CONDITION( pSession != NULL );

   memset( logFile->filename, '\0', PATH_MAX );
   logFile->handle = -1;

   if ( access( dirName, F_OK ) != 0 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error accessing directoryfor log file = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }
   
   sprintf( logFile->filename, "%s%sLog-%d-%lu.%s", 
            dirName,
            VDS_DIR_SEPARATOR,
            getpid(),
            (unsigned long)pSession,
            VDS_LOG_EXT );
   
   logFile->handle = open( logFile->filename, 
                           O_RDWR | O_CREAT | O_APPEND , 
                           0755 );
   if ( logFile->handle == -1 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error opening log = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }
   
   logFile->initialized = PSN_LOGFILE_SIGNATURE;
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors psnLogTransaction( psnLogFile*      logFile,
                              int               transactionId,
                              pscErrorHandler* pError )
{
   char msg[80];
   char timeBuf[30];
   int err;
#if defined (WIN32)
   char tmpTime[9];
#else
   time_t t;
   struct tm formattedTime;
#endif

   VDS_PRE_CONDITION( pError  != NULL );
   VDS_PRE_CONDITION( logFile != NULL );
   VDS_INV_CONDITION( logFile->initialized == PSN_LOGFILE_SIGNATURE );
   VDS_INV_CONDITION( logFile->handle != -1 );
   
   memset( timeBuf, '\0', 30 );

#if defined (WIN32)
   _strdate( timeBuf );
   _strtime( tmpTime );
   strcat( timeBuf, " " );
   strcat( timeBuf, tmpTime );
#else
   t = time(NULL);
   localtime_r( &t, &formattedTime );
   strftime( timeBuf, 30, "%a %b %e %H:%M:%S %Y''", &formattedTime );
#endif

/*   fprintf(stderr, " ctime: %d %d %s %s\n", t, errno, szTime, n ); */

   /* szTime already includes a \n - part of the return value for ::ctime */
   memset( msg, '\0', 80 );
   sprintf( msg, "Committed  %d %s", transactionId, timeBuf );
   
   err = write( logFile->handle, msg, strlen(msg) );
   if ( err <= 0 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error write log = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }
   
   err = fdatasync( logFile->handle );
   if ( err < 0 ) {
      pscSetError( pError, PSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error fdatasync log = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnCloseLogFile( psnLogFile*     logFile,
                       pscErrorHandler* pError )
{
   int err;

   VDS_PRE_CONDITION( pError  != NULL );
   VDS_PRE_CONDITION( logFile != NULL );
   VDS_INV_CONDITION( logFile->initialized == PSN_LOGFILE_SIGNATURE );
   
   if ( logFile->handle != -1 ) {
      close( logFile->handle );
      logFile->handle = -1;
      err = unlink( logFile->filename );
      if ( err != 0 ) fprintf( stderr, "Unlink error %d\n", errno );
   }

   logFile->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

