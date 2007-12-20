/*
 * Copyright (C) 2006-2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseMemObject* pObj;
   vdsErrors errcode;
   vdseSessionContext context;
   unsigned char *buff[9];
   vdstObjDummy  *pDummy;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   pObj = &pDummy->memObject;
   
   errcode = vdseMemObjectInit( pObj, 
                                VDSE_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   if ( errcode != VDS_OK )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   buff[0] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[0] == NULL ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   buff[1] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   buff[2] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[2] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseFree( pObj, buff[1], VDSE_BLOCK_SIZE, &context );
   if ( pDummy->blockGroup.maxFreeBytes != pDummy->blockGroup.freeBytes+2*VDSE_BLOCK_SIZE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   vdseFree( pObj, buff[2], VDSE_BLOCK_SIZE, &context );
   vdseFree( pObj, buff[0], VDSE_BLOCK_SIZE, &context );
   if ( pObj->totalBlocks != 4 ) 
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pDummy->blockGroup.maxFreeBytes != pDummy->blockGroup.freeBytes )
      ERROR_EXIT( expectedToPass, NULL, ; );

   buff[0] = vdseMalloc( pObj, 3*VDSE_BLOCK_SIZE, &context );
   if ( buff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 4 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   /* Needs two new blocks at this point */
   if ( pDummy->blockGroup.freeBytes >= VDSE_BLOCK_SIZE )
      ERROR_EXIT( expectedToPass, NULL, ; );
   buff[3] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[3] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 6 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseFree( pObj, buff[3], VDSE_BLOCK_SIZE, &context );
   if ( pObj->totalBlocks != 4 )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseMemObjectFini( pObj, VDSE_ALLOC_ANY, &context );
   if ( errcode != VDS_OK )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

