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
#include "Nucleus/Hash.h"
#include "Nucleus/InitEngine.h"

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

ptrdiff_t g_memObjOffset = PSON_NULL_OFFSET;

struct psotObjDummy
{
   struct psonMemObject memObject;
   struct psonHash      hashObj;
   /* Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;
};

typedef struct psotObjDummy psotObjDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object holding a
 * psonHash that can be used to test the hash functions.
 *
 * A reminder: when the hash needs memory, it first ask its 
 * owner, the memory object which can ask the global allocator.
 * All of these must be properly created to be able to test all
 * of the hash function calls.
 */
 
psonHash* initHashTest( psonSessionContext * pContext )
{
   int errcode;
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psotObjDummy* pDummy;
   size_t allocatedLength = PSON_BLOCK_SIZE * 12;
   
   ok = psonInitEngine();
   assert( ok );
   memset(pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   assert(ptr != NULL );
   
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

#endif /* HASH_TEST_COMMON_H */

