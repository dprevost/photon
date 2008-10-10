/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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
   psonMemObject* pObj;
   psoErrors errcode;
   psonSessionContext context;
   unsigned char *buff[9];
   psotObjDummy  *pDummy;
   
   pDummy = initMemObjTest( expectedToPass, &context );
   pObj = &pDummy->memObject;
   
   errcode = psonMemObjectInit( pObj, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   buff[0] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[0] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   buff[1] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[1] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   buff[2] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[2] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   psonFree( pObj, buff[1], PSON_BLOCK_SIZE, &context );
   if ( pDummy->blockGroup.maxFreeBytes != 
        pDummy->blockGroup.freeBytes+2*PSON_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   psonFree( pObj, buff[2], PSON_BLOCK_SIZE, &context );
   psonFree( pObj, buff[0], PSON_BLOCK_SIZE, &context );
   if ( pObj->totalBlocks != 4 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pDummy->blockGroup.maxFreeBytes != pDummy->blockGroup.freeBytes ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   buff[0] = psonMalloc( pObj, 3*PSON_BLOCK_SIZE, &context );
   if ( buff[0] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 4 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs two new blocks at this point */
   if ( pDummy->blockGroup.freeBytes >= PSON_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   buff[3] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[3] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonFree( pObj, buff[3], PSON_BLOCK_SIZE, &context );
   if ( pObj->totalBlocks != 4 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = psonMemObjectFini( pObj, PSON_ALLOC_ANY, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

