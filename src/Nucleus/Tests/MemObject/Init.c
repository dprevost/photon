/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "Nucleus/MemoryObject.h"
#include "MemObjTest.h"

psonMemObject* pObj;
psotObjDummy  *pDummy;
psonSessionContext context;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   psoErrors errcode;
   
   pDummy = initMemObjTest( &context );
   pObj = &pDummy->memObject;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   if ( g_pBaseAddr ) free(g_pBaseAddr);
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_invalid_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemObjectInit( pObj, 
                                             (psonMemObjIdent)(PSON_IDENT_LAST + 200),
                                             &pDummy->blockGroup,
                                             4 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_group( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemObjectInit( pObj, 
                                             PSON_IDENT_ALLOCATOR,
                                             NULL,
                                             4 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_obj( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemObjectInit( NULL, 
                                             PSON_IDENT_ALLOCATOR,
                                             &pDummy->blockGroup,
                                             4 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_pages( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemObjectInit( pObj, 
                                             PSON_IDENT_ALLOCATOR,
                                             &pDummy->blockGroup,
                                             0 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_zero_type( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonMemObjectInit( pObj, 
                                             (psonMemObjIdent)0,
                                             &pDummy->blockGroup,
                                             4 ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   psoErrors errcode;
   
   errcode = psonMemObjectInit( pObj, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                4 );
   assert_true( errcode == PSO_OK );
   assert_true( pObj->objType == PSON_IDENT_ALLOCATOR );
   assert_true( pObj->totalBlocks == 4 );
   
   psonMemObjectFini( pObj, PSON_ALLOC_ANY, &context );
   
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_invalid_type, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_group,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_obj,     setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_pages,   setup_test, teardown_test ),
      unit_test_setup_teardown( test_zero_type,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

