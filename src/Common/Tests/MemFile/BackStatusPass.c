/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Common/MemoryFile.h"
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdscMemoryFile  mem;
   vdscErrorHandler errorHandler;
   int errcode = 0;
   vdscMemoryFileStatus status;
   
   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );

   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &mem, 10, "MemFile.mem" );

   errcode = vdscCreateBackstore( &mem, 0644, &errorHandler );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   
   vdscBackStoreStatus( &mem, &status );
   if ( status.fileExist != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.fileReadable != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.fileWritable != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.lenghtOK != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.actualLLength != 10*1024 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );

   errcode = unlink( "MemFile.mem" );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdscCreateBackstore( &mem, 0600, &errorHandler );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   
   vdscBackStoreStatus( &mem, &status );
   if ( status.fileExist != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.fileReadable != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.fileWritable != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );

   errcode = unlink( "MemFile.mem" );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );

   errcode = vdscCreateBackstore( &mem, 0660, &errorHandler );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   
   vdscBackStoreStatus( &mem, &status );

   if ( status.fileExist != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.fileReadable != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   if ( status.fileWritable != 1 )
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );

   unlink( "MemFile.mem" );
   
   vdscFiniMemoryFile( &mem );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

