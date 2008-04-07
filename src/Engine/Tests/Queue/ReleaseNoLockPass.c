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
/*
 * The pragma is to remove this type of message:
 * warning C4273: 'vdseQueueInit' : inconsistent dll linkage.  dllexport assumed.
 *
 * [These warnings are caused by the direct inclusion of the .c file]
 */
#if defined(WIN32)
#  pragma warning(disable:4273)
#endif
#include "Engine/Queue.c"

const bool expectedToPass = true;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   vdseQueue * pQueue;
   vdseSessionContext context;
   int errcode;
   vdseTxStatus status;
   char * data = "My Data";
   vdseQueueItem * pItem = NULL;
   
   pQueue = initQueueTest( expectedToPass, &context );

   vdseTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseQueueInit( pQueue, 
                            0, 1, &status, 4, 
                            strCheck("Map1"), NULL_OFFSET, &context );
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
   
   errcode = vdseQueueInsert( pQueue,
                              data,
                              6,
                              VDSE_QUEUE_LAST,
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
   
   errcode = vdseQueueGet( pQueue,
                           VDS_NEXT,
                           &pItem,
                           20,
                           &context );
   if ( errcode != 0 ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->dataLength != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   vdseQueueReleaseNoLock( pQueue,
                           pItem,
                           &context );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
