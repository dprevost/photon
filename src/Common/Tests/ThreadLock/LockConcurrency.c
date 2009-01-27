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
   psocThreadLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

bool              g_tryMode = false;
psocMemoryFile    g_memFile;
struct localData *g_data = NULL;
unsigned long     g_maxTime = 0;
psotBarrier       g_barrier;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int worker( void* arg )
{
   unsigned long sec, nanoSec;
   psocTimer timer;   
   int identifier;
   unsigned long elapsedTime = 0;
   unsigned long loop = 0;
   char dum3[100];
   int dumId;
   bool ok;
   
   identifier = *((int*)arg);

   psocInitTimer( &timer );
   psotBarrierWait( &g_barrier );
   psocBeginTimer( &timer );
   
   while ( 1 ) {      
      if ( g_tryMode ) {
         ok = psocTryAcquireThreadLock( &g_data->lock, 10000 );
         if ( ok != true ) continue;
      }
      else {
         psocAcquireThreadLock( &g_data->lock );
      }
      
      sprintf( g_data->dum2, "dumStr2 %d  ", identifier );
      memcpy( g_data->dum1, g_data->dum2, 100 );

      sscanf( g_data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier ) {
         fprintf( stderr, "Wrong... %d %d %s-%s\n", identifier, 
                  dumId, g_data->dum1, g_data->dum2 );
         g_data->exitFlag = 1;
         psocReleaseThreadLock( &g_data->lock );
         return 1;
      }
      
      psocReleaseThreadLock( &g_data->lock );
      
      if ( g_data->exitFlag == 1 ) break;

      loop++;
      
      if ( (loop % CHECK_TIMER ) == 0 ) {
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
   int i, *identifier;
   psotThreadWrap *threadWrap;
   int numThreads = 0;
   
   psocOptionHandle handle;
   char *argument;
   struct psocOptStruct opts[4] = { 
      { 'f', "filename",   1, "memoryFile", "Filename for shared memory" },
      { 'm', "mode",       1, "lockMode",   "Set this to 'try' for testing TryAcquire" },
      { 'n', "numThreads", 1, "numThreads", "Number of threads" },
      { 't', "time",       1, "timeInSecs", "Time to run the tests" }
   };

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );

   ok = psocSetSupportedOptions( 4, opts, &handle );
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
      if ( threadWrap[i].returnCode != 0 ) {
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
      }
   }
   
   psocFiniMemoryFile( &g_memFile );
   psotFiniBarrier( &g_barrier );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

