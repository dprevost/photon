/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonSessionContext context;
   psonMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = PSON_BLOCK_SIZE*10;
   unsigned char* newBuff[8];
   int i;
   
   initTest( expectedToPass, &context );
   
   ptr = malloc( allocatedLength );
   if ( ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   if ( newBuff[0] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   /* 6 blocks remaining */
   newBuff[1] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   if ( newBuff[1] == NULL ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   /* No blocks remaining */
   newBuff[2] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 6, &context );
   if ( newBuff[2] != NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[0], 2, &context );
   psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[1], 6, &context );
   /* 8 blocks remaining */
   
   for ( i = 0; i < 8; ++i ) {
      newBuff[i] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, &context );
      if ( newBuff[i] == NULL ) {
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
      }
   }
   for ( i = 0; i < 8; i += 2 ) {
      psonFreeBlocks( pAlloc, PSON_ALLOC_ANY, newBuff[i], 1, &context );
   }
   
   /* 4 blocks remaining - fragmented. This new alloc should fail! */
   newBuff[0] = psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, &context );
   if ( newBuff[0] != NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

