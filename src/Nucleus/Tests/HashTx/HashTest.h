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
#include "Tests/PrintError.h"

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
 
psonHashTx* initHashTest( bool testIsExpectedToSucceed,
                        psonSessionContext* pContext )
{
   int errcode;
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psotObjDummy* pDummy;
   size_t allocatedLength = PSON_BLOCK_SIZE * 12;
   
   ok = psonInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   memset(pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy object + initialize it + blockGroup */
   pDummy = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, pContext );
   if ( pDummy == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   errcode = psonMemObjectInit( &pDummy->memObject, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
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
 
void initHashCopyTest( bool                 testIsExpectedToSucceed,
                       psonHashTx          ** ppOldHash,
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
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   memset(pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy objects + initialize + blockGroup */
   pDummy1 = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, pContext );
   if ( pDummy1 == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   errcode = psonMemObjectInit( &pDummy1->memObject, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy1->blockGroup,
                                2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   errcode = psonHashTxInit( &pDummy1->hashObj, 
                           SET_OFFSET(&pDummy1->memObject), 
                           10,
                           pContext );
   if ( errcode != 0 ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   *ppOldHash = &pDummy1->hashObj;

   pDummy2 = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 2, pContext );
   if ( pDummy2 == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   errcode = psonMemObjectInit( &pDummy2->memObject, 
                                PSON_IDENT_ALLOCATOR,
                                &pDummy2->blockGroup,
                                2 );
   if ( errcode != PSO_OK ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }

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
   if ( errcode != 0 ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   *ppNewHash = &pDummy2->hashObj;
   
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* HASH_TEST_COMMON_H */

