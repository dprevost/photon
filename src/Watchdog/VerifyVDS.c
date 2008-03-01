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

#include "Common/Common.h"

#include "Watchdog/VerifyCommon.h"
#include "Watchdog/VerifyVDS.h"

#include "Engine/MemoryHeader.h"

#include "Engine/ProcessManager.h"
#include "Engine/Folder.h"
#include "Engine/MemoryAllocator.h"
#include "Engine/Queue.h"

vdseMemoryHeader * g_pMemoryAddress = NULL;
bool               g_bTestAllocator = false;
FILE *             g_fp = NULL;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswValidate( vdseMemoryHeader * pMemoryAddress, 
                  size_t           * pNumObjectsOK,
                  size_t           * pNumObjectsRepaired,
                  size_t           * pNumObjectsDeleted,
                  size_t           * pNumObjectsError,
                  FILE             * fp,
                  bool               doRepair )
{
   struct vdseProcessManager * processMgr;
   vdseFolder * topFolder;
   vdseMemAlloc * memAllocator;
   enum vdswRecoverError valid;
   vdseSessionContext context;
   ptrdiff_t  lockOffsets[VDSE_MAX_LOCK_DEPTH];
   int        numLocks = 0;
   struct vdswVerifyStruct verifyStruct = { 
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
   
   GET_PTR( processMgr, pMemoryAddress->processMgrOffset, struct vdseProcessManager );
   GET_PTR( topFolder, pMemoryAddress->treeMgrOffset, vdseFolder );
   GET_PTR( memAllocator, pMemoryAddress->allocatorOffset, vdseMemAlloc );

   /* allocate the bitmap */
   bitmapLength = offsetof( vdseMemBitmap, bitmap ) + 
      vdseGetBitmapLengthBytes( memAllocator->totalLength, VDSE_BLOCK_SIZE );
   verifyStruct.pBitmap = (vdseMemBitmap *) malloc(bitmapLength);
   vdseMemBitmapInit( verifyStruct.pBitmap,
                      0,
                      memAllocator->totalLength,
                      VDSE_BLOCK_SIZE );

   // Test the lock of the allocator
   if ( vdscIsItLocked( &memAllocator->memObj.lock ) ) {
      vdswEcho( &verifyStruct, 
         "Warning! The memory allocator is locked - the VDS might be corrupted" );
      if ( doRepair ) {
         vdswEcho( &verifyStruct, "Trying to reset it..." );
         vdscReleaseProcessLock ( &memAllocator->memObj.lock );
      }
      g_bTestAllocator = true;
   }
   
   vdswEcho( &verifyStruct, "Object name: /" );

   vdseInitSessionContext( &context );
   context.pAllocator = (void *) memAllocator;
   context.lockOffsets = lockOffsets;
   context.numLocks = &numLocks;
   
   valid = vdswVerifyFolder( &verifyStruct, topFolder, &context );
   switch ( valid ) {
   case VDSWR_OK:
      verifyStruct.numObjectsOK++;
      break;
   case VDSWR_CHANGES:
      verifyStruct.numObjectsRepaired++;
      break;
   case VDSWR_DELETED_OBJECT:
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

   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswVerify( vdseMemoryHeader * pMemoryAddress, 
                size_t           * pNumObjectsOK,
                size_t           * pNumObjectsRepaired,
                size_t           * pNumObjectsDeleted,
                size_t           * pNumObjectsError,
                FILE             * fp )
{
   fprintf( fp, "Verification of VDS (no repair) is starting\n" );
   
   return vdswValidate( pMemoryAddress, 
                        pNumObjectsOK,
                        pNumObjectsRepaired,
                        pNumObjectsDeleted,
                        pNumObjectsError, 
                        fp, 
                        false );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswRepair( vdseMemoryHeader * pMemoryAddress, 
                size_t           * pNumObjectsOK,
                size_t           * pNumObjectsRepaired,
                size_t           * pNumObjectsDeleted,
                size_t           * pNumObjectsError,
                FILE             * fp )
{
   fprintf( fp, "Verification and repair (if needed) of VDS is starting\n" );
   
   return vdswValidate( pMemoryAddress, 
                        pNumObjectsOK,
                        pNumObjectsRepaired,
                        pNumObjectsDeleted,
                        pNumObjectsError, 
                        fp, 
                        true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

