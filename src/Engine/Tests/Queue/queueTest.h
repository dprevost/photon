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

#ifndef VDST_QUEUE_TEST_H
#define VDST_QUEUE_TEST_H

#include "Engine/Engine.h"
#include "Engine/MemoryObject.h"
#include "Engine/BlockGroup.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Transaction.h"
#include "Engine/InitEngine.h"
#include "Engine/Folder.h"
#include "Engine/Queue.h"
#include "Tests/PrintError.h"

VDSF_ENGINE_EXPORT
vdscErrMsgHandle g_vdsErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the queue
 * object.
 *
 * A reminder: queues are memory objects - when they need memory, 
 * they ask it from their internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
vdseQueue * 
initQueueTest( bool                testIsExpectedToSucceed,
                 vdseSessionContext* pContext )
{
   int errcode;
   unsigned char* ptr;
   vdseMemAlloc*  pAlloc;
   vdseTx* pTx;
   vdseQueue* pQueue;
   size_t allocatedLength = VDSE_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(vdseSessionContext) );
   pContext->pidLocker = getpid();
   
   errcode = vdseInitEngine();
   if ( errcode != 0 )
   {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   vdscInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   if (ptr == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
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
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   errcode = vdseTxInit( pTx, 1, pContext );
   if ( errcode != 0 ) 
   {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the queue object */
   pQueue = (vdseQueue*)vdseMallocBlocks( pAlloc, VDSE_ALLOC_API_OBJ, 1, pContext );
   if ( pQueue == NULL )
   {
      fprintf( stderr, "Abnormal error at line %d in folderTest.h\n", __LINE__ );
      if ( testIsExpectedToSucceed )
         exit(1);
      exit(0);
   }
   
   return pQueue;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/*
 * The problem: if we use unicode, litterals like "string" must be defined
 * as L"string".
 *
 * One solution is to encode the strings on the fly. 
 *
 * Only problem is that this requires allocating memory for the new array
 * of wchar_t (and also for the second - lowecase - array, when needed.
 *
 */
#if VDS_SUPPORT_i18n

wchar_t *wcTemp = NULL, *wcTempLowcase = NULL;
size_t mbsrtowcs(wchar_t *dst, const char **src, size_t len, mbstate_t *ps);

wchar_t* strCheck( const char* str )
{
   mbstate_t ps;
   size_t len;
   
   if ( wcTemp != NULL )
      free( wcTemp );
   
   len = mbsrtowcs( NULL, &str, 0, &ps );
   wcTemp = (wchar_t*)malloc( (len+1)*sizeof(wchar_t) );
   
   mbsrtowcs( wcTemp, &str, len, &ps );
   
   return wcTemp;
}

wchar_t* strCheckLow( const char* str )
{
   mbstate_t ps;
   size_t len;
   
   if ( wcTempLowcase != NULL )
      free( wcTempLowcase );

   len = mbsrtowcs( NULL, &str, 0, &ps );
   wcTempLowcase = (wchar_t*)malloc( (len+1)*sizeof(wchar_t) );

   mbsrtowcs( wcTempLowcase, &str, len, &ps );
   return wcTempLowcase;
}

#else

char* strCheck( char* str )
{
   return str;
}

char* strCheckLow( char* str )
{
   return str;
}

#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDST_QUEUE_TEST_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

