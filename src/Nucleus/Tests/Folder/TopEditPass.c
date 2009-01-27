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
   psonFolderItem folderItem;
   psoBasicObjectDef folderDef = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}
   };
   psoBasicObjectDef mapDef = { 
      PSO_FAST_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 100 }
   };
   
   psoFieldDefinition fields[1] =  { 
      { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } 
   };

   pTopFolder = initTopFolderTest( expectedToPass, &context );

   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &folderDef,
                                   NULL,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &mapDef,
                                   fields,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test2",
                                 strlen("Test1/Test2"),
                                 PSO_FAST_MAP,
                                 &folderItem,
                                 &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test3/Test2",
                                 strlen("Test3/Test2"),
                                 PSO_FAST_MAP,
                                 &folderItem,
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_NO_SUCH_FOLDER ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonTopFolderEditObject( pTopFolder,
                                 "Test1/Test5",
                                 strlen("Test1/Test5"),
                                 PSO_FAST_MAP,
                                 &folderItem,
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = psocGetLastError( &context.errorHandler );
   if ( errcode != PSO_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
