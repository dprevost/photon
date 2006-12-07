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
 * failure of our locks is controlled by the #define FAILURE_RATE.
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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   vdscThreadLock lock;
   int counter;
   int overflow;
   char dum1[150];
   char dum2[250];
};

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
/* #define FAILURE_RATE 1000 000 000 */
#define NUM_CHILDREN 4

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   void* ptr = NULL;   
   char filename[PATH_MAX];
   int errcode;
   vdscErrorHandler errorHandler;
   int i, identifier[TEST_MAX_THREADS];
   vdstThreadWrap threadWrap[TEST_MAX_THREADS];
   char msg[MAX_MSG] = "";
   
   if ( argc < 3 )
      ERROR_EXIT( expectedToPass, NULL, );

   vdscInitErrorDefs();

   g_maxTime = strtol( argv[1], NULL, 0 );
   g_maxTime *= US_PER_SEC;
  
   vdscInitErrorHandler( &errorHandler );

   errcode = vdstInitBarrier( &g_barrier, TEST_MAX_THREADS, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, );
   
   strcpy( filename, argv[2] );
   vdscInitMemoryFile( &g_memFile, 10, filename );

   errcode = vdscCreateBackstore( &g_memFile, 0644, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, );

   errcode = vdscOpenMemFile( &g_memFile, &ptr, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, );

   memset( ptr, 0, 10000 );
   g_data = (struct localData*) ptr;
   
   errcode = vdscInitThreadLock( &g_data->lock );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, NULL, );
   
   for ( i = 0; i < TEST_MAX_THREADS; ++i )
   {
      identifier[i] = i+1;
      errcode = vdstCreateThread( &threadWrap[i], 
                                  &worker,
                                  (void*)&identifier[i],
                                  &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, );
   }

   for ( i = 0; i < TEST_MAX_THREADS; ++i )
   {
      errcode = vdstJoinThread( &threadWrap[i], NULL, &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, );
   }
   
   vdscFiniMemoryFile( &g_memFile );
   vdstFiniBarrier( &g_barrier );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

