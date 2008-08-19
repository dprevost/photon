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
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "Watchdog/VerifyCommon.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError
vdswVerifyMemObject( struct vdswVerifyStruct   * pVerify,
                     struct psnMemObject      * pMemObj,
                     struct psnSessionContext * pContext )
{
   enum vdswRecoverError rc = VDSWR_OK;
   struct psnMemAlloc * pAlloc = (psnMemAlloc *) pContext->pAllocator;
   psnLinkNode * dummy;
   psnBlockGroup * pGroup;
   size_t numBlocks = 0;
   bool ok;
   
   /*
    * Reset the bitmap and the the list of groups.
    */
   vdswResetBitmap( pVerify->pBitmap );
   psnSetBufferFree( pVerify->pBitmap, 0, pAlloc->totalLength );

   rc = vdswVerifyList( pVerify, &pMemObj->listBlockGroup );
   if ( rc > VDSWR_START_ERRORS ) return rc;
   
   /*
    * We retrieve the first node
    */
   ok = psnLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   while ( ok ) {
      pGroup = (psnBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psnBlockGroup,node));
      numBlocks += pGroup->numBlocks;
      
      ok = psnLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   dummy,
                                   &dummy );
   }
   if ( numBlocks != pMemObj->totalBlocks ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Number of blocks is wrong" );
      if (pVerify->doRepair) {
         pMemObj->totalBlocks = numBlocks;
         vdswEcho( pVerify, "Number of blocks set to proper value" );
      }
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdswPopulateBitmap( struct vdswVerifyStruct   * pVerify,
                         struct psnMemObject      * pMemObj,
                         struct psnSessionContext * pContext )
{
   psnLinkNode * dummy;
   psnBlockGroup * pGroup;
   bool ok;
   
   /*
    * Reset the bitmap and populate it for validating the content of the 
    * object itself.
    */
   vdswResetBitmap( pVerify->pBitmap );
   /*
    * We retrieve the first node
    */
   ok = psnLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   while ( ok ) {
      pGroup = (psnBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psnBlockGroup,node));

      psnSetBufferFree( pVerify->pBitmap, 
         SET_OFFSET( pGroup )/PSN_BLOCK_SIZE*PSN_BLOCK_SIZE, 
         pGroup->numBlocks*PSN_BLOCK_SIZE );
      
      ok = psnLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   dummy,
                                   &dummy );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

