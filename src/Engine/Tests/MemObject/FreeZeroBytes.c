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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psnMemObject* pObj;
   vdsErrors errcode;
   psnSessionContext context;
   unsigned char *buff[9];
   vdstObjDummy  *pDummy;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   pObj = &pDummy->memObject;
   
   errcode = psnMemObjectInit( pObj, 
                                PSN_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   buff[0] = psnMalloc( pObj, PSN_BLOCK_SIZE, &context );
   if ( buff[0] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psnFree( pObj, buff[0], 0, &context );
   
   psnMemObjectFini( pObj, PSN_ALLOC_ANY, &context );
   
   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

