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
   char * name;
   psoUint32 nameLength;
   unsigned char * dataDef;
   unsigned int dataDefLength;
   
   errcode = psoInit( "10701", "GetDef" );
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
   errcode = psoaDataDefGetDef( NULL,
                                &name,
                                &nameLength,
                                &type,
                                &dataDef,
                                &dataDefLength );
   assert_true( errcode == PSO_NULL_HANDLE );

   errcode = psoaDataDefGetDef( sessionHandle,
                                &name,
                                &nameLength,
                                &type,
                                &dataDef,
                                &dataDefLength );
   assert_true( errcode == PSO_WRONG_TYPE_HANDLE );

   errcode = psoaDataDefGetDef( defHandle,
                                NULL,
                                &nameLength,
                                &type,
                                &dataDef,
                                &dataDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaDataDefGetDef( defHandle,
                                &name,
                                NULL,
                                &type,
                                &dataDef,
                                &dataDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaDataDefGetDef( defHandle,
                                &name,
                                &nameLength,
                                NULL,
                                &dataDef,
                                &dataDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaDataDefGetDef( defHandle,
                                &name,
                                &nameLength,
                                &type,
                                NULL,
                                &dataDefLength );
   assert_true( errcode == PSO_NULL_POINTER );

   errcode = psoaDataDefGetDef( defHandle,
                                &name,
                                &nameLength,
                                &type,
                                &dataDef,
                                NULL );
   assert_true( errcode == PSO_NULL_POINTER );

   /* End of invalid args. This call should succeed. */
   errcode = psoaDataDefGetDef( defHandle,
                                &name,
                                &nameLength,
                                &type,
                                &dataDef,
                                &dataDefLength );
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

