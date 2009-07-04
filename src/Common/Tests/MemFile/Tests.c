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

#include "Common/MemoryFile.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#  define unlink(a) _unlink(a)
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* 
 * This is going to be a very simple test. We open a shared memory file,
 * write to it, synch and close. Reopen it and check that the content 
 * matches what we wrote previously. Like I said, very simple, nothing to
 * write home about...
 */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocMemoryFile  mem1, mem2;
   psocErrorHandler errorHandler;
   void*           pAddr = NULL;
   bool ok;
   unsigned char* str;
   unsigned int i;
   
   /* The rename is a work around for a bug on Windows. It seems that the delete
    * call is not as synchroneous as it should be...
    */
   rename( "MemFile.mem", "MemFile.old" );
   unlink( "MemFile.old" );
   
   psocInitErrorDefs();
   psocInitErrorHandler( &errorHandler );
   psocInitMemoryFile( &mem1, 10, "MemFile.mem" );
   psocInitMemoryFile( &mem2, 10, "MemFile.mem" );
   
   ok = psocCreateBackstore( &mem1, 0600, &errorHandler );
   assert_true( ok );
   
   ok = psocOpenMemFile( &mem1, &pAddr, &errorHandler );
   assert_true( ok );
   
   assert_false( mem1.fileHandle == PSO_INVALID_HANDLE );
   assert_false( mem1.baseAddr == PSO_MAP_FAILED );
   
#if defined (WIN32)
   assert_false( mem1.mapHandle == PSO_INVALID_HANDLE );
#endif

   str = (unsigned char*) pAddr;
   
   for ( i = 0; i < 10*1024; ++i ) {
      str[i] = (unsigned char) (i % 256);
   }
   
   psocSyncMemFile( &mem1, &errorHandler );
   psocCloseMemFile( &mem1, &errorHandler );

   ok = psocOpenMemFile( &mem2, &pAddr, &errorHandler );
   assert_true( ok );
   
   str = (unsigned char*) pAddr;
   for ( i = 0; i < 10*1024; ++i ) {
      assert_true( str[i] == (unsigned char)(i % 256) );
   }
   
   psocCloseMemFile( &mem2, &errorHandler );

   unlink( "MemFile.mem" );

   psocFiniMemoryFile( &mem1 );
   psocFiniMemoryFile( &mem2 );
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

