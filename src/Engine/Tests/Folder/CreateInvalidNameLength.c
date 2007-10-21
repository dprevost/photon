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
   char name[VDS_MAX_FULL_NAME_LENGTH+100];
   
   memset( name, 't', VDS_MAX_FULL_NAME_LENGTH+99 );
   name[VDS_MAX_FULL_NAME_LENGTH+99] = 0;
   
   pTopFolder = initTopFolderTest( expectedToPass, &context );

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "Test1",
                                        0,
                                        VDS_FOLDER,
                                        &context );
#if VDS_SUPPORT_i18n
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
#else
   if ( errcode != -1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_INVALID_OBJECT_NAME ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
#endif

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "/Test2",
                                        1,
                                        VDS_FOLDER,
                                        &context );
#if VDS_SUPPORT_i18n
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
#else
   if ( errcode != -1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_INVALID_OBJECT_NAME ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
#endif

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        name,
                                        VDS_MAX_FULL_NAME_LENGTH+1,
                                        VDS_FOLDER,
                                        &context );
   if ( errcode != -1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_OBJECT_NAME_TOO_LONG ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   memset( name, 0, VDS_MAX_FULL_NAME_LENGTH+100 );
   memset( name, 't', VDS_MAX_NAME_LENGTH+1 );

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        name,
                                        VDS_MAX_NAME_LENGTH+1,
                                        VDS_FOLDER,
                                        &context );
   if ( errcode != -1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   errcode = vdscGetLastError( &context.errorHandler );
   if ( errcode != VDS_OBJECT_NAME_TOO_LONG ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
