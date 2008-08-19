/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of photon (photonsoftware.org).
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

#ifndef VDST_HASH_MAP_TEST_H
#define VDST_HASH_MAP_TEST_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/Folder.h"
#include "Nucleus/Map.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
pscErrMsgHandle g_psoErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the hash map
 * object.
 *
 * A reminder: hash maps are memory objects - when they need memory, 
 * they ask it from their internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psnMap * 
initHashMapTest( bool                testIsExpectedToSucceed,
                 psnSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psnMemAlloc*  pAlloc;
   psnTx* pTx;
   psnMap* pHashMap;
   size_t allocatedLength = PSN_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psnInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psnTx*)psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   ok = psnTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the hash map object */
   pHashMap = (psnMap*)psnMallocBlocks( pAlloc, PSN_ALLOC_API_OBJ, 1, pContext );
   if ( pHashMap == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   return pHashMap;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the hash map
 * object.
 *
 * A reminder: hash maps are memory objects - when they need memory, 
 * they ask it from their internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
void
initHashMapCopyTest( bool                 testIsExpectedToSucceed,
                     psnMap           ** ppOldMap,
                     psnMap           ** ppNewMap,
                     psnSessionContext * pContext )
{
   bool ok;
   unsigned char* ptr;
   psnMemAlloc*  pAlloc;
   psnTx* pTx;
   psnMap* pHashMap;
   size_t allocatedLength = PSN_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psnInitEngine();
   if ( ok != true ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psnTx*)psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   ok = psnTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the hash map object */
   pHashMap = (psnMap*)psnMallocBlocks( pAlloc, PSN_ALLOC_API_OBJ, 1, pContext );
   if ( pHashMap == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   *ppOldMap = pHashMap;
   
   /* Allocate memory for the hash map object */
   pHashMap = (psnMap*)psnMallocBlocks( pAlloc, PSN_ALLOC_API_OBJ, 1, pContext );
   if ( pHashMap == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   *ppNewMap = pHashMap;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDST_HASH_MAP_TEST_H */

