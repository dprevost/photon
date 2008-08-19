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

#ifndef VDST_SESSION_TEST_H
#define VDST_SESSION_TEST_H

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Transaction.h"
#include "Engine/InitEngine.h"
#include "Engine/Session.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
pscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the session
 * object.
 *
 * A reminder: session is a memory object - when it needs memory, 
 * is asks its internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psnSession * initSessionTest( bool                testIsExpectedToSucceed,
                               psnSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psnMemAlloc*  pAlloc;
   psnTx* pTx;
   psnSession * pSession;
   size_t allocatedLength = PSN_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psnInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in sessionTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in sessionTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psnTx*)psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in sessionTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   ok = psnTxInit( pTx, 1, pContext );
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in sessionTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the folder object */
   pSession = (psnSession *) psnMallocBlocks( pAlloc, PSN_ALLOC_ANY, 1, pContext );
   if ( pSession == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in sessionTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   
   return pSession;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDST_SESSION_TEST_H */

