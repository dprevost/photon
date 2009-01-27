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
#include "Common/ThreadLock.h"
#include "Common/ErrorHandler.h"
#include "Common/Tests/ThreadLock/ThreadWrap.h"
#include "Common/Tests/ThreadLock/Barrier.h"
#include "Tests/PrintError.h"
#include "Common/Options.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   psocThreadLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

psocMemoryFile    g_memFile;
struct localData *g_data = NULL;
unsigned long     g_maxTime = 0;
unsigned long     g_failureRate;
psotBarrier       g_barrier;
bool              g_tryMode = false;

/*
 * Misnamed define... the failure rate is 1/DEFAULT_FAILURE_RATE...
 *
 * I would prefer a much lower failure rate but it would mean
 * being very patient... (failures can only be seen when context
 * switches are done and with time slices of 1/1000 sec. (on many
 * modern OSes), it would force us to run the test for a lot longer).
 *
 * [DEFAULT_FAILURE_RATE 1000000 --> 0.0001% failure]
 */
#define DEFAULT_FAILURE_RATE 1000000
#define DEFAULT_NUM_THREADS        4
#define DEFAULT_TIME             300
#define CHECK_TIMER             1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int worker( void* arg )
{
   unsigned long sec, nanoSec;
   psocTimer timer;
   int identifier;
   unsigned long elapsedTime = 0;
   unsigned long loop = 1;
   char dum3[100];
   int dumId;
   
   identifier = *((int*)arg);

   psocInitTimer( &timer );   
   psotBarrierWait( &g_barrier );
   psocBeginTimer( &timer );
  
   while ( 1 ) {
      if ( (loop%DEFAULT_FAILURE_RATE) != 0 ) {
         psocAcquireThreadLock( &g_data->lock );
      }
      
      sprintf( g_data->dum2, "dumStr2 %d  ", identifier );
      memcpy( g_data->dum1, g_data->dum2, 100 );

      sscanf( g_data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier ) {
         psocEndTimer( &timer );
         psocCalculateTimer( &timer, &sec, &nanoSec );

         fprintf( stderr, "%s - time = %u.%03u secs, \n",
                  "Ok! We got our expected error",
                  (unsigned int)sec,
                  (unsigned int)(nanoSec/1000/1000) );
         g_data->exitFlag = 1;
         if ( (loop%DEFAULT_FAILURE_RATE) != 0 ) {
            psocReleaseThreadLock( &g_data->lock );
         }
         fprintf( stderr, "Thread #%d, Number of loops = %lu\n", 
                  identifier, loop );
         return 1;
      }
      
      if ( (loop%DEFAULT_FAILURE_RATE) != 0 ) {
         psocReleaseThreadLock( &g_data->lock );
      }

      if ( g_data->exitFlag == 1 ) break;      
      loop++;
      
      if ( (loop%CHECK_TIMER) != 0 ) {
         psocEndTimer( &timer );
         psocCalculateTimer( &timer, &sec, &nanoSec );

         elapsedTime = sec*US_PER_SEC + nanoSec/1000;
         if ( elapsedTime > g_maxTime ) break;
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
   bool ok;
   psocErrorHandler errorHandler;
   int i, *identifier, numThreads;
   psotThreadWrap *threadWrap;
   bool foundError = false;
   psocOptionHandle handle;
   char *argument;
   struct psocOptStruct opts[5] = { 
      { 'f', "filename",   1, "memoryFile",    "Filename for shared memory" },
      { 'm', "mode",       1, "lockMode",      "Set this to 'try' for testing TryAcquire" },
      { 'n', "numThreads", 1, "numThreads",    "Number of threads" },
      { 'r', "rate",       1, "rateOfFailure", "Inverse rate: 1000 means a rate of 0.1%" },
      { 't', "time",       1, "timeInSecs",    "Time to run the tests" }
   };
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );

   ok = psocSetSupportedOptions( 5, opts, &handle );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psocValidateUserOptions( handle, argc, argv, 1 );
   if ( errcode < 0 ) {
      psocShowUsage( handle, "LockConcurrency", "" );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errcode > 0 ) {
      psocShowUsage( handle, "LockConcurrency", "" );
      return 0;
   }

   if ( psocGetShortOptArgument( handle, 'n', &argument ) ) {
      numThreads = atoi( argument );
      if ( numThreads < 2 ) {
         fprintf( stderr, "Number of childs must be >= to two\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else {
      numThreads = DEFAULT_NUM_THREADS;
   }
   
   if ( psocGetShortOptArgument( handle, 'r', &argument ) ) {
      g_failureRate = strtol( argument, NULL, 0 );
      if ( g_failureRate < 1 ) {
         fprintf( stderr, "Failure rate must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else {
      g_failureRate = DEFAULT_FAILURE_RATE;
   }
   
   if ( psocGetShortOptArgument( handle, 't', &argument ) ) {
      g_maxTime = strtol( argument, NULL, 0 );
      if ( g_maxTime < 1 ) {
         fprintf( stderr, "Time of test must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else {
      g_maxTime = DEFAULT_TIME; /* in seconds */
   }
   
   if ( psocGetShortOptArgument( handle, 'm', &argument ) ) {
      if ( strcmp( argument, "try" ) == 0 ) g_tryMode = true;
   }
   
   if ( psocGetShortOptArgument( handle, 'f', &argument ) ) {
      strncpy( filename, argument, PATH_MAX );
      if ( filename[0] == '\0' ) {
         fprintf( stderr, "Empty memfile name\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else {
      strcpy( filename, "Memfile.mem" );
   }
   
   g_maxTime *= US_PER_SEC;
   identifier = (int*) malloc( numThreads*sizeof(int));
   if ( identifier == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   memset( identifier, 0, numThreads*sizeof(int) );

   threadWrap = (psotThreadWrap*) malloc(numThreads*sizeof(psotThreadWrap));
   if ( threadWrap == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   memset( threadWrap, 0, numThreads*sizeof(psotThreadWrap) );
   
   errcode = psotInitBarrier( &g_barrier, numThreads, &errorHandler );
   if ( errcode < 0 ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psocInitMemoryFile( &g_memFile, 10, filename );

   ok = psocCreateBackstore( &g_memFile, 0644, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   ok = psocOpenMemFile( &g_memFile, &ptr, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   memset( ptr, 0, 10000 );
   g_data = (struct localData*) ptr;
   
   ok = psocInitThreadLock( &g_data->lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   for ( i = 0; i < numThreads; ++i ) {
      identifier[i] = i+1;
      errcode = psotCreateThread( &threadWrap[i], 
                                  &worker,
                                  (void*)&identifier[i],
                                  &errorHandler );
      if ( errcode < 0 ) {
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      }
   }

   for ( i = 0; i < numThreads; ++i ) {
      errcode = psotJoinThread( &threadWrap[i], &errorHandler );
      if ( errcode < 0 ) {
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      }
      if ( threadWrap[i].returnCode != 0 ) foundError = true;
   }
   
   if ( ! foundError ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   psocFiniMemoryFile( &g_memFile );
   psotFiniBarrier( &g_barrier );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

