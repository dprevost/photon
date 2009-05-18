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
#include "API/Lifo.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE objHandle, sessionHandle;
   int errcode;
   psoObjectDefinition lifoDef = { PSO_LIFO, 0, 0, 0 };
   psoFieldDefinition fields[1] = {
      { "Field_1", PSO_VARCHAR, {10} }
   };
   PSO_HANDLE dataDefHandle, returnedDef;
   
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

   errcode = psoCreateFolder( sessionHandle,
                              "/api_lifo_record_def",
                              strlen("/api_lifo_record_def") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoDataDefCreate( sessionHandle,
                               "api_lifo_record_def",
                               strlen("api_lifo_record_def"),
                               PSO_DEF_PHOTON_ODBC_SIMPLE,
                               (unsigned char *)fields,
                               sizeof(psoFieldDefinition),
                               &dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateQueue( sessionHandle,
                             "/api_lifo_record_def/test",
                             strlen("/api_lifo_record_def/test"),
                             &lifoDef,
                             dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoLifoOpen( sessionHandle,
                          "/api_lifo_record_def/test",
                          strlen("/api_lifo_record_def/test"),
                          &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoLifoRecordDefinition( NULL, &returnedDef );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoLifoRecordDefinition( sessionHandle, &returnedDef );
   if ( errcode != PSO_WRONG_TYPE_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoLifoRecordDefinition( objHandle, NULL );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoLifoRecordDefinition( objHandle, &returnedDef );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoLifoClose( objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoDataDefClose( returnedDef );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psoExit();

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

