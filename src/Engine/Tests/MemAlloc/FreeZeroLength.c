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
   vdseSessionContext context;
   vdseMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = PAGESIZE*10;
   unsigned char* newBuff;
   int i;
   
   initTest( false );
   vdscInitErrorHandler( &context.errorHandler );
   
   ptr = malloc( allocatedLength );

   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + PAGESIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, &context );
   
   newBuff = vdseMallocPages( pAlloc, 2, &context );
   if ( newBuff == NULL ) return 0;

   vdseFreePages( pAlloc, newBuff, 0, &context );

   return 0;
}