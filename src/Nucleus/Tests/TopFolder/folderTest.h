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

#ifndef PSOT_FOLDER_TEST_H
#define PSOT_FOLDER_TEST_H

#include "Nucleus/Engine.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Transaction.h"
#include "Nucleus/InitEngine.h"
#include "Nucleus/Folder.h"
#include <photon/psoPhotonODBC.h>

PHOTON_ENGINE_EXPORT
psocErrMsgHandle g_psoErrorHandle;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/**
 * This function initializes the memory needed by the folder
 * object.
 *
 * A reminder: folders are memory objects - when they need memory, 
 * they ask it from their internal memObject which can then ask the 
 * global allocator. This requires proper initialization.
 *
 * The returned object is not initialized however - so that we can test
 * the Init() call.
 */
 
psonFolder* initFolderTest( psonSessionContext* pContext )
{
   bool ok;
   unsigned char* ptr;
   psonMemAlloc*  pAlloc;
   psonTx* pTx;
   psonFolder* pFolder;
   size_t allocatedLength = PSON_BLOCK_SIZE * 25;

   memset( pContext, 0, sizeof(psonSessionContext) );
   pContext->pidLocker = getpid();
   
   ok = psonInitEngine();
   assert( ok );
   psocInitErrorHandler( &pContext->errorHandler );

   /* Initialize the global allocator */
   ptr = malloc( allocatedLength );
   assert( ptr != NULL );
   g_pBaseAddr = ptr;
   pAlloc = (psonMemAlloc*)(g_pBaseAddr + PSON_BLOCK_SIZE);
   psonMemAllocInit( pAlloc, ptr, allocatedLength, pContext );
   
   /* Allocate memory for the tx object and initialize it */
   pTx = (psonTx*)psonMallocBlocks( pAlloc, PSON_ALLOC_ANY, 1, pContext );
   assert( pTx != NULL );
   ok = psonTxInit( pTx, 1, pContext );
   assert( ok );
   pContext->pTransaction = pTx;
   
   /* Allocate memory for the folder object */
   pFolder = (psonFolder*)psonMallocBlocks( pAlloc, PSON_ALLOC_API_OBJ, 1, pContext );
   assert( pFolder != NULL );
   
   return pFolder;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

psonTxStatus objTxStatus;

psonFolder* initTopFolderTest( psonSessionContext* pContext )
{
   psoErrors errcode;
   psonFolder* pFolder;
   pFolder = initFolderTest( pContext );
   
   errcode = psonMemObjectInit( &pFolder->memObject, 
                                PSON_IDENT_FOLDER,
                                &pFolder->blockGroup,
                                1 );
   assert( errcode == PSO_OK );

   psonTxStatusInit( &objTxStatus, SET_OFFSET(pContext->pTransaction) );
   objTxStatus.status = PSON_TXS_ADDED;

   pFolder->nodeObject.txCounter      = 0;
   pFolder->nodeObject.myNameLength   = 0;
   pFolder->nodeObject.myNameOffset   = PSON_NULL_OFFSET;
   pFolder->nodeObject.txStatusOffset = SET_OFFSET(&objTxStatus);
   pFolder->nodeObject.myParentOffset = PSON_NULL_OFFSET;

   errcode = psonHashTxInit( &pFolder->hashObj, 
                             SET_OFFSET(&pFolder->memObject),
                             25, 
                             pContext );
   assert( errcode == PSO_OK );

   return pFolder;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOT_FOLDER_TEST_H */

