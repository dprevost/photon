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

psocMemoryFile mem;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_empty_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocInitMemoryFile( &mem, 10, "" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_length_zero( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocInitMemoryFile( &mem, 0, "MemFile.mem" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_mem( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocInitMemoryFile( NULL, 10, "MemFile.mem" ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psocInitMemoryFile( &mem, 10, NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psocInitMemoryFile( &mem, 10, "MemFile.mem" );

   assert_true( mem.initialized == PSOC_MEMFILE_SIGNATURE );
   assert_true( strcmp( mem.name, "MemFile.mem" ) == 0 );
   assert_true( mem.length == 1024*10 );
   assert_true( mem.baseAddr == PSO_MAP_FAILED );
   assert_true( mem.fileHandle == PSO_INVALID_HANDLE );
   
#if defined (WIN32)
   assert_true( mem.mapHandle == PSO_INVALID_HANDLE );
#endif

   psocFiniMemoryFile( &mem );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_empty_name ),
      unit_test( test_length_zero ),
      unit_test( test_null_mem ),
      unit_test( test_null_name ),
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

