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

#ifndef MEM_OBJ_TEST_COMMON_H
#define MEM_OBJ_TEST_COMMON_H

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/InitEngine.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
vdscErrMsgHandle g_vdsErrorHandle;

typedef struct vdstObjDummy
{
   struct vdseMemObject memObject;
   /* Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;
} vdstObjDummy;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes a dummy Memory Object to be used
 * for testing this object interface.
 */
 
vdstObjDummy* initMemObjTest( bool testIsExpectedToSucceed,
                              vdseSessionContext* pContext )
{
   int errcode;
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdstObjDummy* pDummy;
   size_t allocatedLength = VDSE_BLOCK_SIZE * 10;
   
   memset( pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   vdscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_BLOCK_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy object + initialize it + blockGroup */
   pDummy = (vdstObjDummy*) vdseMallocBlocks( pAlloc, VDSE_ALLOC_API_OBJ, 4, pContext );
   if ( pDummy == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in MemObjTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }

   return pDummy;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* MEM_OBJ_TEST_COMMON_H */
