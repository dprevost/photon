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

#ifndef VDST_PROCESS_TEST_H
#define VDST_PROCESS_TEST_H

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Transaction.h"
#include "Engine/InitEngine.h"
#include "Engine/Process.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
vdscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the process
 * object.
 *
 * A reminder: process is a memory object - when it needs memory, 
 * is asks its internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
vdseProcess * initProcessTest( bool                testIsExpectedToSucceed,
                               vdseSessionContext* pContext )
{
   int errcode;
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdseTx* pTx;
   vdseProcess * process;
   size_t allocatedLength = VDSE_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   
   errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      fprintf( stderr, "Abnormal error at line %d in processTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   vdscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in processTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (vdseTx*)vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 1, pContext );
   if ( pTx == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in processTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   errcode = vdseTxInit( pTx, 1, pContext );
   if ( errcode != 0 ) 
   {
      fprintf( stderr, "Abnormal error at line %d in processTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the folder object */
   process = (vdseProcess *) vdseMallocBlocks( pAlloc, VDSE_ALLOC_ANY, 1, pContext );
   if ( process == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in processTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   
   return process;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDST_PROCESS_TEST_H */

