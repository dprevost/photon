/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "Test1",
                                        strlen("Test1"),
                                        VDS_FOLDER,
                                        &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "Test1/Test2",
                                        strlen("Test1/Test2"),
                                        VDS_FOLDER,
                                        &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTopFolderDestroyObject( pTopFolder,
                                         "Test1",
                                         strlen("Test1"),
                                         &context );
                                         
   if ( vdscGetLastError(&context.errorHandler) != VDS_FOLDER_IS_NOT_EMPTY )
   {
      if ( errcode != 0 ) 
      {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
      else
         ERROR_EXIT( expectedToPass, NULL, ; );
   }

   errcode = vdseTopFolderDestroyObject( pTopFolder,
                                         "Test1/Test2",
                                         strlen("Test1/Test2"),
                                         &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseTopFolderDestroyObject( pTopFolder,
                                         "Test3/Test2",
                                         strlen("Test3/Test2"),
                                         &context );
   if ( errcode != -1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_NO_SUCH_FOLDER ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTopFolderDestroyObject( pTopFolder,
                                         "Test1/Test5",
                                         strlen("Test1/Test5"),
                                         &context );
   if ( errcode != -1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_NO_SUCH_OBJECT )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   /* Calling destroy on the same object, twice */
   errcode = vdseTopFolderDestroyObject( pTopFolder,
                                         "Test1/Test2",
                                         strlen("Test1/Test2"),
                                         &context );
   if ( errcode != -1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_NO_SUCH_OBJECT )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTopFolderDestroyObject( pTopFolder,
                                         "Test1",
                                         strlen("Test1"),
                                         &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
