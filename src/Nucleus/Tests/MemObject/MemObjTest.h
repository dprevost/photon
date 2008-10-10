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

#ifndef MEM_OBJ_TEST_COMMON_H
#define MEM_OBJ_TEST_COMMON_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/InitEngine.h"
#include "Tests/PrintError.h"

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

struct psotObjDummy
{
   struct psonMemObject memObject;
   /* Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;
};

typedef struct psotObjDummy psotObjDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object to be used
 * for testing this object interface.
 */
 
psotObjDummy* initMemObjTest( bool testIsExpectedToSucceed,
                              psonSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psotObjDummy* pDummy;
   size_t allocatedLength = PSON_BLOCK_SIZE * 10;
   
   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   ok = psonInitEngine();
   if ( ! ok ) {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy object + initialize it + blockGroup */
   pDummy = (psotObjDummy*) psonMallocBlocks( pAlloc, PSON_ALLOC_API_OBJ, 4, pContext );
   if ( pDummy == NULL ) {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed ) exit(1);
      exit(0);
   }

   return pDummy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* MEM_OBJ_TEST_COMMON_H */

