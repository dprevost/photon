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

#ifndef TX_TEST_COMMON_H
#define TX_TEST_COMMON_H

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Transaction.h"
#include "Engine/InitEngine.h"
#include "Tests/PrintError.h"
#include "Engine/Folder.h"

VDSF_ENGINE_EXPORT
pscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the transaction
 * object.
 *
 * A reminder: transactions are memory objects - when they need memory, 
 * they ask it from their internalmemObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object isnot initialized however - so that we can test
 * the Init() call.
 */
 
psnTx* initTxTest( bool                testIsExpectedToSucceed,
                    psnSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psnMemAlloc*  pAlloc;
   psnTx* pDummy;
   size_t allocatedLength = PSN_BLOCK_SIZE * 10;

   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psnInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in txTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in txTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object */
   pDummy = (psnTx*) psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pDummy == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in txTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   return pDummy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/**
 * This function initializes the memory needed by the folder
 * object.
 *
 * A reminder: folders are memory objects - when they need memory, 
 * they ask it from their internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psnFolder* initFolderTest( bool                testIsExpectedToSucceed,
                            psnSessionContext* pContext )
{
   unsigned char* ptr;
   psnMemAlloc*  pAlloc;
   psnTx* pTx;
   psnFolder* pFolder;
   size_t allocatedLength = PSN_BLOCK_SIZE * 25;
   bool ok;

   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psnInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psnTx*)psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   ok = psnTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the folder object */
   pFolder = (psnFolder*)psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pFolder == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   return pFolder;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* TX_TEST_COMMON_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

