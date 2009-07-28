/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include <photon/photon.h>
#include "API/CommonObject.h"


PSO_HANDLE sessionHandle;
psoaCommonObject object;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   int errcode;
   
   errcode = psoInit( "10701", "Close" );
   assert( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert( errcode == PSO_OK );

   errcode = psoCreateFolder( sessionHandle,
                              "/api_common_open",
                              strlen("/api_common_open") );
   assert( errcode == PSO_OK );
   
   memset( &object, 0, sizeof(psoaCommonObject) );
   object.pSession = (psoaSession *) sessionHandle;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   psoRollback( sessionHandle );
   psoExit();
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_name( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaCommonObjOpen( &object,
                                             PSO_FOLDER,
                                             false,
                                             NULL,
                                             strlen("/api_common_open") ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_object( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaCommonObjOpen( NULL,
                                             PSO_FOLDER,
                                             false,
                                             "/api_common_open",
                                             strlen("/api_common_open") ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_wrong_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaCommonObjOpen( &object,
                                             0,
                                             false,
                                             "/api_common_open",
                                             strlen("/api_common_open") ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_length( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psoaCommonObjOpen( &object,
                                             PSO_FOLDER,
                                             false,
                                             "/api_common_open",
                                             0 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int errcode;
   
   errcode = psoaCommonObjOpen( &object,
                                PSO_FOLDER,
                                false,
                                "/api_common_open",
                                strlen("/api_common_open") );
   assert_true( errcode == PSO_OK );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_name,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_object, setup_test, teardown_test ),
      unit_test_setup_teardown( test_wrong_type,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_length, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,        setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

