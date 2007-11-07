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

#include "Engine/MemoryAllocator.h"
#include "Engine/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = VDSE_BLOCK_SIZE*10;
   unsigned char* newBuff[8];
   int i;
   
   initTest( expectedToPass, &context );
   
   ptr = malloc( allocatedLength );
   if ( ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   newBuff[0] = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 2, &context );
   if ( newBuff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   /* 6 blocks remaining */
   newBuff[1] = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 6, &context );
   if ( newBuff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   /* No blocks remaining */
   newBuff[2] = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 6, &context );
   if ( newBuff[2] != NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   vdseFreeBlocks( pAlloc, VDSE_ALLOC_ANY, newBuff[0], 2, &context );
   vdseFreeBlocks( pAlloc, VDSE_ALLOC_ANY, newBuff[1], 6, &context );
   /* 8 blocks remaining */
   
   for ( i = 0; i < 8; ++i )
   {
      newBuff[i] = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 1, &context );
      if ( newBuff[i] == NULL )
         ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   for ( i = 0; i < 8; i += 2 )
      vdseFreeBlocks( pAlloc, VDSE_ALLOC_ANY, newBuff[i], 1, &context );
   
   /* 4 blocks remaining - fragmented. This new alloc should fail! */
   newBuff[0] = vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 2, &context );
   if ( newBuff[0] != NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

