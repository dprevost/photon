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

#define DEFAULT_NUM_THREADS    4
#define DEFAULT_TIME          30
#define CHECK_TIMER         1345 /* Check the time every CHECK_TIMER loops */
#define US_PER_SEC       1000000

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct localData
{
   vdscThreadLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

bool g_tryMode = false;
vdscMemoryFile g_memFile;
struct localData *g_data = NULL;
unsigned long g_maxTime = 0;
vdstBarrier g_barrier;

#define TEST_MAX_THREADS 4
#define MAX_MSG 100

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void * worker( void* arg )
{
   pid_t pid;
   unsigned long sec, nanoSec;
   vdscTimer timer;
   
   vdstThreadWrap* pThread = (vdstThreadWrap*) arg;
   int identifier;
   unsigned long elapsedTime = 0;
   unsigned long loop = 0;
   int errcode;
   
   identifier = *((int*)pThread->arg);

   vdscInitTimer( &timer );
   
   vdstBarrierWait( &g_barrier );
   
   vdscBeginTimer( &timer );

   char dum3[100];
   int dumId;
  
   while ( 1 )
   {      
      if ( g_tryMode )
      {
         errcode = vdscTryAcquireThreadLock( &g_data->lock, 10000 );
         if ( errcode != 0 )
            continue;
      }
      else
         vdscAcquireThreadLock( &g_data->lock );

      sprintf( g_data->dum2, "dumStr2 %d  ", identifier );
      memcpy( g_data->dum1, g_data->dum2, 100 );

      sscanf( g_data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier )
      {
         fprintf( stderr, "Wrong... %d %d %s-%s\n", identifier, 
                  dumId, g_data->dum1, g_data->dum2 );
         g_data->exitFlag = 1;
         vdscReleaseThreadLock( &g_data->lock );
         pThread->returnCode = 1;
         return;
      }
      
      vdscReleaseThreadLock( &g_data->lock );
      
      if ( g_data->exitFlag == 1 )
         break;

      loop++;
      
      if ( (loop % CHECK_TIMER ) == 0 )
      {
         vdscEndTimer( &timer );
         vdscCalculateTimer( &timer, &sec, &nanoSec );

         elapsedTime = sec*US_PER_SEC + nanoSec/1000;
         if ( elapsedTime > g_maxTime )
            break;
      }
   }
   printf( "Thread #%d, Number of loops = %lu\n", identifier, loop );
   pThread->returnCode = 0;
   
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   void* ptr = NULL;   
   char filename[PATH_MAX];
   int errcode;
   vdscErrorHandler errorHandler;
   int i, *identifier;
   vdstThreadWrap *threadWrap;
   char msg[MAX_MSG] = "";
   int numThreads = 0;
   
   vdscOptionHandle handle;
   char *argument;
   struct vdscOptStruct opts[4] = 
      { 
         'f', "filename",   1, "memoryFile", "Filename for shared memory",
         'm', "mode",       1, "lockMode",   "Set this to 'try' for testing TryAcquire",
         'n', "numThreads", 1, "numThreads", "Number of threads",
         't', "time",       1, "timeInSecs", "Time to run the tests"
      };

   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );

   errcode = vdscSetSupportedOptions( 4, opts, &handle );
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
      
   errcode = vdstInitBarrier( &g_barrier, TEST_MAX_THREADS, &errorHandler );
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
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   
   vdscFiniMemoryFile( &g_memFile );
   vdstFiniBarrier( &g_barrier );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

