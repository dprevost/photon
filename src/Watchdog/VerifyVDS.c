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
                  FILE * fp,
                  bool   doRepair )
{
   struct vdseProcessManager * processMgr;
   vdseFolder * topFolder;
   vdseMemAlloc * memAllocator;
   enum vdswValidation valid;
   vdseSessionContext context;
   ptrdiff_t  lockOffsets[VDSE_MAX_LOCK_DEPTH];
   int        numLocks = 0;
   struct vdswVerifyStruct verifyStruct = { 1, 0, stderr, doRepair };
   char timeBuf[30];
//#if defined (WIN32)
//   char tmpTime[9];
//#else
   time_t t;
   struct tm formattedTime;

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
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswVerify( vdseMemoryHeader * pMemoryAddress, FILE * fp )
{
   fprintf( fp, "Verification of VDS (no repair) is starting\n" );
   
   return vdswValidate( pMemoryAddress, fp, false );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

int vdswRepair( vdseMemoryHeader * pMemoryAddress, FILE * fp )
{
   fprintf( fp, "Verification and repair (if needed) of VDS is starting\n" );
   
   return vdswValidate( pMemoryAddress, fp, true );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0
int vdsValidator::ValidateMemObject( vdseMemObject * pMemObject )
{
   int rc;
   size_t numBlocks = 0;
   
   rc = ValidateList( &pMemObject->listBlockGroup );
   if ( rc == 0 ) {
      if ( pMemObject->listBlockGroup.currentSize == 0 ) {
         echo("listBlockGroup with zero size!");
         return -1;
      }

      // Retrieve all the blockgroups and add up the # blocks

      headOffset = SET_OFFSET( &pMemObject->listBlockGroup.head );
      next = &pMemObject->listBlockGroup.head;
      while ( next->nextOffset != headOffset ) {
         numBlocks += ;
         GET_PTR( next, next->nextOffset, vdseLinkNode );
      }
   }
   
   return rc;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

