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
#include "PrintError.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * This is going to be a very simple test. We open a shared memory file,
 * write to it, synch and close. Reopen it and check that the content 
 * matches what we wrote previously. Like I said, very simple, nothing to
 * write home about...
 */

int main()
{
   vdscMemoryFile  mem1, mem2;
   vdscErrorHandler errorHandler;
   void*           pAddr = NULL;
   int errcode = 0, rc = 0;
   unsigned char* str;
   unsigned int i;
   
   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );
   
   vdscInitErrorDefs();
   vdscInitErrorHandler( &errorHandler );
   vdscInitMemoryFile( &mem1, 10, "MemFile.mem" );
   vdscInitMemoryFile( &mem2, 10, "MemFile.mem" );
   
   errcode = vdscCreateBackstore( &mem1, 0600, &errorHandler );
   if ( errcode != 0 ) 
   {
      rc = -1;
      goto the_exit;
   }

   errcode = vdscOpenMemFile( &mem1, &pAddr, &errorHandler );

   if ( errcode != 0 ) rc = -1;
   if ( mem1.fileHandle == VDS_INVALID_HANDLE ) rc = -1;
   if ( mem1.baseAddr == VDS_MAP_FAILED ) rc = -1;
   
#if defined (WIN32)
   if ( mem1.mapHandle == VDS_INVALID_HANDLE ) rc = -1;
#endif

   if ( rc != 0 ) goto the_exit;
   
   str = (unsigned char*) pAddr;
   
   for ( i = 0; i < 10*1024; ++i )
      str[i] = (unsigned char) (i % 256);


   vdscSyncMemFile( &mem1, &errorHandler );
   vdscCloseMemFile( &mem1, &errorHandler );

   errcode = vdscOpenMemFile( &mem2, &pAddr, &errorHandler );

   if ( errcode != 0 )
   {
      rc = -1;
      goto the_exit;
   }
   
   str = (unsigned char*) pAddr;
   for ( i = 0; i < 10*1024; ++i )
      if ( str[i] != (unsigned char)(i % 256) )
      {
         rc = -1;
         break;
      }

   vdscCloseMemFile( &mem2, &errorHandler );

the_exit:

   printError( &errorHandler );
   unlink( "MemFile.mem" );

   vdscFiniMemoryFile( &mem1 );
   vdscFiniMemoryFile( &mem2 );
   vdscFiniErrorHandler( &errorHandler );
   vdscFiniErrorDefs();

   return rc;
}
