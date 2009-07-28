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
  
   errcode = psoInit( "10701", "Create" );
   assert_true( errcode == PSO_OK );
   
   errcode = psoInitSession( &sessionHandle );
   assert_true( errcode == PSO_OK );

   /* Invalid arguments to tested function. */

   errcode = psoDataDefCreate( NULL,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoDataDefCreate( sessionHandle,
                               NULL,
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               0,
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               0,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_WRONG_OBJECT_TYPE );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               NULL,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               0,
                               &defHandle );
   assert_true( errcode == PSO_INVALID_LENGTH );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_OK );

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   assert_true( errcode == PSO_OK );

   errcode = psoDataDefCreate( sessionHandle,
                               "api_data_definition_create",
                               strlen("api_data_definition_create"),
                               PSO_DEF_USER_DEFINED,
                               (const unsigned char *) fields,
                               sizeof(psoFieldDefinition),
                               &defHandle );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

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

