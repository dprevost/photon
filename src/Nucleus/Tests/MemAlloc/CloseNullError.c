/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psnMemAlloc* pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = PSN_BLOCK_SIZE*10;
   psnSessionContext context;
   
   initTest( expectedToPass, &context );
   
   ptr = malloc( allocatedLength );
   if ( ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, &context );

   psnMemAllocClose( pAlloc, NULL );
   
   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

