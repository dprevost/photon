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

#include "MemoryAllocator.h"
#include "EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = VDSE_PAGE_SIZE*10;
   unsigned char* newBuff[5];
   
   initTest( expectedToPass );
   vdscInitErrorHandler( &context.errorHandler );
   
   ptr = malloc( allocatedLength );
   if ( ptr == NULL )
      ERROR_EXIT( expectedToPass, NULL, ; );
   
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_PAGE_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   newBuff[0] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseFreePages( pAlloc, newBuff[0], 2, &context );
   if (pAlloc->totalAllocPages != 2 ) ERROR_EXIT( expectedToPass, NULL, ; );
   if (pAlloc->numFreeCalls != 1 ) ERROR_EXIT( expectedToPass, NULL, ; );
   
   /*
    * Test that the code will reunite free buffers (if they are
    * following each other, of course).
    */
   /* unite with following buffer */
   newBuff[0] = vdseMallocPages( pAlloc, 3, &context );
   if ( newBuff[0] == NULL ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[1] = vdseMallocPages( pAlloc, 3, &context );
   if ( newBuff[1] == NULL ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[2] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[2] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseFreePages( pAlloc, newBuff[1], 3, &context );
   vdseFreePages( pAlloc, newBuff[0], 3, &context );
   /* if the "unite" failed, no 6 pages free buffer should exist */
   newBuff[3] = vdseMallocPages( pAlloc, 6, &context );
   if ( newBuff[3] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   vdseFreePages( pAlloc, newBuff[3], 6, &context );
   vdseFreePages( pAlloc, newBuff[2], 2, &context );

   /* unite with preceding buffer */
   newBuff[0] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[1] = vdseMallocPages( pAlloc, 3, &context );
   if ( newBuff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[2] = vdseMallocPages( pAlloc, 3, &context );
   if ( newBuff[2] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseFreePages( pAlloc, newBuff[1], 3, &context );
   vdseFreePages( pAlloc, newBuff[2], 3, &context );
   /* if the "unite" failed, no 6 pages free buffer should exist */
   newBuff[3] = vdseMallocPages( pAlloc, 6, &context );
   if ( newBuff[3] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   vdseFreePages( pAlloc, newBuff[0], 2, &context );
   vdseFreePages( pAlloc, newBuff[3], 6, &context );

   /* unite with both */
   newBuff[0] = vdseMallocPages( pAlloc, 1, &context );
   if ( newBuff[0] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[1] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[2] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[2] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[3] = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff[3] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   newBuff[4] = vdseMallocPages( pAlloc, 1, &context );
   if ( newBuff[4] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseFreePages( pAlloc, newBuff[1], 2, &context );
   vdseFreePages( pAlloc, newBuff[3], 2, &context );
   vdseFreePages( pAlloc, newBuff[2], 2, &context );
   /* if the "unite" failed, no 6 pages free buffer should exist */
   newBuff[1] = vdseMallocPages( pAlloc, 6, &context );
   if ( newBuff[1] == NULL )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   vdseFreePages( pAlloc, newBuff[0], 1, &context );
   vdseFreePages( pAlloc, newBuff[1], 6, &context );
   vdseFreePages( pAlloc, newBuff[4], 1, &context );

   if (pAlloc->totalAllocPages != 2 ) ERROR_EXIT( expectedToPass, NULL, ; );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

