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
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
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

#define DEFAULT_NUM_THREADS    4
#define DEFAULT_TIME          30
#define CHECK_TIMER         1345 /* Check the time every CHECK_TIMER loops */
#define US_PER_SEC       1000000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct localData
{
   pscThreadLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

bool              g_tryMode = false;
pscMemoryFile    g_memFile;
struct localData *g_data = NULL;
unsigned long     g_maxTime = 0;
vdstBarrier       g_barrier;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int worker( void* arg )
{
   unsigned long sec, nanoSec;
   pscTimer timer;   
   int identifier;
   unsigned long elapsedTime = 0;
   unsigned long loop = 0;
   char dum3[100];
   int dumId;
   bool ok;
   
   identifier = *((int*)arg);

   pscInitTimer( &timer );
   vdstBarrierWait( &g_barrier );
   pscBeginTimer( &timer );
   
   while ( 1 ) {      
      if ( g_tryMode ) {
         ok = pscTryAcquireThreadLock( &g_data->lock, 10000 );
         if ( ok != true ) continue;
      }
      else {
         pscAcquireThreadLock( &g_data->lock );
      }
      
      sprintf( g_data->dum2, "dumStr2 %d  ", identifier );
      memcpy( g_data->dum1, g_data->dum2, 100 );

      sscanf( g_data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier ) {
         fprintf( stderr, "Wrong... %d %d %s-%s\n", identifier, 
                  dumId, g_data->dum1, g_data->dum2 );
         g_data->exitFlag = 1;
         pscReleaseThreadLock( &g_data->lock );
         return 1;
      }
      
      pscReleaseThreadLock( &g_data->lock );
      
      if ( g_data->exitFlag == 1 ) break;

      loop++;
      
      if ( (loop % CHECK_TIMER ) == 0 ) {
         pscEndTimer( &timer );
         pscCalculateTimer( &timer, &sec, &nanoSec );

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
   pscErrorHandler errorHandler;
   int i, *identifier;
   vdstThreadWrap *threadWrap;
   int numThreads = 0;
   
   pscOptionHandle handle;
   char *argument;
   struct pscOptStruct opts[4] = { 
      { 'f', "filename",   1, "memoryFile", "Filename for shared memory" },
      { 'm', "mode",       1, "lockMode",   "Set this to 'try' for testing TryAcquire" },
      { 'n', "numThreads", 1, "numThreads", "Number of threads" },
      { 't', "time",       1, "timeInSecs", "Time to run the tests" }
   };

   pscInitErrorDefs();
   pscInitErrorHandler( &errorHandler );

   ok = pscSetSupportedOptions( 4, opts, &handle );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = pscValidateUserOptions( handle, argc, argv, 1 );
   if ( errcode < 0 ) {
      pscShowUsage( handle, "LockConcurrency", "" );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( errcode > 0 ) {
      pscShowUsage( handle, "LockConcurrency", "" );
      return 0;
   }

   if ( pscGetShortOptArgument( handle, 'n', &argument ) ) {
      numThreads = atoi( argument );
      if ( numThreads < 2 ) {
         fprintf( stderr, "Number of childs must be >= to two\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else {
      numThreads = DEFAULT_NUM_THREADS;
   }
   
   if ( pscGetShortOptArgument( handle, 't', &argument ) ) {
      g_maxTime = strtol( argument, NULL, 0 );
      if ( g_maxTime < 1 ) {
         fprintf( stderr, "Time of test must be positive\n" );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }      
   }
   else {
      g_maxTime = DEFAULT_TIME; /* in seconds */
   }
   
   if ( pscGetShortOptArgument( handle, 'm', &argument ) ) {
      if ( strcmp( argument, "try" ) == 0 ) g_tryMode = true;
   }
   
   if ( pscGetShortOptArgument( handle, 'f', &argument ) ) {
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
   threadWrap = (vdstThreadWrap*) malloc(numThreads*sizeof(vdstThreadWrap));
   if ( threadWrap == NULL ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   memset( threadWrap, 0, numThreads*sizeof(vdstThreadWrap) );
      
   errcode = vdstInitBarrier( &g_barrier, numThreads, &errorHandler );
   if ( errcode < 0 ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   pscInitMemoryFile( &g_memFile, 10, filename );

   ok = pscCreateBackstore( &g_memFile, 0644, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   ok = pscOpenMemFile( &g_memFile, &ptr, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   memset( ptr, 0, 10000 );
   g_data = (struct localData*) ptr;
   
   ok = pscInitThreadLock( &g_data->lock );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   for ( i = 0; i < numThreads; ++i ) {
      identifier[i] = i+1;
      errcode = vdstCreateThread( &threadWrap[i], 
                                  &worker,
                                  (void*)&identifier[i],
                                  &errorHandler );
      if ( errcode < 0 ) {
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      }
   }

   for ( i = 0; i < numThreads; ++i ) {
      errcode = vdstJoinThread( &threadWrap[i], &errorHandler );
      if ( errcode < 0 ) {
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      }
      if ( threadWrap[i].returnCode != 0 ) {
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      }
   }
   
   pscFiniMemoryFile( &g_memFile );
   vdstFiniBarrier( &g_barrier );
   pscFiniErrorHandler( &errorHandler );
   pscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

