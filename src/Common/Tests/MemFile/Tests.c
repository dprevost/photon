/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

/* 
 * This is going to be a very simple test. We open a shared memory file,
 * write to it, synch and close. Reopen it and check that the content 
 * matches what we wrote previously. Like I said, very simple, nothing to
 * write home about...
 */

int main()
{
   pscMemoryFile  mem1, mem2;
   pscErrorHandler errorHandler;
   void*           pAddr = NULL;
   bool ok;
   unsigned char* str;
   unsigned int i;
   
   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );
   
   pscInitErrorDefs();
   pscInitErrorHandler( &errorHandler );
   pscInitMemoryFile( &mem1, 10, "MemFile.mem" );
   pscInitMemoryFile( &mem2, 10, "MemFile.mem" );
   
   ok = pscCreateBackstore( &mem1, 0600, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   ok = pscOpenMemFile( &mem1, &pAddr, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   if ( mem1.fileHandle == VDS_INVALID_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   if ( mem1.baseAddr == VDS_MAP_FAILED ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
   
#if defined (WIN32)
   if ( mem1.mapHandle == VDS_INVALID_HANDLE ) {
      ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
   }
#endif

   str = (unsigned char*) pAddr;
   
   for ( i = 0; i < 10*1024; ++i ) {
      str[i] = (unsigned char) (i % 256);
   }
   
   pscSyncMemFile( &mem1, &errorHandler );
   pscCloseMemFile( &mem1, &errorHandler );

   ok = pscOpenMemFile( &mem2, &pAddr, &errorHandler );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &errorHandler, unlink( "MemFile.mem" ) );
   }
   
   str = (unsigned char*) pAddr;
   for ( i = 0; i < 10*1024; ++i ) {
      if ( str[i] != (unsigned char)(i % 256) ) {
         ERROR_EXIT( expectedToPass, NULL, unlink( "MemFile.mem" ) );
      }
   }
   
   pscCloseMemFile( &mem2, &errorHandler );

   unlink( "MemFile.mem" );

   pscFiniMemoryFile( &mem1 );
   pscFiniMemoryFile( &mem2 );
   pscFiniErrorHandler( &errorHandler );
   pscFiniErrorDefs();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

