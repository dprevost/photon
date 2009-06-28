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
#include "Common/DirAccess.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test1( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok = 0;
   psocDirIterator iterator;
   const char* str;
   psocErrorHandler errorHandler;

   psocInitErrorDefs();
   psocInitDir( &iterator );
   psocInitErrorHandler( &errorHandler );
   
   ok = psocOpenDir( &iterator, ".", &errorHandler );
   assert_true( ok );
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );
   assert_false( str == NULL );
   
   /* Close and reopen */
   psocCloseDir( &iterator );

   ok = psocOpenDir( &iterator, ".", &errorHandler );
   assert_true( ok );

   str = psocDirGetNextFileName( &iterator, &errorHandler );
   assert_false( str == NULL );
   
   /* Close twice and reopen - should work */
   psocCloseDir( &iterator );
   psocCloseDir( &iterator );

   ok = psocOpenDir( &iterator, ".", &errorHandler );
   assert_true( ok );
   
   str = psocDirGetNextFileName( &iterator, &errorHandler );
   assert_false( str == NULL );
   
   psocCloseDir( &iterator );

   psocFiniDir( &iterator );
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

