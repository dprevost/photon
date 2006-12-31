/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common.h"
#include "MemoryFile.h"
#include "Timer.h"
#include "ThreadLock.h"
#include "ErrorHandler.h"
#include "ThreadWrap.h"
#include "Barrier.h"
#include "PrintError.h"
#include "Options.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   vdscThreadLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

vdscMemoryFile    g_memFile;
struct localData *g_data = NULL;
unsigned long     g_maxTime = 0;
unsigned long     g_failureRate;
vdstBarrier       g_barrier;
bool              g_tryMode = false;

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
#define DEFAULT_NUM_THREADS    4
#define DEFAULT_TIME         300
#define CHECK_TIMER         1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int worker( void* arg )
{
   unsigned long sec, nanoSec;
   vdscTimer timer;
   int identifier;
   unsigned long elapsedTime = 0;
   unsigned long loop = 0;
   char dum3[100];
   int dumId;
   
   identifier = *((int*)arg);

   vdscInitTimer( &timer );   
   vdstBarrierWait( &g_barrier );
   vdscBeginTimer( &timer );
  
   while ( 1 )
   {      
      if ( (loop%DEFAULT_FAILURE_RATE) != 0 )
         vdscAcquireThreadLock( &g_data->lock );
  
      sprintf( g_data->dum2, "dumStr2 %d  ", identifier );
      memcpy( g_data->dum1, g_data->dum2, 100 );

      sscanf( g_data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier )
      {
         vdscEndTimer( &timer );
         vdscCalculateTimer( &timer, &sec, &nanoSec );

         fprintf( stderr, "%s - time = %d.%03d secs, \n",
                  "Ok! We got our expected error",
                  sec,
                  nanoSec/1000/1000 );
         g_data->exitFlag = 1;
         if ( (loop%DEFAULT_FAILURE_RATE) != 0 )
            vdscReleaseThreadLock( &g_data->lock );
         fprintf( stderr, "Thread #%d, Number of loops = %lu\n", 
                  identifier, loop );
         return 1;
      }
      
      if ( (loop%DEFAULT_FAILURE_RATE) != 0 )
         vdscReleaseThreadLock( &g_data->lock );

      if ( g_data->exitFlag == 1 )
         break;
      
      loop++;
      
      if ( (loop%CHECK_TIMER) != 0 )
      {
         vdscEndTimer( &timer );
         vdscCalculateTimer( &timer, &sec, &nanoSec );

         elapsedTime = sec*US_PER_SEC + nanoSec/1000;
         if ( elapsedTime > g_maxTime )
            break;
      }
   }
   
   printf( "Thread #%d, Number of loops = %lu\n", identifier, loop );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   void* ptr = NULL;   
   char filename[PATH_MAX];
   int errcode;
   vdscErrorHandler errorHandler;
   int i, *identifier, numThreads;
   vdstThreadWrap *threadWrap;
   bool foundError = false;
   vdscOptionHandle handle;
   char *argument;
   struct vdscOptStruct opts[5] = 
      { 
         'f', "filename",   1, "memoryFile",    "Filename for shared memory",
         'm', "mode",       1, "lockMode",      "Set this to 'try' for testing TryAcquire",
         'n', "numThreads", 1, "numThreads",    "Number of threads",
         'r', "rate",       1, "rateOfFailure", "Inverse rate: 1000 means a rate of 0.1%",
         't', "time",       1, "timeInSecs",    "Time to run the tests"
      };
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );

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

   if ( vdscGetShortOptArgument( handle, 'n', &argument ) )
   {
      numThreads = atoi( argument );
      if ( numThreads < 2 )
      {
         fprintf( stderr, "Number of childs must be >= to two\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      numThreads = DEFAULT_NUM_THREADS;

   if ( vdscGetShortOptArgument( handle, 'r', &argument ) )
   {
      g_failureRate = strtol( argument, NULL, 0 );
      if ( g_failureRate < 1 )
      {
         fprintf( stderr, "Failure rate must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      g_failureRate = DEFAULT_FAILURE_RATE;

   if ( vdscGetShortOptArgument( handle, 't', &argument ) )
   {
      g_maxTime = strtol( argument, NULL, 0 );
      if ( g_maxTime < 1 )
      {
         fprintf( stderr, "Time of test must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else
      g_maxTime = DEFAULT_TIME; /* in seconds */
  
   if ( vdscGetShortOptArgument( handle, 'm', &argument ) )
   {
      if ( strcmp( argument, "try" ) == 0 )
         g_tryMode = true;
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

   g_maxTime *= US_PER_SEC;
   identifier = (int*) malloc( numThreads*sizeof(int));
   if ( identifier == NULL )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   memset( identifier, 0, numThreads*sizeof(int) );
   threadWrap = (vdstThreadWrap*) malloc(numThreads*sizeof(vdstThreadWrap));
   if ( threadWrap == NULL )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   memset( threadWrap, 0, numThreads*sizeof(vdstThreadWrap) );
   
   errcode = vdstInitBarrier( &g_barrier, numThreads, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   
   vdscInitMemoryFile( &g_memFile, 10, filename );

   errcode = vdscCreateBackstore( &g_memFile, 0644, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );

   errcode = vdscOpenMemFile( &g_memFile, &ptr, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );

   memset( ptr, 0, 10000 );
   g_data = (struct localData*) ptr;
   
   errcode = vdscInitThreadLock( &g_data->lock );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   for ( i = 0; i < numThreads; ++i )
   {
      identifier[i] = i+1;
      errcode = vdstCreateThread( &threadWrap[i], 
                                  &worker,
                                  (void*)&identifier[i],
                                  &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }

   for ( i = 0; i < numThreads; ++i )
   {
      errcode = vdstJoinThread( &threadWrap[i], &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      if ( threadWrap[i].returnCode != 0 )
         foundError = true;
   }
   
   if ( ! foundError )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );

   vdscFiniMemoryFile( &g_memFile );
   vdstFiniBarrier( &g_barrier );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

