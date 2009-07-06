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
      psocAcquireThreadLock( &g_data->lock );
      
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

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   void* ptr = NULL;   
   char filename[PATH_MAX];
   int errcode;
   bool ok;
   psocErrorHandler errorHandler;
   int i, *identifier;
   psotThreadWrap *threadWrap;
   int numThreads = 0;
   
   char *argument;

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );

   numThreads = DEFAULT_NUM_THREADS;
   g_maxTime = DEFAULT_TIME; /* in seconds */
   strcpy( filename, "Memfile.mem" );
   
   g_maxTime *= US_PER_SEC;
   identifier = (int*) malloc( numThreads*sizeof(int));
   assert_false( identifier == NULL );
   memset( identifier, 0, numThreads*sizeof(int) );
   
   threadWrap = (psotThreadWrap*) malloc(numThreads*sizeof(psotThreadWrap));
   assert_false( threadWrap == NULL );
   memset( threadWrap, 0, numThreads*sizeof(psotThreadWrap) );
      
   errcode = psotInitBarrier( &g_barrier, numThreads, &errorHandler );
   assert_true( errcode == 0 );
   
   psocInitMemoryFile( &g_memFile, 10, filename );

   ok = psocCreateBackstore( &g_memFile, 0644, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &g_memFile, &ptr, &errorHandler );
   assert_true( ok );
   
   memset( ptr, 0, 10000 );
   g_data = (struct localData*) ptr;
   
   ok = psocInitThreadLock( &g_data->lock );
   assert_true( ok );
   
   for ( i = 0; i < numThreads; ++i ) {
      identifier[i] = i+1;
      errcode = psotCreateThread( &threadWrap[i], 
                                  &worker,
                                  (void*)&identifier[i],
                                  &errorHandler );
      assert_true( errcode == 0 );
   }

   for ( i = 0; i < numThreads; ++i ) {
      errcode = psotJoinThread( &threadWrap[i], &errorHandler );
      assert_true( errcode == 0 );
      assert_true( threadWrap[i].returnCode == 0 );
   }
   
   psocFiniMemoryFile( &g_memFile );
   unlink( filename );
   psotFiniBarrier( &g_barrier );
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test1 ),
   };

   rc = run_tests(tests);
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

