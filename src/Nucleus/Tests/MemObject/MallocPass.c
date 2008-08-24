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
   psnMemObject* pObj;
   psoErrors errcode;
   psnSessionContext context;
   unsigned char *buff[9];
   psotObjDummy  *pDummy;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   pObj = &pDummy->memObject;
   
   errcode = psnMemObjectInit( pObj, 
                                PSN_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   buff[0] = psnMalloc( pObj, PSN_BLOCK_SIZE, &context );
   if ( buff[0] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pDummy->blockGroup.maxFreeBytes != 
        pDummy->blockGroup.freeBytes+PSN_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   buff[1] = psnMalloc( pObj, PSN_BLOCK_SIZE, &context );
   if ( buff[1] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   buff[2] = psnMalloc( pObj, PSN_BLOCK_SIZE, &context );
   if ( buff[2] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Needs two new blocks at this point */
   if ( pDummy->blockGroup.freeBytes >= PSN_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   buff[3] = psnMalloc( pObj, PSN_BLOCK_SIZE, &context );
   if ( buff[3] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs NO new block at this point */
   buff[4] = psnMalloc( pObj, PSN_BLOCK_SIZE/2, &context );
   if ( buff[4] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs NO new block at this point */
   buff[5] = psnMalloc( pObj, PSN_BLOCK_SIZE/2, &context );
   if ( buff[5] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs a new block at this point */
   buff[6] = psnMalloc( pObj, PSN_BLOCK_SIZE/2, &context );
   if ( buff[6] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 7 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs a new block at this point */
   buff[7] = psnMalloc( pObj, PSN_BLOCK_SIZE/2, &context );
   if ( buff[7] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* We exhausted all the memory */
   buff[8] = psnMalloc( pObj, PSN_BLOCK_SIZE, &context );
   if ( buff[8] != NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pObj->totalBlocks != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psnMemObjectFini( pObj, PSN_ALLOC_ANY, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

