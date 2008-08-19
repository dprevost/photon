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

#ifndef MEM_OBJ_TEST_COMMON_H
#define MEM_OBJ_TEST_COMMON_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/InitEngine.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
pscErrMsgHandle g_psoErrorHandle;

struct psotObjDummy
{
   struct psnMemObject memObject;
   /* Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;
};

typedef struct psotObjDummy psotObjDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object to be used
 * for testing this object interface.
 */
 
psotObjDummy* initMemObjTest( bool testIsExpectedToSucceed,
                              psnSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psnMemAlloc*  pAlloc;
   psotObjDummy* pDummy;
   size_t allocatedLength = PSN_BLOCK_SIZE * 10;
   
   memset( pContext, 0, sizeof(psnSessionContext) );
   pContext->pidLocker = getpid();
   ok = psnInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   pscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psnMemAlloc*)(g_pBaseAddr + PSN_BLOCK_SIZE);
   psnMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy object + initialize it + blockGroup */
   pDummy = (psotObjDummy*) psnMallocBlocks( pAlloc, PSN_ALLOC_API_OBJ, 4, pContext );
   if ( pDummy == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }

   return pDummy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* MEM_OBJ_TEST_COMMON_H */

