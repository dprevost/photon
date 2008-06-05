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
   vdseQueue * pQueue;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   char * data = "My Data";
   vdseQueueItem * pItem = NULL;
   struct vdseFieldDef def[1] = { { "Field_1", 4, 10, VDSE_VAR_STRING } };
   
   pQueue = initQueueTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseQueueInit( pQueue, 
                            0, 1, &status, 4, 
                            "Map1", NULL_OFFSET, 
                            def, 1, &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueInsert( pQueue,
                              data,
                              8,
                              VDSE_QUEUE_FIRST,
                              &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   errcode = vdseQueueGet( pQueue,
                           VDS_FIRST,
                           &pItem,
                           20,
                           &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->dataLength != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   errcode = vdseQueueRelease( pQueue,
                               pItem,
                               &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   vdseQueueCommitAdd( pQueue, NULL_OFFSET );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
