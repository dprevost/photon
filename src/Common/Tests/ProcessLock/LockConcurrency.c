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
#include "Common/ProcessLock.h"
#include "Common/ErrorHandler.h"
#include "Common/Options.h"
#if defined(WIN32)
#  include <Process.h>
#else
#  include <sys/wait.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

#define DEFAULT_NUM_CHILDREN   4
#define DEFAULT_TIME          30

#define CHECK_TIMER 1345 /* Check the time every CHECK_TIMER loops */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   psocProcessLock lock;
   int exitFlag;
   char dum1[150];
   char dum2[250];
};

char * argv_0;
psocErrorHandler errorHandler;
psocMemoryFile memFile;
char filename[PATH_MAX];

void * ptr = NULL;   
struct localData *data = NULL;
   
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   
   strcpy( filename, "Memfile.mem" );
   
   psocInitMemoryFile( &memFile, 10, filename );
   
   ok = psocCreateBackstore( &memFile, 0644, &errorHandler );
   assert( ok );
      
   ok = psocOpenMemFile( &memFile, &ptr, &errorHandler );
   assert( ok );
      
   memset( ptr, 0, 10000 );
   data = (struct localData*) ptr;
   
   ok = psocInitProcessLock( &data->lock );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psocFiniProcessLock( &data->lock );
   
   psocCloseMemFile( &memFile, &errorHandler );
   psocFiniMemoryFile( &memFile );
   
   unlink( filename );

   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_std( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   pid_t pid, *childPid = NULL;
   int  numChilds, i, childStatus;
   bool foundError = false;
   char * processName;

   processName = malloc( strlen(argv_0) + 5 );
   assert_false( processName == NULL );
   strcpy( processName, argv_0 );
   strcat( processName, "Std" );
   
   numChilds = DEFAULT_NUM_CHILDREN;
   childPid = malloc( numChilds*sizeof(pid_t) );
   assert_false( childPid == NULL );
      
   /* Launch the childs */
   for ( i = 0; i < numChilds; ++i ) {
#if defined (WIN32)
      pid = _spawnl( _P_NOWAIT, processName, processName, NULL ); 
      assert_true( pid > 0 );
      childPid[i] = pid;
#else
      pid = fork();
      if ( pid == 0 ) {
         execl( processName, processName, NULL );
         /* If we come here, something is wrong ! */
         fail();
      }
      else if ( pid > 0 ) {
         childPid[i] = pid;
         fprintf( stderr, "Launched child, pid = %d\n", pid );
      }
      else {
         fprintf( stderr, "Fork failure, errno = %d\n", errno );
         fail();
      }
#endif
   } /* for loop launching child processes */
      
   /* Now wait for the child processes to end */
   for ( i = 0; i < numChilds; ++i ) {
#if defined(WIN32)
      _cwait( &childStatus, childPid[i], _WAIT_CHILD );
      if ( childStatus != 0 ) foundError = true;
#else
      waitpid( childPid[i], &childStatus, 0 );
      if ( WEXITSTATUS(childStatus) != 0 ) foundError = true;
#endif
   }
   assert_false( foundError );

   free( processName );
   free( childPid );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_try( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   pid_t pid, *childPid = NULL;
   int  numChilds, i, childStatus;
   bool foundError = false;
   char * processName;

   processName = malloc( strlen(argv_0) + 5 );
   assert_false( processName == NULL );
   strcpy( processName, argv_0 );
   strcat( processName, "Try" );
   
   numChilds = DEFAULT_NUM_CHILDREN;
   childPid = malloc( numChilds*sizeof(pid_t) );
   assert_false( childPid == NULL );
      
   /* Launch the childs */
   for ( i = 0; i < numChilds; ++i ) {
#if defined (WIN32)
      pid = _spawnl( _P_NOWAIT, processName, processName, NULL ); 
      assert_true( pid > 0 );
      childPid[i] = pid;
#else
      pid = fork();
      if ( pid == 0 ) {
         execl( processName, processName, NULL );
         /* If we come here, something is wrong ! */
         fail();
      }
      else if ( pid > 0 ) {
         childPid[i] = pid;
         fprintf( stderr, "Launched child, pid = %d\n", pid );
      }
      else {
         fprintf( stderr, "Fork failure, errno = %d\n", errno );
         fail();
      }
#endif
   } /* for loop launching child processes */
      
   /* Now wait for the child processes to end */
   for ( i = 0; i < numChilds; ++i ) {
#if defined(WIN32)
      _cwait( &childStatus, childPid[i], _WAIT_CHILD );
      if ( childStatus != 0 ) foundError = true;
#else
      waitpid( childPid[i], &childStatus, 0 );
      if ( WEXITSTATUS(childStatus) != 0 ) foundError = true;
#endif
   }
   assert_false( foundError );

   free( processName );
   free( childPid );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_std, setup_test, teardown_test ),
      unit_test_setup_teardown( test_try, setup_test, teardown_test )
   };

   argv_0 = argv[0];
   
   rc = run_tests(tests);
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

