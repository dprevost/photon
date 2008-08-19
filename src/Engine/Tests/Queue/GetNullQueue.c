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

#include "queueTest.h"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psnQueue * pQueue;
   psnSessionContext context;
   bool ok;
   psnTxStatus status;
   char * data = "My Data";
   psnQueueItem * pItem = NULL;
   vdsObjectDefinition def = { 
      VDS_QUEUE, 
      1, 
      { 0, 0, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   
   pQueue = initQueueTest( expectedToPass, &context );

   psnTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psnQueueInit( pQueue, 
                       0, 1, &status, 4, 
                       "Queue1", SET_OFFSET(pQueue), &def, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnQueueInsert( pQueue,
                         data,
                         8,
                         PSN_QUEUE_FIRST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnQueueInsert( pQueue,
                         data,
                         6,
                         PSN_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psnQueueGet( NULL,
                      VDS_FIRST,
                      &pItem,
                      20,
                      &context );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
