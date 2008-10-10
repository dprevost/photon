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

#include "Common/Common.h"

#include "Quasar/VerifyCommon.h"
#include "Quasar/VerifyVDS.h"

#include "Nucleus/MemoryHeader.h"

#include "Nucleus/ProcessManager.h"
#include "Nucleus/Folder.h"
#include "Nucleus/MemoryAllocator.h"
#include "Nucleus/Queue.h"

psonMemoryHeader * g_pMemoryAddress = NULL;
bool               g_bTestAllocator = false;
FILE *             g_fp = NULL;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoqValidate( psonMemoryHeader * pMemoryAddress, 
                   size_t           * pNumObjectsOK,
                   size_t           * pNumObjectsRepaired,
                   size_t           * pNumObjectsDeleted,
                   size_t           * pNumObjectsError,
                   FILE             * fp,
                   bool               doRepair )
{
   struct psonProcessManager * processMgr;
   psonFolder * topFolder;
   psonMemAlloc * memAllocator;
   enum psoqRecoverError valid;
   psonSessionContext context;
   ptrdiff_t  lockOffsets[PSON_MAX_LOCK_DEPTH];
   int        numLocks = 0;
   struct psoqVerifyStruct verifyStruct = { 
      1, 0, stderr, doRepair, 0, 0, 0, 0, NULL };
   char timeBuf[30];
   time_t t;
   struct tm formattedTime;
   size_t bitmapLength = 0;
   
   *pNumObjectsOK       = 0;
   *pNumObjectsRepaired = 0;
   *pNumObjectsDeleted  = 0;
   *pNumObjectsError    = 0;

   verifyStruct.fp = fp;
   
   t = time(NULL);
   localtime_r( &t, &formattedTime );

   memset( timeBuf, '\0', 30 );
   strftime( timeBuf, 30, "%B %d, %Y", &formattedTime );
   fprintf( fp, "Current date: %s\n", timeBuf );

   memset( timeBuf, '\0', 30 );
   strftime( timeBuf, 30, "%H:%M:%S", &formattedTime );
   fprintf( fp, "Current time: %s\n\n", timeBuf );
   
   GET_PTR( processMgr, pMemoryAddress->processMgrOffset, struct psonProcessManager );
   GET_PTR( topFolder, pMemoryAddress->treeMgrOffset, psonFolder );
   GET_PTR( memAllocator, pMemoryAddress->allocatorOffset, psonMemAlloc );

   /* allocate the bitmap */
   bitmapLength = offsetof( psonMemBitmap, bitmap ) + 
      psonGetBitmapLengthBytes( memAllocator->totalLength, PSON_BLOCK_SIZE );
   verifyStruct.pBitmap = (psonMemBitmap *) malloc(bitmapLength);
   psonMemBitmapInit( verifyStruct.pBitmap,
                      0,
                      memAllocator->totalLength,
                      PSON_BLOCK_SIZE );

   // Test the lock of the allocator
   if ( psocIsItLocked( &memAllocator->memObj.lock ) ) {
      psoqEcho( &verifyStruct, 
         "Warning! The memory allocator is locked - the shared memory might be corrupted" );
      if ( doRepair ) {
         psoqEcho( &verifyStruct, "Trying to reset it..." );
         psocReleaseProcessLock ( &memAllocator->memObj.lock );
      }
      g_bTestAllocator = true;
   }
   
   psoqEcho( &verifyStruct, "Object name: /" );

   psonInitSessionContext( &context );
   context.pAllocator = (void *) memAllocator;
   context.lockOffsets = lockOffsets;
   context.numLocks = &numLocks;
   
   valid = psoqVerifyFolder( &verifyStruct, topFolder, &context );
   switch ( valid ) {
   case PSOQ_REC_OK:
      verifyStruct.numObjectsOK++;
      break;
   case PSOQ_REC_CHANGES:
      verifyStruct.numObjectsRepaired++;
      break;
   case PSOQ_REC_DELETED_OBJECT:
      verifyStruct.numObjectsDeleted++;
      break;
   default: /* other errors */
      verifyStruct.numObjectsError++;
      break;
   }

   *pNumObjectsOK = verifyStruct.numObjectsOK;
   *pNumObjectsRepaired = verifyStruct.numObjectsRepaired;
   *pNumObjectsDeleted = verifyStruct.numObjectsDeleted;
   *pNumObjectsError = verifyStruct.numObjectsError;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoqVerify( psonMemoryHeader * pMemoryAddress, 
                 size_t           * pNumObjectsOK,
                 size_t           * pNumObjectsRepaired,
                 size_t           * pNumObjectsDeleted,
                 size_t           * pNumObjectsError,
                 FILE             * fp )
{
   fprintf( fp, "Verification of VDS (no repair) is starting\n" );
   
   psoqValidate( pMemoryAddress, 
                 pNumObjectsOK,
                 pNumObjectsRepaired,
                 pNumObjectsDeleted,
                 pNumObjectsError, 
                 fp, 
                 false );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoqRepair( psonMemoryHeader * pMemoryAddress, 
                 size_t           * pNumObjectsOK,
                 size_t           * pNumObjectsRepaired,
                 size_t           * pNumObjectsDeleted,
                 size_t           * pNumObjectsError,
                 FILE             * fp )
{
   fprintf( fp, "Verification and repair (if needed) of VDS is starting\n" );
   
   psoqValidate( pMemoryAddress, 
                 pNumObjectsOK,
                 pNumObjectsRepaired,
                 pNumObjectsDeleted,
                 pNumObjectsError, 
                 fp, 
                 true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

