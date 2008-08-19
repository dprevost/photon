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

#include "procMgrTest.h"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   psnProcMgr* pManager;
   psnSessionContext context;
   psnProcess *process;
   bool ok;
   
   pManager = initProcMgrTest( expectedToPass, &context );

   ok = psnProcMgrInit( pManager, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnProcMgrAddProcess( pManager, 
                               12345, 
                               &process,
                               &context );

   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnProcMgrFindProcess( pManager, 
                                12345, 
                                &process,
                                &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnProcMgrFindProcess( pManager, 
                                12378,
                                &process,
                                &context );
   if ( ok == true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   if ( pscGetLastError( &context.errorHandler ) != VDS_INTERNAL_ERROR ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

