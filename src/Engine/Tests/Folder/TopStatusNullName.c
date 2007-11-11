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
#include <vdsf/vdsCommon.h>

const bool expectedToPass = false;

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

   errcode = vdseTopFolderGetStatus( pTopFolder,
                                     NULL,
                                     strlen("Test1"),
                                     &status,
                                     &context );

   ERROR_EXIT( expectedToPass, NULL, ; );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */