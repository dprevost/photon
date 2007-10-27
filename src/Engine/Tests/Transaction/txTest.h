/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

VDSF_ENGINE_EXPORT
vdscErrMsgHandle g_vdsErrorHandle;

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
 
vdseTx* initTxTest( bool                testIsExpectedToSucceed,
                    vdseSessionContext* pContext )
{
   int errcode;
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdseTx* pDummy;
   size_t allocatedLength = VDSE_BLOCK_SIZE * 10;

   memset( pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   
   errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      fprintf( stderr, "Abnormal error at line %d in txTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   vdscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in txTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object */
   pDummy = (vdseTx*) vdseMallocBlocks( pAlloc, 1, pContext );
   if ( pDummy == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in txTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   
   return pDummy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* TX_TEST_COMMON_H */
