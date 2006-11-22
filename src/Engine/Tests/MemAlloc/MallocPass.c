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

int main()
{
   vdscErrorHandler error;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = PAGESIZE*10;
   unsigned char* newBuff[8];
   int i;
   
   initTest( true );
   vdscInitErrorHandler( &error );
   
   ptr = malloc( allocatedLength );

   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &error );
   
   newBuff[0] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[0] == NULL ) return 1;
   /* 6 pages remaining */
   newBuff[1] = vdseMallocPages( pAlloc, 6, &error );
   if ( newBuff[1] == NULL ) return 1;
   /* No pages remaining */
   newBuff[2] = vdseMallocPages( pAlloc, 6, &error );
   if ( newBuff[2] != NULL ) return 1;
   
   vdseFreePages( pAlloc, newBuff[0], 2, &error );
   vdseFreePages( pAlloc, newBuff[1], 6, &error );
   /* 8 pages remaining */
   
   for ( i = 0; i < 8; ++i )
   {
      newBuff[i] = vdseMallocPages( pAlloc, 1, &error );
   }
   for ( i = 0; i < 8; i += 2 )
      vdseFreePages( pAlloc, newBuff[i], 1, &error );
   
   /* 4 pages remaining - fragmented. This new alloc should fail! */
   newBuff[0] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[0] != NULL ) return 1;
   
   return 0;
}
