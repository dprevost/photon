/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/MemoryFile.h"
#include "Common/Timer.h"
#include "Common/ProcessLock.h"
#include "Common/ErrorHandler.h"
#include "Tests/PrintError.h"
#include "Common/Options.h"
#if defined(WIN32)
#  include <Process.h>
#else
#  include <sys/wait.h>
#endif

const bool expectedToPass = true;
const bool childExpectedToPass = true;

#define DEFAULT_NUM_CHILDREN   4
#define DEFAULT_TIME          30

#define CHECK_TIMER 1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   vdscProcessLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   pid_t pid, *childPid = NULL;
   unsigned long sec, nanoSec;
   vdscTimer timer;
   bool tryMode = false;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 0;
   void* ptr = NULL;   
   char filename[PATH_MAX];
   struct localData *data = NULL;
   int errcode;
   vdscMemoryFile memFile;
   vdscErrorHandler errorHandler;
   int identifier, numChilds, i, childStatus;
   char dum3[100];
   int dumId;
   bool foundError = false;
   vdscOptionHandle handle;
   char *argument;
   char strId[10], strNumChilds[10], strTime[10], strMode[5];
   struct vdscOptStruct opts[5] = 
      { 
         { 'c', "child",      1, "numChilds",  "Number of child processes" },
         { 'f', "filename",   1, "memoryFile", "Filename for shared memory" },
         { 'i', "identifier", 1, "identifier", "Identifier for the process" },
         { 'm', "mode",       1, "lockMode",   "Are we testing Acquire or TryAcquire" },
         { 't', "time",       1, "timeInSecs", "Time to run the tests" }
      };

   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitTimer( &timer );

   errcode = vdscSetSupportedOptions( 5, opts, &handle );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdscValidateUserOptions( handle, argc, argv, 1 );
   if ( errcode < 0 )
   {
      vdscShowUsage( handle, "LockConcurrency", "" );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errcode > 0 )
   {
      vdscShowUsage( handle, "LockConcurrency", "" );
      return 0;
   }
   
   if ( vdscGetShortOptArgument( handle, 'c', &argument ) )
   {
      numChilds = atoi( argument );
      if ( numChilds < 2 )
      {
         fprintf( stderr, "Number of childs must be >= to two\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      numChilds = DEFAULT_NUM_CHILDREN;

   if ( vdscGetShortOptArgument( handle, 'i', &argument ) )
   {
      identifier = atoi( argument );
      if ( identifier > numChilds )
      {
         fprintf( stderr, "Identifier must be between 0 and number of childs\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      identifier = 0;

   if ( vdscGetShortOptArgument( handle, 't', &argument ) )
   {
      maxTime = strtol( argument, NULL, 0 );
      if ( maxTime < 1 )
      {
         fprintf( stderr, "Time of test must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      maxTime = DEFAULT_TIME; /* in seconds */
  
   if ( vdscGetShortOptArgument( handle, 'm', &argument ) )
   {
      if ( strcmp( argument, "try" ) == 0 )
         tryMode = true;
   }
   
   if ( vdscGetShortOptArgument( handle, 'f', &argument ) )
   {
      strncpy( filename, argument, PATH_MAX );
      if ( filename[0] == '\0' )
      {
         fprintf( stderr, "Empty memfile name\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else
      strcpy( filename, "Memfile.mem" );
   
   vdscInitMemoryFile( &memFile, 10, filename );
   
   if ( identifier == 0 )
   {
      /* The master process */
      
      childPid = malloc( numChilds*sizeof(pid_t) );
      if ( childPid == NULL )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
         
      errcode = vdscCreateBackstore( &memFile, 0644, &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      
      errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );

      memset( ptr, 0, 10000 );
      data = (struct localData*) ptr;
   
      errcode = vdscInitProcessLock( &data->lock );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
      
      sprintf( strNumChilds, "%d", numChilds );
      sprintf( strTime, "%d", maxTime );
      if ( tryMode )
         strcpy( strMode, "try" );
      else
         strcpy( strMode, "lock" );
      
      /* Launch the childs */
      for ( i = 0; i < numChilds; ++i )
      {
         sprintf( strId, "%d", i+1 );
#if defined (WIN32)
         pid = _spawnl( _P_NOWAIT, argv[0], argv[0], 
                        "-c", strNumChilds,
                        "-f", filename,
                        "-i", strId,
                        "-m", strMode,
                        "-t", strTime,
                        NULL );
         if ( pid < 0 )
         {
            fprintf( stderr, "_spawnl failure, errno = %d\n", errno );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
         childPid[i] = pid;
#else
         pid = fork();
         if ( pid == 0 )
         {
            execl( argv[0], argv[0],
                   "-c", strNumChilds,
                   "-f", filename,
                   "-i", strId,
                   "-m", strMode,
                   "-t", strTime,                   
                    NULL );
            /* If we come here, something is wrong ! */
            ERROR_EXIT( childExpectedToPass, NULL, ; );
         }
         else if ( pid > 0 )
         {
            childPid[i] = pid;
            fprintf( stderr, "Launched child, pid = %d\n", pid );
         }
         else
         {
            fprintf( stderr, "Fork failure, errno = %d\n", errno );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
#endif
      } /* for loop launching child processes */
      
      /* Now wait for the child processes to end */
      for ( i = 0; i < numChilds; ++i )
      {
#if defined(WIN32)
         _cwait( &childStatus, childPid[i], _WAIT_CHILD );
         if ( childStatus != 0 )
            foundError = true;
#else
         waitpid( childPid[i], &childStatus, 0 );
         if ( WEXITSTATUS(childStatus) != 0 )
            foundError = true;
#endif
      }
      if ( foundError )
      {
         vdscFiniErrorHandler( &errorHandler );
         vdscFiniErrorDefs();
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }

   /*
    * Code for the child process(es) starts here
    */
   else
   {
      maxTime *= US_PER_SEC;

      errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );

      data = (struct localData*) ptr;
   
      vdscBeginTimer( &timer );

      pid = getpid();
   
      for (;;)
      {      
         if ( tryMode )
         {
            int err = vdscTryAcquireProcessLock( &data->lock, pid, 10000 );
            if ( err != 0 )
               continue;
         }
         else
            vdscAcquireProcessLock( &data->lock, pid );

         sprintf( data->dum2, "dumStr2 %d  ", identifier+1 );
         memcpy( data->dum1, data->dum2, 100 );

         sscanf( data->dum1, "%s %d", dum3, &dumId );
         if ( dumId != identifier+1 )
         {
            fprintf( stderr, "Wrong... %d %d %s-%s\n", identifier+1, 
                     dumId, data->dum1, data->dum2 );
            data->exitFlag = 1;
            vdscReleaseProcessLock( &data->lock );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
      
         vdscReleaseProcessLock( &data->lock );
      
         if ( data->exitFlag == 1 )
            break;

         loop++;

         if ( (loop % CHECK_TIMER ) == 0 )
         {
            vdscEndTimer( &timer );
            vdscCalculateTimer( &timer, &sec, &nanoSec );

            elapsedTime = sec*US_PER_SEC + nanoSec/1000;
            if ( elapsedTime > maxTime )
               break;
         }
      }
   }
   
   if ( identifier != 0 )
      printf( "Program #%d Number of loops = %lu\n", identifier, loop );
   
   unlink( filename );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

