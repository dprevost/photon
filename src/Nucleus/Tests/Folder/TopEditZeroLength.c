/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation and appearing in the file COPYING included in the
 * packaging of this library.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "folderTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnFolder * pTopFolder;
   psnSessionContext context;
   int errcode;
   bool ok;
   psnFolderItem folderItem;
   psoObjectDefinition folderDef = { 
      PSO_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   psoObjectDefinition mapDef = { 
      PSO_FAST_MAP, 
      1, 
      { PSO_KEY_VAR_STRING, 0, 1, 100 }, 
      { { "Field_1", PSO_VAR_STRING, 0, 1, 100, 0, 0 } } 
   };
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   ok = psnTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   strlen("Test1"),
                                   &folderDef,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderCreateObject( pTopFolder,
                                   "Test1/Test2",
                                   strlen("Test1/Test2"),
                                   &mapDef,
                                   &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTopFolderEditObject( pTopFolder,
                                 "Test1/Test2",
                                 0,
                                 PSO_FAST_MAP,
                                 &folderItem,
                                 &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != PSO_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

