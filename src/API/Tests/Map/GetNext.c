/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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
#include "API/Map.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   const char * key1  = "My Key1";
   const char * data1 = "My Data1";
   const char * key2  = "My Key2";
   const char * data2 = "My Data2";
   char buffer[200];
   char buffKey[50];
   uint32_t dataLength, keyLength;
   psoBasicObjectDef mapDef = { 
      PSO_FAST_MAP,
      1, 
      { PSO_KEY_VAR_STRING, 0, 4, 10 }
   };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VAR_STRING, 0, 4, 10, 0, 0 }
   };
   psoBasicObjectDef folderDef = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}
   };

   if ( argc > 1 ) {
      errcode = psoInit( argv[1], 0 );
   }
   else {
      errcode = psoInit( "10701", 0 );
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

   errcode = psoCreateObject( sessionHandle,
                              "/amgn",
                              strlen("/amgn"),
                              &folderDef,
                              NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle,
                              "/amgn/test",
                              strlen("/amgn/test"),
                              &mapDef,
                              fields );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapEdit( sessionHandle,
                             "/amgn/test",
                             strlen("/amgn/test"),
                             &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandle,
                               key1,
                               7,
                               data1,
                               8 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapInsert( objHandle,
                               key2,
                               7,
                               data2,
                               8 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* No GetFirst... */
   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_INVALID_ITERATOR ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoFastMapGetNext( NULL,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                NULL,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                2,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                NULL,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                2,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_INVALID_LENGTH ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                NULL,
                                &dataLength );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   /* Reset the internal iterator first... */
   errcode = psoFastMapGetFirst( objHandle,
                                 buffKey,
                                 50,
                                 buffer,
                                 200,
                                 &keyLength,
                                 &dataLength );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoExitSession( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoFastMapGetNext( objHandle,
                                buffKey,
                                50,
                                buffer,
                                200,
                                &keyLength,
                                &dataLength );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

