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
#include "Tests/PrintError.h"
#include "Common/Options.h"
#if defined(WIN32)
#  include <Process.h>
#else
#  include <sys/wait.h>
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

const bool expectedToPass = true;

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

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   pid_t pid;
   unsigned long sec, nanoSec;
   psocTimer timer;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 0;
   void* ptr = NULL;   
   char filename[PATH_MAX];
   struct localData *data = NULL;
   bool ok;
   psocMemoryFile memFile;
   psocErrorHandler errorHandler;
   int numChilds, i, childStatus;
   char dum3[100];
   int dumId;

   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   psocInitTimer( &timer );

   numChilds = DEFAULT_NUM_CHILDREN;
   
   maxTime = DEFAULT_TIME; /* in seconds */
   
   strcpy( filename, "Memfile.mem" );
   
   psocInitMemoryFile( &memFile, 10, filename );
   
   maxTime *= US_PER_SEC;

   ok = psocOpenMemFile( &memFile, &ptr, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
      
   data = (struct localData*) ptr;
   
   psocBeginTimer( &timer );

   pid = getpid();
   
   for (;;) {      
      psocAcquireProcessLock( &data->lock, pid );
         
      sprintf( data->dum2, "dumStr2 %d  ", pid );
      memcpy( data->dum1, data->dum2, 100 );

      sscanf( data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != pid ) {
         fprintf( stderr, "Wrong... %d %d %s-%s\n", pid, 
                  dumId, data->dum1, data->dum2 );
         data->exitFlag = 1;
         psocReleaseProcessLock( &data->lock );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      
      psocReleaseProcessLock( &data->lock );
      
      if ( data->exitFlag == 1 ) break;

      loop++;

      if ( (loop % CHECK_TIMER ) == 0 ) {
         psocEndTimer( &timer );
         psocCalculateTimer( &timer, &sec, &nanoSec );

         elapsedTime = sec*US_PER_SEC + nanoSec/1000;
         if ( elapsedTime > maxTime ) break;
      }
   }
   
   printf( "Program #%d Number of loops = %lu\n", pid, loop );
   
   psocFiniErrorHandler( &errorHandler );
   psocFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

