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
   vdseFolder* pFolder;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   vdsObjStatus objStatus;
   
   pFolder = initFolderTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseFolderInit( pFolder, 0, 1, 0, &status, 5, strCheck("Test1"), 1234, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseFolderInsertObject( pFolder,
                                     strCheckLow("test2"),
                                     strCheck("Test2"),
                                     5,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseFolderInsertObject( pFolder,
                                     strCheckLow("test2/test3"),
                                     strCheck("Test2/test3"),
                                     11,
                                     VDS_FOLDER,
                                     1,
                                     0,
                                     &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseFolderGetStatus( pFolder,
                                  strCheck("test2"),
                                  5,
                                  &objStatus,
                                  &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   if ( objStatus.numDataItem != 1 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( objStatus.numBlocks != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( objStatus.numBlockGroup != 1 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( objStatus.freeBytes == 0 || objStatus.freeBytes >=VDSE_BLOCK_SIZE ) 
      ERROR_EXIT( expectedToPass, NULL, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
