/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
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
   if ( pDummy->blockGroup.maxFreeBytes != 
        pDummy->blockGroup.freeBytes+PSON_BLOCK_SIZE ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   buff[1] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[1] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   buff[2] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[2] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
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
   
   /* Needs NO new block at this point */
   buff[4] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   if ( buff[4] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs NO new block at this point */
   buff[5] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   if ( buff[5] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs a new block at this point */
   buff[6] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   if ( buff[6] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 7 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* Needs a new block at this point */
   buff[7] = psonMalloc( pObj, PSON_BLOCK_SIZE/2, &context );
   if ( buff[7] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pObj->totalBlocks != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   /* We exhausted all the memory */
   buff[8] = psonMalloc( pObj, PSON_BLOCK_SIZE, &context );
   if ( buff[8] != NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pObj->totalBlocks != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = psonMemObjectFini( pObj, PSON_ALLOC_ANY, &context );
   if ( errcode != PSO_OK ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

