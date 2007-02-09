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

#ifndef HASH_TEST_COMMON_H
#define HASH_TEST_COMMON_H

#include "Engine.h"
#include "MemoryObject.h"
#include "PageGroup.h"
#include "MemoryAllocator.h"
#include "Hash.h"
#include "InitEngine.h"
#include "PrintError.h"

vdscErrMsgHandle g_vdsErrorHandle;

typedef struct vdstObjDummy
{
   struct vdseMemObject memObject;
   struct vdseHash      hashObj;
   /* Variable size struct - always put at the end */
   struct vdsePageGroup pageGroup;
} vdstObjDummy;

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
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdstObjDummy* pDummy;
   size_t allocatedLength = VDSE_PAGE_SIZE * 10;
   errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   vdscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   g_pBaseAddr = ptr;
   pAlloc = (vdseMemAlloc*)(g_pBaseAddr + VDSE_PAGE_SIZE);
   vdseMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for our dummy object + initialize it + pageGroup */
   pDummy = vdseMallocPages( pAlloc, 2, pContext );
   if ( pDummy == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   errcode = vdseMemObjectInit( &pDummy->memObject, 
                                VDSE_IDENT_ALLOCATOR,
                                2 );
   if ( errcode != VDS_OK )
   {
      fprintf( stderr, "Abnormal error at line %d in HashTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   pContext->pCurrentMemObject = pDummy;
   
   vdsePageGroupInit( &pDummy->pageGroup,
                      2*VDSE_PAGE_SIZE, /* offset */
                      2 ); /* Number of pages */

   /*
    * We do not initialize hash - otherwise we would not be able
    * to test the init call.
    */
   return &pDummy->hashObj;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* ENGINE_TEST_COMMON_H */