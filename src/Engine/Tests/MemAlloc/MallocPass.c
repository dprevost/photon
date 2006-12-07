/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "MemoryAllocator.h"
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = PAGESIZE*10;
   unsigned char* newBuff[8];
   int i;
   
   initTest( expectedToPass );
   vdscInitErrorHandler( &context.errorHandler );
   
   ptr = malloc( allocatedLength );
   if ( ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   newBuff[0] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   /* 6 pages remaining */
   newBuff[1] = vdseMallocPages( pAlloc, 6, &context );
   if ( newBuff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, );
   /* No pages remaining */
   newBuff[2] = vdseMallocPages( pAlloc, 6, &context );
   if ( newBuff[2] != NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   
   vdseFreePages( pAlloc, newBuff[0], 2, &context );
   vdseFreePages( pAlloc, newBuff[1], 6, &context );
   /* 8 pages remaining */
   
   for ( i = 0; i < 8; ++i )
   {
      newBuff[i] = vdseMallocPages( pAlloc, 1, &context );
      if ( newBuff[i] == NULL )
         ERROR_EXIT( expectedToPass, &context.errorHandler, );
   }
   for ( i = 0; i < 8; i += 2 )
      vdseFreePages( pAlloc, newBuff[i], 1, &context );
   
   /* 4 pages remaining - fragmented. This new alloc should fail! */
   newBuff[0] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[0] != NULL )
      ERROR_EXIT( expectedToPass, NULL, );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

