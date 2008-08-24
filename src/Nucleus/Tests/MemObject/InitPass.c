/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#include "Nucleus/MemoryObject.h"
#include "MemObjTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psoErrors errcode;
   psnMemObject* pObj;
   psotObjDummy  *pDummy;
   psnSessionContext context;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   pObj = &pDummy->memObject;
   
   errcode = psnMemObjectInit( pObj, 
                                PSN_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pObj->objType != PSN_IDENT_ALLOCATOR ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pObj->totalBlocks != 4 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psnMemObjectFini( pObj, PSN_ALLOC_ANY, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

