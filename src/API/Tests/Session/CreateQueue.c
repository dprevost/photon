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
#include "Tests/PrintError.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle;
   int errcode;
   psoObjectDefinition folderDef = { PSO_FOLDER, 0, 0, 0 };
   psoObjectDefinition queueDef = { PSO_QUEUE, 0, 0, 0 };
   psoFieldDefinition fields[2] = {
      { "field1", PSO_INTEGER, 3, 0, 0, 0, 0 },
      { "dummy",  PSO_INTEGER, 1, 0, 0, 0, 0 }
   };
   
   if ( argc > 1 ) {
      errcode = psoInit( argv[1] );
   }
   else {
      errcode = psoInit( "10701" );
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
                              "/api_session_create_queue",
                              strlen("/api_session_create_queue") );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* End of preparation work. */

   errcode = psoCreateObject( sessionHandle,
                              "/api_session_create_queue/test",
                              strlen("/api_session_create_queue/test"),
                              &queueDef,
                              NULL,
                              dataDefHandle );
   if ( errcode != PSO_INVALID_FIELD_LENGTH_INT ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   

   strcpy( fields[0].name, "Field_1" );
   queueDef.numFields = 2;
   fields[0].type = PSO_VARBINARY;
   fields[0].maxLength = 0;
   fields[0].minLength = 200;
   fields[1].type = PSO_TINYINT;
   strcpy( fields[1].name, "Field_2" );
   errcode = psoCreateObject( sessionHandle,
                              "/api_session_create_queue/test",
                              strlen("/api_session_create_queue/test"),
                              &queueDef,
                              NULL,
                              dataDefHandle );
   if ( errcode != PSO_INVALID_FIELD_TYPE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }


   fields[0].type = PSO_BINARY;
   fields[0].length = 10;
   fields[1].type = PSO_TINYINT;
   strcpy( fields[1].name, "Field_2" );
   errcode = psoCreateObject( sessionHandle,
                              "/api_session_create_queue/test",
                              strlen("/api_session_create_queue/test"),
                              &queueDef,
                              NULL,
                              dataDefHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

