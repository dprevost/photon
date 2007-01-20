/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Hash.h"
#include "HashTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseSessionContext context;
   vdseHash* pHash;
   enum ListErrors listErr;
   char* key = "My Key";
   char* data1 = "My Data 1";
   ptrdiff_t offsetOfNewItem;
   
   pHash = initHashTest( expectedToPass,
                         &context );
   
   listErr = vdseHashInit( pHash, 100, &context );
   if ( listErr != LIST_OK )
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   listErr = vdseHashInsert( pHash,
                             (unsigned char*)key,
                             strlen(key),
                             data1,
                             0,
                             &offsetOfNewItem,
                             &context );
                             
   ERROR_EXIT( expectedToPass, NULL, ; );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
