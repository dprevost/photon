/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Tests/EngineTestCommon.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psonMemAlloc*     pAlloc;
   unsigned char* ptr;
   size_t allocatedLength = PSON_BLOCK_SIZE*10;
   psonSessionContext context;
   
   initTest( expectedToPass, &context );
   
   ptr = malloc( allocatedLength );
   if ( ptr == NULL ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, &context );

   psonMemAllocClose( pAlloc, &context );
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

