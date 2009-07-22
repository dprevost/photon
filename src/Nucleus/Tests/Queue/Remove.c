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

const bool expectedToPass = true;
psonQueue * pQueue;
psonSessionContext context;
psonTxStatus status;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psonDataDefinition fields;
   
   pQueue = initQueueTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );

   ok = psonQueueInit( pQueue, 
                       0, 1, &status, 6, 
                       "Queue1", SET_OFFSET(pQueue), 
                       &def, &fields, &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure(  );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psonQueue * pQueue;
   psonSessionContext context;
   bool ok;
   psonTxStatus status;
   char * data = "My Data";
   psonQueueItem * pQueueItem;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psonDataDefinition fields;
   
   pQueue = initQueueTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );
   
   ok = psonQueueInit( pQueue, 
                       0, 1, &status, 6, 
                       "Queue1", SET_OFFSET(pQueue), 
                       &def, &fields, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonQueueInsert( pQueue,
                         data,
                         8,
                         NULL,
                         PSON_QUEUE_FIRST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   ok = psonQueueInsert( pQueue,
                         data,
                         6,
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   
   /* Must commit the insert before we attempt to remove */
   ok = psonQueueGetFirst( pQueue, &pQueueItem, 100, &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   psonQueueCommitAdd( pQueue, SET_OFFSET(pQueueItem) );

   ok = psonQueueRemove( pQueue,
                         &pQueueItem,
                         PSON_QUEUE_FIRST,
                         8,
                         &context );
   if ( ok != true ) {
      ERROR_EXIT( expectedToPass, &context.errorHandler, ; );
   }
   if ( pQueue->nodeObject.txCounter != 2 ) {
      ERROR_EXIT( expectedToPass, NULL, ; );
   }
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

