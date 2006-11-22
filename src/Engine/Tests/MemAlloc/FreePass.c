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
   unsigned char* newBuff[5];
   int i;
   
   initTest( false );
   vdscInitErrorHandler( &error );
   
   ptr = malloc( allocatedLength );

   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &error );
   
   newBuff[0] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[0] == NULL ) return 1;

   vdseFreePages( pAlloc, newBuff[0], 2, &error );
   if (pAlloc->totalAllocPages != 2 ) return 1;
   if (pAlloc->numFreeCalls != 1 ) return 1;
   
   /*
    * Test that the code will reunite free buffers (if they are
    * following each other, of course).
    */
   /* unite with following buffer */
   newBuff[0] = vdseMallocPages( pAlloc, 3, &error );
   if ( newBuff[0] == NULL ) return 1;
   newBuff[1] = vdseMallocPages( pAlloc, 3, &error );
   if ( newBuff[1] == NULL ) return 1;
   newBuff[2] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[2] == NULL ) return 1;

   vdseFreePages( pAlloc, newBuff[1], 3, &error );
   vdseFreePages( pAlloc, newBuff[0], 3, &error );
   /* if the "unite" failed, no 6 pages free buffer should exist */
   newBuff[3] = vdseMallocPages( pAlloc, 6, &error );
   if ( newBuff[3] == NULL ) return 1;
   vdseFreePages( pAlloc, newBuff[3], 6, &error );
   vdseFreePages( pAlloc, newBuff[2], 2, &error );

   /* unite with preceding buffer */
   newBuff[0] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[0] == NULL ) return 1;
   newBuff[1] = vdseMallocPages( pAlloc, 3, &error );
   if ( newBuff[1] == NULL ) return 1;
   newBuff[2] = vdseMallocPages( pAlloc, 3, &error );
   if ( newBuff[2] == NULL ) return 1;

   vdseFreePages( pAlloc, newBuff[1], 3, &error );
   vdseFreePages( pAlloc, newBuff[2], 3, &error );
   /* if the "unite" failed, no 6 pages free buffer should exist */
   newBuff[3] = vdseMallocPages( pAlloc, 6, &error );
   if ( newBuff[3] == NULL ) return 1;   
   vdseFreePages( pAlloc, newBuff[0], 2, &error );
   vdseFreePages( pAlloc, newBuff[3], 6, &error );

   /* unite with both */
   newBuff[0] = vdseMallocPages( pAlloc, 1, &error );
   if ( newBuff[0] == NULL ) return 1;
   newBuff[1] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[1] == NULL ) return 1;
   newBuff[2] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[2] == NULL ) return 1;   
   newBuff[3] = vdseMallocPages( pAlloc, 2, &error );
   if ( newBuff[3] == NULL ) return 1;   
   newBuff[4] = vdseMallocPages( pAlloc, 1, &error );
   if ( newBuff[4] == NULL ) return 1;   

   vdseFreePages( pAlloc, newBuff[1], 2, &error );
   vdseFreePages( pAlloc, newBuff[3], 2, &error );
   vdseFreePages( pAlloc, newBuff[2], 2, &error );
   /* if the "unite" failed, no 6 pages free buffer should exist */
   newBuff[1] = vdseMallocPages( pAlloc, 6, &error );
   if ( newBuff[1] == NULL ) return 1;   
   
   vdseFreePages( pAlloc, newBuff[0], 1, &error );
   vdseFreePages( pAlloc, newBuff[1], 6, &error );
   vdseFreePages( pAlloc, newBuff[4], 1, &error );

   if (pAlloc->totalAllocPages != 2 ) return 1;
   
   return 0;
}
