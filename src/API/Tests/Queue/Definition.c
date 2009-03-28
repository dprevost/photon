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
#include "API/Queue.h"

const bool expectedToPass = true;

struct dummy {
   char c;
   uint32_t u32;
   char str[30];
   uint16_t u16;
   unsigned char bin[1];
};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main( int argc, char * argv[] )
{
   PSO_HANDLE sessionHandle, objHandle;
   int errcode;
   struct dummy * data1 = NULL;
   size_t lenData;

   psoObjectDefinition folderDef = { PSO_FOLDER, PSO_DEF_NONE, PSO_DEF_NONE };
   psoObjectDefinition queueDef = { PSO_QUEUE, PSO_DEF_NONE, PSO_DEF_USER_DEFINED };

   psoFieldDefinition fields[5] = {
      { "field1", PSO_TINYINT,   {0} },
      { "field2", PSO_INTEGER,   {0} },
      { "field3", PSO_CHAR,      {30} },
      { "field4", PSO_SMALLINT,  {0} },
      { "field5", PSO_LONGVARBINARY, {0} }
   };
   
   psoFieldDefinition retFields[5];
   psoObjectDefinition retDef;
   
   memset( &retDef, 0, sizeof(psoObjectDefinition) );
   memset( &retFields, 0, 5*sizeof(psoFieldDefinition) );

   lenData = offsetof(struct dummy, bin) + 10;
   data1 = (struct dummy *)malloc( lenData );
   
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
                              "/aqsp",
                              strlen("/aqsp"),
                              &folderDef,
                              NULL,
                              NULL );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoCreateObject( sessionHandle,
                              "/aqsp/test",
                              strlen("/aqsp/test"),
                              &queueDef,
                              NULL,
                              0,
                              (unsigned char *)fields,
                              5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoQueueOpen( sessionHandle,
                           "/aqsp/test",
                           strlen("/aqsp/test"),
                           &objHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoQueuePush( objHandle, data1, lenData );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Invalid arguments to tested function. */

   errcode = psoQueueDefinition( NULL, &retDef, 
                                 (unsigned char *)retFields, 
                                 5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_NULL_HANDLE ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoQueueDefinition( objHandle, NULL, (unsigned char *)retFields, 
                                 5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_NULL_POINTER ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* End of invalid args. This call should succeed. */
   errcode = psoQueueDefinition( objHandle, &retDef, NULL, 0 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   if ( memcmp( &queueDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoQueueDefinition( objHandle, &retDef, 
                                 (unsigned char *)retFields,
                                 5*sizeof(psoFieldDefinition) );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   if ( memcmp( &queueDef, &retDef, sizeof(psoObjectDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( memcmp( fields, retFields, 5*sizeof(psoFieldDefinition) ) != 0 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   /* Close the session and try to act on the object */

   errcode = psoExitSession( sessionHandle );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = psoQueueDefinition( objHandle, &retDef, NULL, 0 );
   if ( errcode != PSO_SESSION_IS_TERMINATED ) {
      fprintf( stderr, "err: %d\n", errcode );
      ERROR_EXIT( expectedToPass, NULL, ; );
   }

   psoExit();
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

