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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "LogFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseInitLogFile( vdseLogFile*      logFile,
                           const char*       dirName,
                           void*             pSession,
                           vdscErrorHandler* pError )
{
   VDS_PRE_CONDITION( pError   != NULL );
   VDS_PRE_CONDITION( logFile  != NULL );
   VDS_PRE_CONDITION( dirName  != NULL );
   VDS_PRE_CONDITION( pSession != NULL );

   memset( logFile->filename, '\0', PATH_MAX );
   logFile->handle = -1;

   if ( access( dirName, F_OK ) != 0 )
   {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error accessing directoryfor log file = %d\n", errno );
      return VDS_CONFIG_ERROR;
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
   if ( logFile->handle == -1 )
   {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error opening log = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }
   
   logFile->initialized = VDSE_LOGFILE_SIGNATURE;
   
   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

vdsErrors vdseLogTransaction( vdseLogFile*      logFile,
                              transaction_T     transactionId,
                              vdscErrorHandler* pError )
{
   char msg[80];
   char timeBuf[30];
   time_t t = time(NULL);
   int err;
   
   VDS_PRE_CONDITION( pError  != NULL );
   VDS_PRE_CONDITION( logFile != NULL );
   VDS_INV_CONDITION( logFile->initialized == VDSE_LOGFILE_SIGNATURE );
   VDS_INV_CONDITION( logFile->handle != -1 );
   
   memset( timeBuf, '\0', 30 );
   new_ctime_r( &t, timeBuf, 30 );
/*   fprintf(stderr, " ctime: %d %d %s %s\n", t, errno, szTime, n ); */

   /* szTime already includes a \n - part of the return value for ::ctime */
   memset( msg, '\0', 80 );
   sprintf( msg, "Committed  %d %s", transactionId, timeBuf );
   
   err = write( logFile->handle, msg, strlen(msg) );
   if ( err <= 0 )
   {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error write log = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }
   
   err = fdatasync( logFile->handle );
   if ( err < 0 )
   {
      vdscSetError( pError, VDSC_ERRNO_HANDLE, errno );
//      fprintf( stderr, "Error fdatasync log = %d\n", errno );
      return VDS_LOGFILE_ERROR;
   }

   return VDS_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseCloseLogFile( vdseLogFile*     logFile,
                       vdscErrorHandler* pError )
{
   VDS_PRE_CONDITION( pError  != NULL );
   VDS_PRE_CONDITION( logFile != NULL );
   VDS_INV_CONDITION( logFile->initialized == VDSE_LOGFILE_SIGNATURE );
   
   if ( logFile->handle != -1 )
   {
      close( logFile->handle );
      logFile->handle = -1;
      int err = unlink( logFile->filename );
      if ( err != 0 )
         fprintf( stderr, "Unlink error %d\n", errno );
   }

   logFile->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

