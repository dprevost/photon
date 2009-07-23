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

#ifndef HASH_TEST_COMMON_H
#define HASH_TEST_COMMON_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/HashTx.h"
#include "Nucleus/InitEngine.h"

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

ptrdiff_t g_memObjOffset = PSON_NULL_OFFSET;

struct psotObjDummy
{
   struct psonMemObject memObject;
   struct psonHashTx      hashObj;
   /* Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;
};

typedef struct psotObjDummy psotObjDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object holding a
 * psonHashTx that can be used to test the hash functions.
 *
 * A reminder: when the hash needs memory, it first ask its 
 * owner, the memory object which can ask the global allocator.
 * All of these must be properly created to be able to test all
 * of the hash function calls.
 */
 
psonHashTx* initHashTest( psonSessionContext * pContext )
{
   int errcode;
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psotObjDummy* pDummy;
   size_t allocatedLength = PSON_BLOCK_SIZE * 15;
   
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
   
   /* Allocate memory for our dummy object + initialize it + blockGroup */
   pDummy = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, pContext );
   assert( pDummy != NULL );
   errcode = psonMemObjectInit( &pDummy->memObject, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                2 );
   assert( errcode == PSO_OK );
   g_memObjOffset = SET_OFFSET(&pDummy->memObject);
   
   /*
    * We do not initialize hash - otherwise we would not be able
    * to test the init call.
    */
   return &pDummy->hashObj;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/**
 * This function initializes a dummy Memory Object holding two
 * psonHashTx objects that can be used to test the hash copy function.
 *
 * A reminder: when the hash needs memory, it first ask its 
 * owner, the memory object which can ask the global allocator.
 * All of these must be properly created to be able to test all
 * of the hash function calls.
 */
 
void initHashCopyTest( psonHashTx          ** ppOldHash,
                       psonHashTx          ** ppNewHash,
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
   
   errcode = psonHashTxInit( &pDummy1->hashObj, 
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
      errcode = psonHashTxInit( &pDummy2->hashObj, 
                              SET_OFFSET(&pDummy2->memObject), 
                              10,
                              pContext );
   }
   else {
      errcode = psonHashTxInit( &pDummy2->hashObj, 
                              SET_OFFSET(&pDummy2->memObject), 
                              100,
                              pContext );
   }
   assert( errcode == 0 );
   *ppNewHash = &pDummy2->hashObj;
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* HASH_TEST_COMMON_H */

