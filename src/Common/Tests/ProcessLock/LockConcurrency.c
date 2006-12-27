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
#include "ProcessLock.h"
#include "ErrorHandler.h"
#include "PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#define US_PER_SEC     1000000

struct localData
{
   vdscProcessLock lock;
   int counter;
   int overflow;
   char dum1[150];
   char dum2[250];
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   pid_t pid;
   unsigned long sec, nanoSec;
   vdscTimer timer;
   bool tryMode = false;
   
   unsigned long elapsedTime = 0, maxTime = 0;
   unsigned long loop = 0;
   void* ptr = NULL;   
   char filename[PATH_MAX];
   struct localData *data = NULL;
   int errcode;
   vdscMemoryFile memFile;
   vdscErrorHandler errorHandler;
   int identifier;
   char dum3[100];
   int dumId;

   if ( argc < 5 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   vdscInitErrorDefs();

   identifier = atoi( argv[1] );
   maxTime = strtol( argv[2], NULL, 0 );
   maxTime *= US_PER_SEC;
  
   if ( strcmp( argv[4], "try" ) == 0 )
      tryMode = true;
   
   vdscInitTimer( &timer );   

   vdscInitErrorHandler( &errorHandler );
   
   strcpy( filename, argv[3] );

   vdscInitMemoryFile( &memFile, 10, filename );
   
   if ( identifier == 0 )
   {
      errcode = vdscCreateBackstore( &memFile, 0644, &errorHandler );

      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, &errorHandler, ; );
   }
   errcode = vdscOpenMemFile( &memFile, &ptr, &errorHandler );
   if ( errcode < 0 )
      ERROR_EXIT( expectedToPass, &errorHandler, ; );

   if ( identifier == 0 )
      memset( ptr, 0, 10000 );
   data = (struct localData*) ptr;
   fprintf( stderr, "id = %d, data = %p\n", identifier, data );
   
   if ( identifier == 0 )
   {
      errcode = vdscInitProcessLock( &data->lock );
      if ( errcode < 0 )
         ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdscBeginTimer( &timer );

   pid = getpid();
   
   for (;;)
   {      
      pid_t savepid = pid;

      if ( tryMode )
      {
         int err = vdscTryAcquireProcessLock( &data->lock, pid, 10000 );
         if ( err != 0 )
            continue;
      }
      else
         vdscAcquireProcessLock( &data->lock, pid );

      if ( pid != savepid || pid == 0 )
      {
         fprintf( stderr, "Wrong2... %d %d\n", pid, savepid );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      sprintf( data->dum2, "dumStr2 %d  ", identifier+1 );
      memcpy( data->dum1, data->dum2, 100 );

      sscanf( data->dum1, "%s %d", dum3, &dumId );
      if ( dumId != identifier+1 )
      {
         fprintf( stderr, "Wrong... %d %d %s-%s\n", identifier+1, 
                  dumId, data->dum1, data->dum2 );
         ERROR_EXIT( expectedToPass, NULL, ; );
      }
      
      vdscReleaseProcessLock( &data->lock );
      
      loop++;
      
      vdscEndTimer( &timer );
      vdscCalculateTimer( &timer, &sec, &nanoSec );

      elapsedTime = sec*US_PER_SEC + nanoSec/1000;
      if ( elapsedTime > maxTime )
         break;
   }
   
//   if ( identifier == 0 )
//      sleep( 10 );
   
   if ( identifier == 0 )
      printf( "\n" );
   printf( "Program #%d Number of loops = %lu\n", identifier, loop );
   
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

