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
   if ( errcode != VDS_OK ) ERROR_EXIT( expectedToPass, NULL, ; );
   
   buff[0] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   buff[1] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   buff[2] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[2] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   /* Needs two new blocks at this point */
   buff[3] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[3] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 6 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   /* Needs NO new block at this point */
   buff[4] = vdseMalloc( pObj, VDSE_BLOCK_SIZE/2, &context );
   if ( buff[4] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 6 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Needs NO new block at this point */
   buff[5] = vdseMalloc( pObj, VDSE_BLOCK_SIZE/2, &context );
   if ( buff[5] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 6 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Needs a new block at this point */
   buff[6] = vdseMalloc( pObj, VDSE_BLOCK_SIZE/2, &context );
   if ( buff[6] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 7 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* Needs a new block at this point */
   buff[7] = vdseMalloc( pObj, VDSE_BLOCK_SIZE/2, &context );
   if ( buff[7] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   if ( pObj->totalBlocks != 8 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   /* We exhausted all the memory */
   buff[8] = vdseMalloc( pObj, VDSE_BLOCK_SIZE, &context );
   if ( buff[8] != NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   if ( pObj->totalBlocks != 8 )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   errcode = vdseMemObjectFini( pObj, &context );
   if ( errcode != VDS_OK )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

