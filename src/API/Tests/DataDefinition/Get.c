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

#include "Common/Common.h"
#include <photon/photon.h>
#include "API/DataDefinition.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   PSO_HANDLE defHandle, sessionHandle;
   int errcode;
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   enum psoDefinitionType type;
   unsigned char dataDef[100];
   uint32_t dataDefLength = 100;
  
   errcode = psoInit( "10701", "Get" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_get",
                               strlen("api_data_definition_get"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */
   errcode = psoDataDefGet( NULL,
                            &type,
                            dataDef,
                            dataDefLength );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoDataDefGet( sessionHandle,
                            &type,
                            dataDef,
                            dataDefLength );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoDataDefGet( defHandle,
                            NULL,
                            dataDef,
                            dataDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoDataDefGet( defHandle,
                            &type,
                            NULL,
                            dataDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoDataDefGet( defHandle,
                            &type,
                            dataDef,
                            0 );
   assert_true( errcode == PSO_INVALID_LENGTH );

   /* End of invalid args. This call should succeed. */
   errcode = psoDataDefGet( defHandle,
                            &type,
                            dataDef,
                            dataDefLength );
   assert_true( errcode == PSO_OK );
   
   psoExit();

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test( test_pass )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

