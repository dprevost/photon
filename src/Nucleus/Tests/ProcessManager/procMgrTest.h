/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef VDST_PROC_MGR_TEST_H
#define VDST_PROC_MGR_TEST_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/ProcessManager.h"
#include "Tests/PrintError.h"

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the process
 * manager object.
 *
 * A reminder: procMgr is a memory object - when it needs memory, 
 * is asks its internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psonProcMgr* initProcMgrTest( bool                testIsExpectedToSucceed,
                              psonSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psonTx* pTx;
   psonProcMgr* pManager;
   size_t allocatedLength = PSON_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in procMgrTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in procMgrTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psonTx*)psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in procMgrTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   ok = psonTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in procMgrTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the folder object */
   pManager = (psonProcMgr *) psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   if ( pManager == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in procMgrTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   return pManager;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDST_PROC_MGR_TEST_H */

