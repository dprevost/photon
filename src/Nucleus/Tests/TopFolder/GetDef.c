/*
 * Copyright (C) 2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "folderTest.h"
#include <photon/psoCommon.h>

const bool expectedToPass = true;
psonFolder * pTopFolder;
psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   pTopFolder = initTopFolderTest( &context );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure(  );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psoObjectDefinition def = { PSO_HASH_MAP, 0, 0, 0 };
   psonKeyDefinition keyDef;
   psonDataDefinition fieldDef;
   psonKeyDefinition * retKeyDef = NULL;
   psonDataDefinition * retDataDef = NULL;
   psoObjectDefinition retDef;
   
   pTopFolder = initTopFolderTest( &context );

   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &def,
                                   &fieldDef,
                                   &keyDef,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonTopFolderGetDef( pTopFolder,
                             "Test1",
                             strlen("Test1"),
                             &retDef,
                             &retDataDef,
                             &retKeyDef,
                             &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   if ( retKeyDef != &keyDef ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( retDataDef != &fieldDef ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

