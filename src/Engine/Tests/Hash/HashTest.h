/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of vdsf (Virtual Data Space Framework).
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

#ifndef HASH_TEST_COMMON_H
#define HASH_TEST_COMMON_H

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Hash.h"
#include "Engine/InitEngine.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
pscErrMsgHandle g_vdsErrorHandle;

ptrdiff_t g_memObjOffset = VDSE_NULL_OFFSET;

struct vdstObjDummy
{
   struct vdseMemObject memObject;
   struct vdseHash      hashObj;
   /* Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;
};

typedef struct vdstObjDummy vdstObjDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object holding a
 * vdseHash that can be used to test the hash functions.
 *
 * A reminder: when the hash needs memory, it first ask its 
 * owner, the memory object which can ask the global allocator.
 * All of these must be properly created to be able to test all
 * of the hash function calls.
 */
 
vdseHash* initHashTest( bool testIsExpectedToSucceed,
                        vdseSessionContext* pContext )
{
   int errcode;
   bool ok;
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdstObjDummy* pDummy;
   size_t allocatedLength = VDSE_BLOCK_SIZE * 10;
   
   ok = vdseInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   memset(pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy object + initialize it + blockGroup */
   pDummy = (vdstObjDummy*) vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 2, pContext );
   if ( pDummy == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   errcode = vdseMemObjectInit( &pDummy->memObject, 
                                VDSE_IDENT_ALLOCATOR,
                                &pDummy->blockGroup,
                                2 );
   if ( errcode != VDS_OK ) {
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
 * vdseHash objects that can be used to test the hash copy function.
 *
 * A reminder: when the hash needs memory, it first ask its 
 * owner, the memory object which can ask the global allocator.
 * All of these must be properly created to be able to test all
 * of the hash function calls.
 */
 
void initHashCopyTest( bool                 testIsExpectedToSucceed,
                       vdseHash          ** ppOldHash,
                       vdseHash          ** ppNewHash,
                       bool                 sameLength, /* array length of hash */
                       vdseSessionContext * pContext )
{
   int errcode;
   bool ok;
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdstObjDummy* pDummy1, * pDummy2;
   size_t allocatedLength = VDSE_BLOCK_SIZE * 30;
   
   ok = vdseInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   memset(pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy objects + initialize + blockGroup */
   pDummy1 = (vdstObjDummy*) vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 2, pContext );
   if ( pDummy1 == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   errcode = vdseMemObjectInit( &pDummy1->memObject, 
                                VDSE_IDENT_ALLOCATOR,
                                &pDummy1->blockGroup,
                                2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   errcode = vdseHashInit( &pDummy1->hashObj, 
                           SET_OFFSET(&pDummy1->memObject), 
                           10,
                           pContext );
   if ( errcode != 0 ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   *ppOldHash = &pDummy1->hashObj;

   pDummy2 = (vdstObjDummy*) vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 2, pContext );
   if ( pDummy2 == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   errcode = vdseMemObjectInit( &pDummy2->memObject, 
                                VDSE_IDENT_ALLOCATOR,
                                &pDummy2->blockGroup,
                                2 );
   if ( errcode != VDS_OK ) {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }

   if ( sameLength ) {
      errcode = vdseHashInit( &pDummy2->hashObj, 
                              SET_OFFSET(&pDummy2->memObject), 
                              10,
                              pContext );
   }
   else {
      errcode = vdseHashInit( &pDummy2->hashObj, 
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

