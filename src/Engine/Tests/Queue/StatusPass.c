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
   int errcode;
   vdseTxStatus txStatus;
   vdsObjStatus status;
   char * data = "My Data";

   pQueue = initQueueTest( expectedToPass, &context );

   vdseTxStatusInit( &txStatus, SET_OFFSET( context.pTransaction ) );
   
   errcode = vdseQueueInit( pQueue, 
                            0, 1, &txStatus, 4, 
                            strCheck("Map1"), NULL_OFFSET, &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   
   errcode = vdseQueueInsert( pQueue,
                              data,
                              8,
                              VDSE_QUEUE_FIRST,
                              &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   errcode = vdseQueueInsert( pQueue,
                              data,
                              8,
                              VDSE_QUEUE_FIRST,
                              &context );
   if ( errcode != 0 ) 
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );

   vdseQueueStatus( pQueue, &status );

   if ( status.numDataItem != 2 )
      ERROR_EXIT( expectedToPass, NULL, ; );

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
