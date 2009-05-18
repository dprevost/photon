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
#include "Tests/PrintError.h"
#include "API/KeyDefinition.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE defHandle, sessionHandle;
   int errcode;
   psoKeyFieldDefinition key = { "Key_1", PSO_KEY_VARCHAR, 10 };
  
   if ( argc > 1 ) {
      errcode = psoInit( argv[1], argv[0] );
   }
   else {
      errcode = psoInit( "10701", argv[0] );
   }
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psoInitSession( &sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoKeyDefCreate( NULL,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              NULL,
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              0,
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              0,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_WRONG_OBJECT_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              NULL,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              0,
                              &defHandle );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoKeyDefCreate( sessionHandle,
                              "api_key_definition_create",
                              strlen("api_key_definition_create"),
                              PSO_DEF_USER_DEFINED,
                              (const unsigned char *)&key,
                              sizeof(psoKeyFieldDefinition),
                              &defHandle );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
