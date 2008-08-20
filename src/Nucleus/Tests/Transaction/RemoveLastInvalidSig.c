/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#include "txTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psnTx* pTx;
   psnSessionContext context;
   bool ok;
   ptrdiff_t parentOffset = 0x1010, childOffset = 0x0101;
   
   pTx = initTxTest( expectedToPass, &context );

   ok = psnTxInit( pTx, 1, &context );
   if ( ! ok ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnTxAddOps( pTx,
                      PSN_TX_ADD_DATA,
                      parentOffset, 
                      PSO_FOLDER,
                      childOffset,
                      PSO_FOLDER,
                      &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   pTx->signature = 0;
   psnTxRemoveLastOps( pTx, &context );
   
   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */