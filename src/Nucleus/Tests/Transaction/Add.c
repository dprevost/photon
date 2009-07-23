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

#include "txTest.h"

psonTx * pTx;
psonSessionContext context;
ptrdiff_t parentOffset = 0x1010, childOffset = 0x0101;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   bool ok;

   pTx = initTxTest( &context );

   ok = psonTxInit( pTx, 1, &context );
   assert( ok );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if (g_pBaseAddr) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_sig( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   int value;
   
   value = pTx->signature;
   pTx->signature = 0;
   expect_assert_failure( psonTxAddOps( pTx,
                                        PSON_TX_ADD_DATA,
                                        parentOffset, 
                                        PSO_FOLDER,
                                        childOffset,
                                        PSO_FOLDER,
                                        &context ) );
   pTx->signature = value;
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_child( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTxAddOps( pTx,
                                        PSON_TX_ADD_DATA,
                                        parentOffset, 
                                        PSO_FOLDER,
                                        PSON_NULL_OFFSET,
                                        PSO_FOLDER,
                                        &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTxAddOps( pTx,
                                        PSON_TX_ADD_DATA,
                                        parentOffset, 
                                        PSO_FOLDER,
                                        childOffset,
                                        PSO_FOLDER,
                                        NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_parent( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTxAddOps( pTx,
                                        PSON_TX_ADD_DATA,
                                        PSON_NULL_OFFSET,
                                        PSO_FOLDER,
                                        childOffset,
                                        PSO_FOLDER,
                                        &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_tx( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonTxAddOps( NULL,
                                        PSON_TX_ADD_DATA,
                                        parentOffset, 
                                        PSO_FOLDER,
                                        childOffset,
                                        PSO_FOLDER,
                                        &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   bool ok;
   
   ok = psonTxAddOps( pTx,
                      PSON_TX_ADD_DATA,
                      parentOffset, 
                      PSO_FOLDER,
                      childOffset,
                      PSO_FOLDER,
                      &context );
   assert_true( ok );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_sig,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_child,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_context, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_parent,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_tx,      setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

