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

/*
 * The code in Hash has two paths for copying data. If the length of
 * both arrays is equal, it will NOT compute the hash values.
 *
 * Both paths must be checked which is why "test_pass" is repeated with
 * 2 different setup functions.
 */
 
#include "Nucleus/Hash.h"
#include "Nucleus/Tests/Hash/HashTest.h"

psonSessionContext context;
psonHash * pOldHash, * pNewHash;
char* key1 = "My Key 1";
char* key2 = "My Key 2";
char* data1 = "My Data 1";
char* data2 = "My Data 2";

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object holding two
 * psonHash objects that can be used to test the hash copy function.
 *
 * A reminder: when the hash needs memory, it first ask its 
 * owner, the memory object which can ask the global allocator.
 * All of these must be properly created to be able to test all
 * of the hash function calls.
 */
 
void initHashCopyTest( psonHash          ** ppOldHash,
                       psonHash          ** ppNewHash,
                       bool                 sameLength, /* array length of hash */
                       psonSessionContext * pContext )
{
   int errcode;
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psotObjDummy* pDummy1, * pDummy2;
   size_t allocatedLength = PSON_BLOCK_SIZE * 30;
   
   ok = psonInitEngine();
   assert( ok );
   memset(pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   assert( ptr != NULL );
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy objects + initialize + blockGroup */
   pDummy1 = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, pContext );
   assert( pDummy1 != NULL );
   errcode = psonMemObjectInit( &pDummy1->memObject, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy1->blockGroup,
                                2 );
   assert( errcode == PSO_OK );
   
   errcode = psonHashInit( &pDummy1->hashObj, 
                           SET_OFFSET(&pDummy1->memObject), 
                           10,
                           pContext );
   assert( errcode == 0 );
   *ppOldHash = &pDummy1->hashObj;

   pDummy2 = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, pContext );
   assert( pDummy2 != NULL );
   errcode = psonMemObjectInit( &pDummy2->memObject, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy2->blockGroup,
                                2 );
   assert( errcode == PSO_OK );

   if ( sameLength ) {
      errcode = psonHashInit( &pDummy2->hashObj, 
                              SET_OFFSET(&pDummy2->memObject), 
                              10,
                              pContext );
      assert( errcode == 0 );
   }
   else {
      errcode = psonHashInit( &pDummy2->hashObj, 
                              SET_OFFSET(&pDummy2->memObject), 
                              100,
                              pContext );
      assert( errcode == 0 );
   }
   
   *ppNewHash = &pDummy2->hashObj;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   enum psoErrors errcode;
   psonHashItem * pHashItem;
   
   initHashCopyTest( &pOldHash, &pNewHash, false, &context );
   
   errcode = psonHashInsert( pOldHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pHashItem,
                             &context );
   assert( errcode == PSO_OK );
   
   errcode = psonHashInsert( pOldHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &pHashItem,
                             &context );
   assert( errcode == PSO_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test_same_length()
{
   enum psoErrors errcode;
   psonHashItem * pHashItem;
   
   initHashCopyTest( &pOldHash, &pNewHash, true, &context );
   
   errcode = psonHashInsert( pOldHash,
                             (unsigned char*)key1,
                             strlen(key1),
                             data1,
                             strlen(data1),
                             &pHashItem,
                             &context );
   assert( errcode == PSO_OK );
   
   errcode = psonHashInsert( pOldHash,
                             (unsigned char*)key2,
                             strlen(key2),
                             data2,
                             strlen(data2),
                             &pHashItem,
                             &context );
   assert( errcode == PSO_OK );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void teardown_test()
{
   free( g_pBaseAddr );
   g_pBaseAddr = NULL;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_context( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashCopy( pOldHash,
                                        pNewHash,
                                        NULL ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_new_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashCopy( pOldHash,
                                        NULL,
                                        &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_old_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   expect_assert_failure( psonHashCopy( NULL,
                                        pNewHash,
                                        &context ) );
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   enum psoErrors errcode;
   unsigned char * pData = NULL;
   psonHashItem* pNewItem;
   size_t bucket = (size_t) -1;
   bool found;
   
   errcode = psonHashCopy( pOldHash,
                           pNewHash,
                           &context );
   assert_true( errcode == PSO_OK );

   found = psonHashGet( pNewHash,
                        (unsigned char*)key2,
                        strlen(key2),
                        &pNewItem,
                        &bucket,
                        &context );
   assert_true( found );
   GET_PTR( pData, pNewItem->dataOffset, unsigned char );
   assert_false( pData == NULL );
   assert_true( pNewItem->dataLength == strlen(data2) );
   assert_true( memcmp( pData, data2, pNewItem->dataLength) == 0 );

#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int main()
{
   int rc = 0;
#if defined(PSO_UNIT_TESTS)
   const UnitTest tests[] = {
      unit_test_setup_teardown( test_null_context,  setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_new_hash, setup_test, teardown_test ),
      unit_test_setup_teardown( test_null_old_hash, setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,          setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,
                                setup_test_same_length, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

