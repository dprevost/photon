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

psonQueue * pQueue;
psonSessionContext context;
psonTxStatus status;
psonQueueItem * pItem = NULL;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;
   psoObjectDefinition def = { PSO_QUEUE, 0, 0, 0 };
   psonDataDefinition fields;
   char * data = "My Data";
   
   pQueue = initQueueTest( &context );

   psonTxStatusInit( &status, SET_OFFSET( context.pTransaction ) );

   ok = psonQueueInit( pQueue, 
                       0, 1, &status, 6, 
                       "Queue1", SET_OFFSET(pQueue), 
                       &def, &fields, &context );
   assert( ok );
   
   ok = psonQueueInsert( pQueue,
                         data,
                         8,
                         NULL,
                         PSON_QUEUE_FIRST,
                         &context );
   assert( ok );
   
   ok = psonQueueInsert( pQueue,
                         data,
                         6,
                         NULL,
                         PSON_QUEUE_LAST,
                         &context );
   assert( ok );
   
   ok = psonQueueGetFirst( pQueue,
                           &pItem,
                           20,
                           &context );
   assert( ok );
   
   ok = psonQueueGetNext( pQueue,
                          &pItem,
                          20,
                          &context );
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
   expect_assert_failure( psonQueueReleaseNoLock( pQueue,
                                                  pItem,
                                                  NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_item( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueReleaseNoLock( pQueue,
                                                  NULL,
                                                  &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_queue( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonQueueReleaseNoLock( NULL,
                                                  pItem,
                                                  &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   psonTxStatus * txItemStatus;
   
   assert_true( pItem->dataLength == 6 );
   assert_true( status.usageCounter == 1 );
   txItemStatus = &pItem->txStatus;
   assert_true( txItemStatus->usageCounter == 1 );
   
   psonQueueReleaseNoLock( pQueue,
                           pItem,
                           &context );
   assert_true( txItemStatus->usageCounter == 0 );
   assert_true( status.usageCounter == 0 );
   assert_true( pQueue->nodeObject.txCounter == 2 );
   
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
      unit_test_setup_teardown( test_null_item,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_queue,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

