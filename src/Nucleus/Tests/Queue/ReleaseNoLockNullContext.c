/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
 *
 * This file may be distributed and/or modified under the terms of the
 * GNU General Public License version 2 or version 3 as published by the 
 * Free Software Foundation and appearing in the file COPYING.GPL2 and 
 * COPYING.GPL3 included in the packaging of this software.
 *
 * Licensees holding a valid Photon Commercial license can use this file 
 * in accordance with the terms of their license.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "queueTest.h"
/*
 * The pragma is to remove this type of message:
 * warning C4273: 'psonQueueInit' : inconsistent dll linkage.  dllexport assumed.
 *
 * [These warnings are caused by the direct inclusion of the .c file]
 */
#if defined(WIN32)
#  pragma warning(disable:4273)
#endif
#include "Nucleus/Queue.c"

const bool expectedToPass = false;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
#if defined(USE_DBC)
   psonQueue * pQueue;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   char * data = "My Data";
   psonQueueItem * pItem = NULL;
   psoObjectDefinition def = { PSO_QUEUE, PSO_DEF_USER_DEFINED, PSO_DEF_NONE };
   const unsigned char * fields =  (unsigned char *)"A dummy definition";
   
   pQueue = initQueueTest( expectedToPass, &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonQueueInit( pQueue, 
                       0, 1, &status, 6, 
                       "Queue1", SET_OFFSET(pQueue), 
                       &def, fields, sizeof(fields), &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonQueueInsert( pQueue,
                         data,
                         8,
                         PSON_QUEUE_FIRST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonQueueInsert( pQueue,
                         data,
                         6,
                         PSON_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonQueueGet( pQueue,
                      PSO_FIRST,
                      &pItem,
                      20,
                      &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->dataLength != 8 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   ok = psonQueueGet( pQueue,
                      PSO_NEXT,
                      &pItem,
                      20,
                      &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pItem->dataLength != 6 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
   psonQueueReleaseNoLock( pQueue,
                      pItem,
                      NULL );

   ERROR_EXIT( expectedToPass, NULL, ; );
#else
   return 1;
#endif
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
