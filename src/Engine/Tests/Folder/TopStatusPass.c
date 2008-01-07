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
#include <vdsf/vdsCommon.h>

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseFolder * pTopFolder;
   vdseSessionContext context;
   int errcode;
   vdsObjStatus status;
   
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

   errcode = vdseTopFolderCreateObject( pTopFolder,
                                        "Test1/Test3",
                                        strlen("Test1/Test3"),
                                        VDS_FOLDER,
                                        &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseTopFolderGetStatus( pTopFolder,
                                     "Test1",
                                     strlen("Test1"),
                                     &status,
                                     &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( status.numBlocks != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.numBlockGroup != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.freeBytes == 0 || status.freeBytes >=VDSE_BLOCK_SIZE ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( status.numDataItem != 2 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
