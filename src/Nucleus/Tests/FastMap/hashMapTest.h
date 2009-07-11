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

#ifndef PSOT_HASH_MAP_TEST_H
#define PSOT_HASH_MAP_TEST_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/Folder.h"
#include "Nucleus/FastMap.h"
#include <photon/psoPhotonODBC.h>

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

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
 
psonFastMap * 
initHashMapTest( psonSessionContext * pContext )
{
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psonTx* pTx;
   psonFastMap* pHashMap;
   size_t allocatedLength = PSON_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psonTx*)psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   ok = psonTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the hash map object */
   pHashMap = (psonFastMap*)psonMallocBlocks( pAlloc, PSON_ALLOC_API_OBJ, 1, pContext );
   if ( pHashMap == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
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
initHashMapCopyTest( psonFastMap       ** ppOldMap,
                     psonFastMap       ** ppNewMap,
                     psonSessionContext * pContext )
{
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psonTx* pTx;
   psonFastMap* pHashMap;
   size_t allocatedLength = PSON_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   if ( ok != true ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psonTx*)psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   ok = psonTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the hash map object */
   pHashMap = (psonFastMap*)psonMallocBlocks( pAlloc, PSON_ALLOC_API_OBJ, 1, pContext );
   if ( pHashMap == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   *ppOldMap = pHashMap;
   
   /* Allocate memory for the hash map object */
   pHashMap = (psonFastMap*)psonMallocBlocks( pAlloc, PSON_ALLOC_API_OBJ, 1, pContext );
   if ( pHashMap == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in hashMapTest.h\n", __LINE__ );
      exit(1);
   }
   *ppNewMap = pHashMap;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOT_HASH_MAP_TEST_H */

