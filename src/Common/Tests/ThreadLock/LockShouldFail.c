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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

/*
 * This test is very similar to the LockConcurrency test... except that
 * this program does not always lock properly (on purpose)!!! The rate of 
 * failure of our locks is controlled by the #define FAILURE_RATE.
 *
 * In a way, it insures that we would be able to detect a failure in
 * our locking mechanism using the standard LockConcurrency test. 
 *
 * The amount of time we will try to observe a concurrency failure is
 * determined by the first argument to the program.
 */
 
// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common.h"
#include "MemoryFile.h"
#include "Timer.h"
#include "ThreadLock.h"
#include "ErrorHandler.h"
#include "ThreadWrap.h"
#include "Barrier.h"

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#define US_PER_SEC     1000000

struct localData
{
   vdscThreadLock lock;
   int counter;
   int overflow;
   char dum1[150];
   char dum2[250];
};

//vdscBool g_tryMode = eFalse;
vdscMemoryFile g_memFile;
struct localData *g_data = NULL;
unsigned long g_maxTime = 0;
vdstBarrier g_barrier;

#define TEST_MAX_THREADS 4
#define MAX_MSG 100

/*
 * Misnamed define... the failure rate is 1/FAILURE_RATE...
 *
 * I would prefer a much lower failure rate but it would mean
 * being very patient... (failures can only be seen when context
 * switches are done and with time slices of 1/1000 sec. (on many
 * modern OSes), it would force us to run the test for a lot longer).
 *
 * [FAILURE_RATE 500 --> 0.2% failure]
 */
#define FAILURE_RATE 500
//#define FAILURE_RATE 1000 000 000
#define NUM_CHILDREN 4

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

void * worker( void* pIdentifier )
{
   pid_t pid;
   unsigned long sec, nanoSec;
   vdscTimer timer;
   
   int identifier = *((int*)pIdentifier);
   unsigned long elapsedTime = 0;
   unsigned long loop = 0;
   int errcode;
   int THIS_ASSERT_IS_OK = 0;
   
   vdscInitTimer( &timer );   

   vdstBarrierWait( &g_barrier );
   
   vdscBeginTimer( &timer );

   char dum3[100];
   int dumId;
  
   while ( 1 )
   {      
      if ( (loop%FAILURE_RATE) != 0 )
         vdscAcquireThreadLock( &g_data->lock );
  
      sprintf( g_data->dum2, "dumStr2 %d  ", identifier );
      memcpy( g_data->dum1, g_data->dum2, 100 );

      sscanf( g_data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier )
      {
         fprintf( stderr, "Ok! We got our expected error !\n" );
         vdscReleaseThreadLock( &g_data->lock );
         assert( THIS_ASSERT_IS_OK == 1 ); /* Crash it! */
      }
      
      if ( (loop%FAILURE_RATE) != 0 )
         vdscReleaseThreadLock( &g_data->lock );
      
      loop++;
      
      vdscEndTimer( &timer );
      vdscCalculateTimer( &timer, &sec, &nanoSec );

      elapsedTime = sec*US_PER_SEC + nanoSec/1000;
      if ( elapsedTime > g_maxTime )
         break;
   }
   
   printf( "Thread #%d, Number of loops = %lu\n", identifier, loop );

   return;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

int main( int argc, char* argv[] )
{
   void* ptr = NULL;   
   char filename[MAXPATHLEN];
   int errcode;
   vdscErrorHandler errorHandler;
   int i, identifier[TEST_MAX_THREADS];
   vdstThreadWrap threadWrap[TEST_MAX_THREADS];
   char msg[MAX_MSG] = "";
   
   if ( argc < 3 )
      return -1;

   vdscInitErrorDefs();

   g_maxTime = strtol( argv[1], NULL, 0 );
   g_maxTime *= US_PER_SEC;
  
   vdscInitErrorHandler( &errorHandler );

   errcode = vdstInitBarrier( &g_barrier, TEST_MAX_THREADS, &errorHandler );
   if ( errcode < 0 )
   {
      vdscGetErrorMsg( &errorHandler, msg, MAX_MSG );
      fprintf( stderr, "vdstInitBarrier error: %s\n", msg );
      return -1;
   }
   
   strcpy( filename, argv[2] );
   vdscInitMemoryFile( &g_memFile, 10, filename );

   errcode = vdscCreateBackstore( &g_memFile, 0644, &errorHandler );
   if ( errcode < 0 )
   {
      vdscGetErrorMsg( &errorHandler, msg, MAX_MSG );
      fprintf( stderr, "vdscCreateBackstore error: %s\n", msg );
      return -1;
   }

   errcode = vdscOpenMemFile( &g_memFile, &ptr, &errorHandler );
   if ( errcode < 0 )
   {
      vdscGetErrorMsg( &errorHandler, msg, MAX_MSG );
      fprintf( stderr, "vdscOpenMemFile error: %s\n", msg );
      return -1;
   }
   memset( ptr, 0, 10000 );
   g_data = (struct localData*) ptr;
   
   errcode = vdscInitThreadLock( &g_data->lock );
   if ( errcode < 0 )
   {
      fprintf( stderr, " lock problem = %d\n", errno );
      return -1;
   }
   
   for ( i = 0; i < TEST_MAX_THREADS; ++i )
   {
      identifier[i] = i+1;
      errcode = vdstCreateThread( &threadWrap[i], 
                                  &worker,
                                  (void*)&identifier[i],
                                  &errorHandler );
      if ( errcode < 0 )
      {
         vdscGetErrorMsg( &errorHandler, msg, MAX_MSG );
         fprintf( stderr, "vdstCreateThread error: %s\n", msg );
         return -1;
      }
   }

   for ( i = 0; i < TEST_MAX_THREADS; ++i )
   {
      errcode = vdstJoinThread( &threadWrap[i], NULL, &errorHandler );
      if ( errcode < 0 )
      {
         vdscGetErrorMsg( &errorHandler, msg, MAX_MSG );
         fprintf( stderr, "vdstJoinThread error: %s\n", msg );
         return -1;
      }
   }
   
   vdscFiniMemoryFile( &g_memFile );
   vdstFiniBarrier( &g_barrier );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

int main2( int argc, char* argv[] )
{
   pid_t pid;
   pid_t child_pid[NUM_CHILDREN];
   unsigned long sec, nanoSec;
   vdscTimer timer;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 1;
   void* ptr = NULL;   
   char filename[MAXPATHLEN];
   struct localData *data = NULL;
   int errcode;
   vdscMemoryFile memFile;
   vdscErrorHandler errorHandler;
   
   char dum3[100];
   int dumId, i;

   vdscInitErrorDefs();
   pid = getpid();
   for ( i = 0; i < NUM_CHILDREN; ++i )
      child_pid[i] = -1;
   
   if ( argc < 3 )
      return -1;
   
   maxTime = strtol( argv[1], NULL, 0 );
   maxTime *= US_PER_SEC;

   vdscInitTimer( &timer );   

   strcpy( filename, argv[2] );

   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &memFile, 10, filename );
   
   errcode = vdscCreateBackstore( &memFile, 0644, &errorHandler );
   if ( errcode < 0 )
   {
      fprintf( stderr, " mmap problem = %d\n", errno );
      return -1;
   }

   errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
   if ( errcode < 0 )
   {
      fprintf( stderr, " mmap problem = %d\n", errno );
      return -1;
   }
   memset( ptr, 0, 10000 );
   data = (struct localData*) ptr;
   
   errcode = vdscInitThreadLock( &data->lock );
   if ( errcode < 0 )
   {
      fprintf( stderr, " lock problem = %d\n", errno );
      return -1;
   }

   vdscSyncMemFile( &memFile, &errorHandler );
   
   vdscCloseMemFile( &memFile, &errorHandler );
   data = NULL;
   ptr = NULL;
   
   vdscBeginTimer( &timer );

   for ( i = 0; i < NUM_CHILDREN; ++i )
   {
      pid = fork();
      if ( pid == 0 )
      {
         // A child - we only get out of this if to exit

         pid_t mypid = getpid();
         vdscInitMemoryFile( &memFile, 10, filename );
         errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
         if ( errcode < 0 )
         {
            fprintf( stderr, " mmap problem = (pid = %d)%d\n", pid, errno );
            return -1;
         }
         data = (struct localData*) ptr;
   
         while ( 1 )
         {            
            if ( (loop%FAILURE_RATE) != 0 )
               vdscAcquireThreadLock( &data->lock );
            
            if ( mypid == 0 )
            {
               fprintf( stderr, "Wrong2... pid is zero\n" );
               return 1; // Error!!!
            }
            sprintf( data->dum2, "dumStr2 %d  ", mypid );
            memcpy( data->dum1, data->dum2, 100 );
            
            sscanf( data->dum1, "%s %d", dum3, &dumId );
            if ( dumId != mypid )
            {
               fprintf( stderr, "Ok! We got our expected error (pid = %d)\n",
                        mypid );
               return 0;
            }

            if ( (loop%FAILURE_RATE) != 0 )
               vdscReleaseThreadLock( &data->lock );
            
            loop++;
         }
      }
      else if ( pid > 0 )
      {
         child_pid[i] = pid;
         
         fprintf( stderr, "Lauch child, pid = %d\n", pid );
      }
      else
      {
         fprintf( stderr, "Fork failure, errno = %d\n", errno );
         return 1; // Error!!!
      }
   }

   /* Warning: the logic of the return is reversed. This program
    * should end in error but if no error is caught after the time
    * limit... we return zero - which should indicate to the caller
    * that something went wrong.
    */
   while ( 1 )
   {
      sleep( 1 );
      vdscEndTimer( &timer );
      vdscCalculateTimer( &timer, &sec, &nanoSec );
         
      elapsedTime = sec*US_PER_SEC + nanoSec/1000;

      if ( elapsedTime > maxTime )
      {
         fprintf( stderr, "Wrong... no error was caught!\n" );
         for ( i = 0; i < NUM_CHILDREN; ++i )
            kill( child_pid[i], SIGTERM );

         vdscFiniErrorHandler( &errorHandler );
         vdscFiniErrorDefs();

         return 0;
      }
      int num = waitpid( -1, NULL, WNOHANG );
      if ( num != 0 )
         break;
   }

   fprintf( stderr, "Time: %d us \n", elapsedTime );
   
   for ( i = 0; i < NUM_CHILDREN; ++i )
      kill( child_pid[i], SIGTERM );
      
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 1;
}

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--
