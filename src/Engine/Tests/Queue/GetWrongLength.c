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

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseQueue * pQueue;
   vdseSessionContext context;
   bool ok;
   int errcode;
   vdseTxStatus status;
   char * data = "My Data";
   vdseQueueItem * pItem = NULL;
   vdsObjectDefinition def = { 
      VDS_QUEUE, 
      1, 
      { 0, 0, 0, 0}, 
      { { "Field_1", VDS_VAR_STRING, 0, 4, 10, 0, 0 } } 
   };
   
   pQueue = initQueueTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = vdseQueueInit( pQueue, 
                       0, 1, &status, 4, 
                       "Queue1", SET_OFFSET(pQueue), &def, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseQueueInsert( pQueue,
                         data,
                         8,
                         VDSE_QUEUE_FIRST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseQueueInsert( pQueue,
                         data,
                         6,
                         VDSE_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = vdseQueueGet( pQueue,
                      VDS_FIRST,
                      &pItem,
                      7,
                      &context );
   if ( ok == true ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   errcode = pscGetLastError( &context.errorHandler );
   if ( errcode != VDS_INVALID_LENGTH ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
