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
#include "ProcessLock.h"
#include "PrintError.h"
#include <sys/wait.h>

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#define US_PER_SEC     1000000

struct localData
{
   vdscProcessLock lock;
   int counter;
   int overflow;
   char dum1[150];
   char dum2[250];
};

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

int main( int argc, char* argv[] )
{
   pid_t pid;
   pid_t child_pid[NUM_CHILDREN];
   unsigned long sec, nanoSec;
   vdscTimer timer;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 1;
   void* ptr = NULL;   
   char filename[PATH_MAX];
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
      ERROR_EXIT( 1, NULL, );
   
   maxTime = strtol( argv[1], NULL, 0 );
   maxTime *= US_PER_SEC;

   vdscInitTimer( &timer );   

   strcpy( filename, argv[2] );

   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &memFile, 10, filename );
   
   errcode = vdscCreateBackstore( &memFile, 0644, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( 1, &errorHandler, );

   errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( 1, &errorHandler, );

   memset( ptr, 0, 10000 );
   data = (struct localData*) ptr;
   
   errcode = vdscInitProcessLock( &data->lock );
   if ( errcode < 0 )
      ERROR_EXIT( 1, NULL, );

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
         // A child - we only get out of this loop when an error is 
         // encountered (or if the timer expires and our parent kill us)

         pid_t mypid = getpid();
         vdscInitMemoryFile( &memFile, 10, filename );
         errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
         if ( errcode < 0 )
            ERROR_EXIT( 1, NULL, );
         data = (struct localData*) ptr;
   
         while ( 1 )
         {            
            if ( (loop%FAILURE_RATE) != 0 )
               vdscAcquireProcessLock( &data->lock, mypid );
            
            if ( mypid == 0 )
            {
               fprintf( stderr, "Wrong2... pid is zero\n" );
               ERROR_EXIT( 1, NULL, );
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
               vdscReleaseProcessLock( &data->lock );
            
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
         ERROR_EXIT( 1, NULL, );
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
