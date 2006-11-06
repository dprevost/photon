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

#include "MemoryFile.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char* argv[] )
{
   vdscMemoryFile  mem;
   vdscErrorHandler errorHandler;
   int errcode = 0, rc = 0;
   vdscMemoryFileStatus status;
   
   unlink( "MemFile.mem" );

   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &mem, 10, "MemFile.mem" );

   errcode = vdscCreateBackstore( &mem, 0644, &errorHandler );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }
   
   vdscBackStoreStatus( &mem, &status );
   if ( status.fileExist != 1 )
   {
      rc = -1;
      goto the_exit;
   }
   if ( status.fileReadable != 1 )
   {
      rc = -1;
      goto the_exit;
   }
   if ( status.fileWritable != 1 )
   {
      rc = -1;
      goto the_exit;
   }
   if ( status.lenghtOK != 1 )
   {
      rc = -1;
      goto the_exit;
   }
   if ( status.actualLLength != 10*1024 )
   {
      rc = -1;
      goto the_exit;
   }

   errcode = unlink( "MemFile.mem" );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   errcode = vdscCreateBackstore( &mem, 0600, &errorHandler );
   if ( errcode != 0 ) 
   {
      fprintf( stderr, "errno = %d\n", errno );
      rc = -1;
      goto the_exit;
   }
   
   vdscBackStoreStatus( &mem, &status );
   if ( status.fileExist != 1 )
   {
      rc = -1;
      goto the_exit;
   }
   if ( status.fileReadable != 1 )
   {
      rc = -1;
      goto the_exit;
   }
   if ( status.fileWritable != 1 )
   {
      rc = -1;
      goto the_exit;
   }

   errcode = unlink( "MemFile.mem" );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   errcode = vdscCreateBackstore( &mem, 0660, &errorHandler );
   if ( errcode != 0 ) 
   {
      fprintf( stderr, "errno = %d\n", errno );
      rc = -1;
      goto the_exit;
   }
   
   vdscBackStoreStatus( &mem, &status );

   if ( status.fileExist != 1 )
      rc = -1;
   if ( status.fileReadable != 1 )
      rc = -1;
   if ( status.fileWritable != 1 )
      rc = -1;

 the_exit:

   unlink( "MemFile.mem" );
   
   vdscFiniMemoryFile( &mem );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return rc;
}

