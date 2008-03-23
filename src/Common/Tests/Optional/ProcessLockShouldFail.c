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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * This test is very similar to the LockConcurrency test... except that
 * this program does not always lock properly (on purpose)!!! The rate of 
 * failure of our locks is controlled by the #define DEFAULT_FAILURE_RATE.
 *
 * In a way, it insures that we would be able to detect a failure in
 * our locking mechanism using the standard LockConcurrency test. 
 *
 * The amount of time we will try to observe a concurrency failure is
 * determined by the first argument to the program.
 */
 
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Common/MemoryFile.h"
#include "Common/Timer.h"
#include "Common/ProcessLock.h"
#include "Tests/PrintError.h"
#include "Common/Options.h"
#if defined(WIN32)
#  include <Process.h>
#else
#  include <sys/wait.h>
#endif

const bool expectedToPass = true;
const bool childExpectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   vdscProcessLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

/*
 * Misnamed define... the failure rate is 1/DEFAULT_FAILURE_RATE...
 *
 * I would prefer a much lower failure rate but it would mean
 * being very patient... (failures can only be seen when context
 * switches are done and with time slices of 1/1000 sec. (on many
 * modern OSes), it would force us to run the test for a lot longer).
 *
 * [DEFAULT_FAILURE_RATE 500 --> 0.2% failure]
 */
#define DEFAULT_FAILURE_RATE 500
#define DEFAULT_NUM_CHILDREN   4
#define DEFAULT_TIME         300

#define CHECK_TIMER 1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   pid_t *childPid, pid, mypid;
   unsigned long sec, nanoSec;
   vdscTimer timer;
   bool foundError = false, tryMode = false;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 1, failureRate;
   void* ptr = NULL;   
   char filename[PATH_MAX];
   struct localData *data = NULL;
   int errcode;
   vdscMemoryFile memFile;
   vdscErrorHandler errorHandler;
   int identifier, numChilds, i, childStatus;
   
   char dum3[100];
   int dumId;
   vdscOptionHandle handle;
   char *argument;
   char strId[10], strNumChilds[10], strTime[10], strMode[5], strRate[10];
   struct vdscOptStruct opts[6] = { 
      { 'c', "child",      1, "numChilds",     "Number of child processes" },
      { 'f', "filename",   1, "memoryFile",    "Filename for shared memory" },
      { 'i', "identifier", 1, "identifier",    "Identifier for the process (do not used)" },
      { 'm', "mode",       1, "lockMode",      "Set this to 'try' for testing TryAcquire" },
      { 'r', "rate",       1, "rateOfFailure", "Inverse rate: 1000 means a rate of 0.1%" },
      { 't', "time",       1, "timeInSecs",    "Time to run the tests" }
   };

   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitTimer( &timer );   

   errcode = vdscSetSupportedOptions( 6, opts, &handle );
   if ( errcode != 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdscValidateUserOptions( handle, argc, argv, 1 );
   if ( errcode < 0 ) {
      vdscShowUsage( handle, "LockShouldFail", "" );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errcode > 0 ) {
      vdscShowUsage( handle, "LockShouldFail", "" );
      return 0;
   }
   
   if ( vdscGetShortOptArgument( handle, 'c', &argument ) ) {
      numChilds = atoi( argument );
      if ( numChilds < 2 ) {
         fprintf( stderr, "Number of childs must be >= to two\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      numChilds = DEFAULT_NUM_CHILDREN;
   
   if ( vdscGetShortOptArgument( handle, 'i', &argument ) ) {
      identifier = atoi( argument );
      if ( identifier > numChilds ) {
         fprintf( stderr, "Identifier must be between 0 and number of childs\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      identifier = 0;

   if ( vdscGetShortOptArgument( handle, 't', &argument ) ) {
      maxTime = strtol( argument, NULL, 0 );
      if ( maxTime < 1 ) {
         fprintf( stderr, "Time of test must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      maxTime = DEFAULT_TIME; /* in seconds */
  
   if ( vdscGetShortOptArgument( handle, 'm', &argument ) ) {
      if ( strcmp( argument, "try" ) == 0 )
         tryMode = true;
   }
   
   if ( vdscGetShortOptArgument( handle, 'f', &argument ) ) {
      strncpy( filename, argument, PATH_MAX );
      if ( filename[0] == '\0' )
      {
         fprintf( stderr, "Empty memfile name\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else
      strcpy( filename, "Memfile.mem" );

   if ( vdscGetShortOptArgument( handle, 'r', &argument ) ) {
      failureRate = strtol( argument, NULL, 0 );
      if ( failureRate < 1 ) {
         fprintf( stderr, "Failure rate must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      failureRate = DEFAULT_FAILURE_RATE;

   vdscInitMemoryFile( &memFile, 10, filename );
   
   if ( identifier == 0 ) {
      /*
       * This is the parent!
       */
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
      data = (struct localData *)ptr;
      
      errcode = vdscInitProcessLock( &data->lock );
      if ( errcode < 0 )
         ERROR_EXIT( 1, NULL, ; );

      vdscSyncMemFile( &memFile, &errorHandler );
   
      vdscCloseMemFile( &memFile, &errorHandler );
      ptr = NULL;
   
      sprintf( strNumChilds, "%d", numChilds );
      sprintf( strRate, "%u", (unsigned int)failureRate );
      sprintf( strTime, "%u", (unsigned int)maxTime );
      if ( tryMode )
         strcpy( strMode, "try" );
      else
         strcpy( strMode, "lock" );
      
      for ( i = 0; i < numChilds; ++i ) {
         sprintf( strId, "%d", i+1 );
#if defined (WIN32)
         pid = _spawnl( _P_NOWAIT, argv[0], argv[0], 
                        "-c", strNumChilds,
                        "-f", filename,
                        "-i", strId,
                        "-m", strMode,
                        "-r", strRate,
                        "-t", strTime,
                        NULL );
         if ( pid < 0 ) {
            fprintf( stderr, "_spawnl failure, errno = %d\n", errno );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
         childPid[i] = pid;
#else
         pid = fork();
         if ( pid == 0 ) {
            execl( argv[0], argv[0],
                   "-c", strNumChilds,
                   "-f", filename,
                   "-i", strId,
                   "-m", strMode,
                   "-r", strRate,
                   "-t", strTime,                   
                    NULL );
            /* If we come here, something is wrong ! */
            ERROR_EXIT( childExpectedToPass, NULL, ; );
         }
         else if ( pid > 0 ) {
            childPid[i] = pid;
            fprintf( stderr, "Launched child, pid = %d\n", pid );
         }
         else {
            fprintf( stderr, "Fork failure, errno = %d\n", errno );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
#endif
      } /* for loop launching child processes */
      
      /* We now wait for the children to exit */
      for ( i = 0; i < numChilds; ++i ) {
#if defined(WIN32)
         _cwait( &childStatus, childPid[i], _WAIT_CHILD );
         if ( childStatus == 0 )
            foundError = true;
#else
         waitpid( childPid[i], &childStatus, 0 );
         if ( WEXITSTATUS(childStatus) == 0 )
            foundError = true;
#endif
      }
      if ( ! foundError ) {
         fprintf( stderr, "Wrong... no error was caught!\n" );
         vdscFiniErrorHandler( &errorHandler );
         vdscFiniErrorDefs();
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else {
      /*
       * A child - we only get out of this loop when an error is 
       * encountered (or if the timer expires)
       */
      vdscBeginTimer( &timer );
      maxTime *= US_PER_SEC;

      mypid = getpid();
      errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( childExpectedToPass, NULL, ; );
      data = (struct localData*) ptr;
   
      for (;;) {
         if ( (loop%failureRate) != 0 )
            vdscAcquireProcessLock( &data->lock, mypid );
         
         sprintf( data->dum2, "dumStr2 %d  ", mypid );
         memcpy( data->dum1, data->dum2, 100 );
            
         sscanf( data->dum1, "%s %d", dum3, &dumId );

         if ( dumId != mypid ) {

            vdscEndTimer( &timer );
            vdscCalculateTimer( &timer, &sec, &nanoSec );

            fprintf( stderr, "%s %d) - time = %u.%03u secs, \n",
                     "Ok! We got our expected error (pid =",
                     mypid,
                     (unsigned int)sec,
                     (unsigned int)(nanoSec/1000/1000) );
            data->exitFlag = 1;
            if ( (loop%failureRate) != 0 )
               vdscReleaseProcessLock( &data->lock );

            break;
         }
         
         if ( (loop%failureRate) != 0 )
            vdscReleaseProcessLock( &data->lock );

         if ( data->exitFlag == 1 )
            break;
   
         loop++;

         if ( (loop%CHECK_TIMER) != 0 ) {
            vdscEndTimer( &timer );
            vdscCalculateTimer( &timer, &sec, &nanoSec );
         
            elapsedTime = sec*US_PER_SEC + nanoSec/1000;
            if ( elapsedTime > maxTime ) {
               vdscFiniErrorHandler( &errorHandler );
               vdscFiniErrorDefs();
               return 1;
            }
         }
      } /* For loop */
   } /* parent or child */

   unlink( filename );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

