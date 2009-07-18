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

#include "Nucleus/Hash.h"
#include "Nucleus/Tests/Hash/HashTest.h"

psonSessionContext context;
psonHash* pHash;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void setup_test()
{
   enum psoErrors errcode;
   
   pHash = initHashTest( &context );
   
   errcode = psonHashInit( pHash, g_memObjOffset, 12, &context );
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
   enum psoErrors errcode;
   char key[20];
   char data[20];
   int i;
   unsigned char * pData;
   psonHashItem* pItem = NULL;
   size_t bucket;
   bool ok;
   psonHashItem * pHashItem;
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      errcode = psonHashInsert( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                data,
                                strlen(data),
                                &pHashItem,
                                &context );
      assert_true( errcode == PSO_OK );
      if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
         expect_assert_failure( psonHashResize( pHash, NULL ) );
         break;
      }
   }
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_null_hash( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   enum psoErrors errcode;
   char key[20];
   char data[20];
   int i;
   unsigned char * pData;
   psonHashItem* pItem = NULL;
   size_t bucket;
   bool ok;
   psonHashItem * pHashItem;
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      errcode = psonHashInsert( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                data,
                                strlen(data),
                                &pHashItem,
                                &context );
      assert_true( errcode == PSO_OK );
      if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
         expect_assert_failure( psonHashResize( NULL, &context ) );
         break;
      }
   }
#endif
   return;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void test_pass( void ** state )
{
#if defined(PSO_UNIT_TESTS)
   enum psoErrors errcode;
   char key[20];
   char data[20];
   int i;
   unsigned char * pData;
   psonHashItem* pItem = NULL;
   size_t bucket;
   bool ok;
   psonHashItem * pHashItem;
   
   /* A loop to 500 with our low initial size will provoke 4 resizes. */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      errcode = psonHashInsert( pHash,
                                (unsigned char*)key,
                                strlen(key),
                                data,
                                strlen(data),
                                &pHashItem,
                                &context );
      assert_true( errcode == PSO_OK );
      if ( pHash->enumResize == PSON_HASH_TIME_TO_GROW ) {
         errcode = psonHashResize( pHash, &context );
         assert_true( errcode == PSO_OK );
         assert_false( pHash->enumResize == PSON_HASH_TIME_TO_GROW );
      }
   }
   
   /* 
    * Next step: test that we can reach all the data by doing a get,
    * than a delete and test shrinking at the same time.
    */
   for ( i = 0; i < 500; ++i ) {
      sprintf( key,  "My Key %d", i );
      sprintf( data, "My Data %d", i );
      
      ok = psonHashGet( pHash,
                        (unsigned char*)key,
                        strlen(key),
                        &pItem,
                        &bucket,
                        &context );
      assert_true( ok );
      GET_PTR( pData, pItem->dataOffset, unsigned char );
      assert_true( memcmp( data, pData, strlen(data) ) == 0 );
      assert_true( pItem->dataLength == strlen(data) );
      
      ok = psonHashDelWithKey( pHash,
                               (unsigned char*)key,
                               strlen(key),
                               &context );
      assert_true( ok );
      
      if ( pHash->enumResize == PSON_HASH_TIME_TO_SHRINK ) {
         errcode = psonHashResize( pHash, &context );
         assert_true( errcode == PSO_OK );
         assert_false( pHash->enumResize == PSON_HASH_TIME_TO_SHRINK );
      }
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
      unit_test_setup_teardown( test_null_hash,    setup_test, teardown_test ),
      unit_test_setup_teardown( test_pass,         setup_test, teardown_test )
   };

   rc = run_tests(tests);
   
#endif
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

