/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/MemoryObject.h"
#include "MemObjTest.h"
#include <vdsf/vdsCommon.h>

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdsErrors errcode;
   vdstObjDummy *pDummy;
   vdseSessionContext context;
   vdsObjStatus status;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   
   errcode = vdseMemObjectInit( &pDummy->memObject, 
                                VDSE_IDENT_FOLDER,
                                &pDummy->blockGroup,
                                1 );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseMemObjectStatus( &pDummy->memObject, &status );

   if ( status.numBlocks != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.numBlockGroup != 1 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( status.freeBytes == 0 || status.freeBytes >=VDSE_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

