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
   bool ok;
   char name[VDS_MAX_FULL_NAME_LENGTH+100];
   vdsObjectDefinition def = { 
      VDS_FOLDER, 
      0, 
      { 0, 0, 0, 0}, 
      { { "", 0, 0, 0, 0, 0, 0} } 
   };
   
   memset( name, 't', VDS_MAX_FULL_NAME_LENGTH+99 );
   name[VDS_MAX_FULL_NAME_LENGTH+99] = 0;
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   ok = vdseTopFolderCreateObject( pTopFolder,
                                   "Test1",
                                   0,
                                   &def,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != VDS_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = vdseTopFolderCreateObject( pTopFolder,
                                   "/Test2",
                                   1,
                                   &def,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != VDS_INVALID_OBJECT_NAME ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }

   ok = vdseTopFolderCreateObject( pTopFolder,
                                   name,
                                   VDS_MAX_FULL_NAME_LENGTH+1,
                                   &def,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != VDS_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   memset( name, 0, VDS_MAX_FULL_NAME_LENGTH+100 );
   memset( name, 't', VDS_MAX_NAME_LENGTH+1 );

   ok = vdseTopFolderCreateObject( pTopFolder,
                                   name,
                                   VDS_MAX_NAME_LENGTH+1,
                                   &def,
                                   &context );
   if ( ok != false ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != VDS_OBJECT_NAME_TOO_LONG ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

