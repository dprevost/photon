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
#include "ProcessLock.h"
#include "PrintError.h"
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
#define CHECK_TIMER 1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   pid_t child_pid[NUM_CHILDREN], pid, mypid;
   unsigned long sec, nanoSec;
   vdscTimer timer;
   int childStatus;
   bool foundError = false;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 1;
   void* ptr = NULL;   
   char filename[PATH_MAX], str[10];
   struct localData *data = NULL;
   int errcode;
   vdscMemoryFile memFile;
   vdscErrorHandler errorHandler;
   
   char dum3[100];
   int dumId, i;

   vdscInitErrorDefs();
   for ( i = 0; i < NUM_CHILDREN; ++i )
      child_pid[i] = -1;
   
   if ( argc < 3 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   maxTime = strtol( argv[1], NULL, 0 );
   maxTime *= US_PER_SEC;

   vdscInitTimer( &timer );   

   strcpy( filename, argv[2] );

   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &memFile, 10, filename );
   
   if ( argc == 3 )
   {
      /*
       * This is the parent!
       */
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
   
      for ( i = 0; i < NUM_CHILDREN; ++i )
      {
         sprintf( str, "%d", i );
#if defined (WIN32)
         pid = _spawnl( _P_NOWAIT, argv[0], argv[0], argv[1], argv[2], str, NULL );
         if ( pid < 0 )
         {
            fprintf( stderr, "_spawnl failure, errno = %d\n", errno );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
         child_pid[i] = pid;
#else
         pid = fork();
         if ( pid == 0 )
         {
            execl( argv[0], argv[0], argv[1], argv[2], str, NULL );
            /* If we come here, something is wrong ! */
            ERROR_EXIT( childExpectedToPass, NULL, ; );
         }
         else if ( pid > 0 )
         {
            child_pid[i] = pid;
            fprintf( stderr, "Launched child, pid = %d\n", pid );
         }
         else
         {
            fprintf( stderr, "Fork failure, errno = %d\n", errno );
            ERROR_EXIT( expectedToPass, NULL, ; );
         }
#endif
      }
      
      /* We now wait for the children to exit */
      for ( i = 0; i < NUM_CHILDREN; ++i )
      {
#if defined(WIN32)
         _cwait( &childStatus, child_pid[i], _WAIT_CHILD );
         if ( childStatus == 0 )
            foundError = true;
#else
         waitpid( child_pid[i], &childStatus, 0 );
         if ( WEXITSTATUS(childStatus) == 0 )
            foundError = true;
#endif
      }
      if ( ! foundError )
      {
         fprintf( stderr, "Wrong... no error was caught!\n" );
         vdscFiniErrorHandler( &errorHandler );
         vdscFiniErrorDefs();
          ERROR_EXIT( expectedToPass, NULL, ; );
      }
   }
   else
   {
      /*
       * A child - we only get out of this loop when an error is 
       * encountered (or if the timer expires)
       */
      vdscBeginTimer( &timer );

      mypid = getpid();
      errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
      if ( errcode < 0 )
         ERROR_EXIT( childExpectedToPass, NULL, ; );
      data = (struct localData*) ptr;
   
      for (;;)
      {            
         if ( (loop%FAILURE_RATE) != 0 )
            vdscAcquireProcessLock( &data->lock, mypid );
         
         sprintf( data->dum2, "dumStr2 %d  ", mypid );
         memcpy( data->dum1, data->dum2, 100 );
            
         sscanf( data->dum1, "%s %d", dum3, &dumId );

         if ( (loop%FAILURE_RATE) != 0 )
            vdscReleaseProcessLock( &data->lock );

         if ( dumId != mypid || data->exitFlag == 1)
         {
            if ( dumId != mypid )
               fprintf( stderr, "Ok! We got our expected error (pid = %d)\n",
                        mypid );
            data->exitFlag = 1;
            break;
         }
         
         if ( (loop%CHECK_TIMER) != 0 )
         {
            vdscEndTimer( &timer );
            vdscCalculateTimer( &timer, &sec, &nanoSec );
         
            elapsedTime = sec*US_PER_SEC + nanoSec/1000;
            if ( elapsedTime > maxTime )
            {
               vdscFiniErrorHandler( &errorHandler );
               vdscFiniErrorDefs();
               return 1;
            }
         }
         loop++;
      } /* For loop */
   } /* parent or child */

   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

