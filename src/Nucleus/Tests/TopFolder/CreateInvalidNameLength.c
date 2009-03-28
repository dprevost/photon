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

#include "folderTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonFolder * pTopFolder;
   psonSessionContext context;
   int errcode;
   bool ok;
   char name[PSO_MAX_FULL_NAME_LENGTH+100];
   psoObjectDefinition def = { PSO_FOLDER, PSO_DEF_NONE, PSO_DEF_NONE };
   
   memset( name, 't', PSO_MAX_FULL_NAME_LENGTH+99 );
   name[PSO_MAX_FULL_NAME_LENGTH+99] = 0;
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   0,
                                   &def,
                                   NULL,
                                   NULL,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonTopFolderCreateObject( pTopFolder,
                                   "/Test2",
                                   1,
                                   &def,
                                   NULL,
                                   NULL,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = psonTopFolderCreateObject( pTopFolder,
                                   name,
                                   PSO_MAX_FULL_NAME_LENGTH+1,
                                   &def,
                                   NULL,
                                   NULL,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   memset( name, 0, PSO_MAX_FULL_NAME_LENGTH+100 );
   memset( name, 't', PSO_MAX_NAME_LENGTH+1 );

   ok = psonTopFolderCreateObject( pTopFolder,
                                   name,
                                   PSO_MAX_NAME_LENGTH+1,
                                   &def,
                                   NULL,
                                   NULL,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

