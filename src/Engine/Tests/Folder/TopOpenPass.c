/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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
   vdseFolder * pTopFolder;
   vdseSessionContext context;
   int errcode;
   vdseFolderItem folderItem;
   vdsObjectDefinition def = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "Test1",
                                        strlen("Test1"),
                                        &def,
                                        &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "Test1/Test2",
                                        strlen("Test1/Test2"),
                                        &def,
                                        &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTopFolderOpenObject( pTopFolder,
                                      "Test1/Test2",
                                      strlen("Test1/Test2"),
                                      VDS_FOLDER,
                                      &folderItem,
                                      &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTopFolderOpenObject( pTopFolder,
                                      "Test3/Test2",
                                      strlen("Test3/Test2"),
                                      VDS_FOLDER,
                                      &folderItem,
                                      &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_NO_SUCH_FOLDER ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseTopFolderOpenObject( pTopFolder,
                                      "Test1/Test5",
                                      strlen("Test1/Test5"),
                                      VDS_FOLDER,
                                      &folderItem,
                                      &context );
   if ( errcode != -1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_NO_SUCH_OBJECT ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
