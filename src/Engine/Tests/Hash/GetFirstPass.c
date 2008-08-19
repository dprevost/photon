/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Engine/Hash.h"
#include "Engine/Tests/Hash/HashTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnSessionContext context;
   psnHash* pHash;
   enum vdsErrors errcode;
   char* key1 = "My Key 1";
   char* key2 = "My Key 2";
   char* data1 = "My Data 1";
   char* data2 = "My Data 2";
   ptrdiff_t offsetFirstItem = PSN_NULL_OFFSET;
   psnHashItem* pNewItem;
   bool found;
   
   pHash = initHashTest( expectedToPass, &context );
   
   errcode = psnHashInit( pHash, g_memObjOffset, 100, &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = psnHashInsert( pHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pNewItem,
                             &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   errcode = psnHashInsert( pHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &pNewItem,
                             &context );
   if ( errcode != VDS_OK ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   found = psnHashGetFirst( pHash, &offsetFirstItem );
   if ( ! found ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( offsetFirstItem == PSN_NULL_OFFSET ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
