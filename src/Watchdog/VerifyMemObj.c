/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of the vdsf (Virtual Data Space Framework) Library.
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
#include "Engine/MemoryObject.h"
#include "Engine/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError
vdswVerifyMemObject( struct vdswVerifyStruct   * pVerify,
                     struct vdseMemObject      * pMemObj,
                     struct vdseSessionContext * pContext )
{
   enum vdswRecoverError rc = VDSWR_OK;
   struct vdseMemAlloc * pAlloc = (vdseMemAlloc *) pContext->pAllocator;
   vdseLinkNode * dummy;
   enum ListErrors errGroup;
   vdseBlockGroup * pGroup;
   size_t numBlocks = 0;
   
   /*
    * Reset the bitmap and the the list of groups.
    */
   vdswResetBitmap( pVerify->pBitmap );
   vdseSetBufferFree( pVerify->pBitmap, 0, pAlloc->totalLength );

   rc = vdswVerifyList( pVerify, &pMemObj->listBlockGroup );
   if ( rc > VDSWR_START_ERRORS ) return rc;
   
   /*
    * We retrieve the first node
    */
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                       &dummy );
   while ( errGroup == LIST_OK ) {
      pGroup = (vdseBlockGroup*)( 
         (unsigned char*)dummy - offsetof(vdseBlockGroup,node));
      numBlocks += pGroup->numBlocks;
      
      errGroup = vdseLinkedListPeakNext( &pMemObj->listBlockGroup,
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
                         struct vdseMemObject      * pMemObj,
                         struct vdseSessionContext * pContext )
{
   vdseLinkNode * dummy;
   enum ListErrors errGroup;
   vdseBlockGroup * pGroup;

   /*
    * Reset the bitmap and populate it for validating the content of the 
    * object itself.
    */
   vdswResetBitmap( pVerify->pBitmap );
   /*
    * We retrieve the first node
    */
   errGroup = vdseLinkedListPeakFirst( &pMemObj->listBlockGroup,
                                       &dummy );
   while ( errGroup == LIST_OK ) {
      pGroup = (vdseBlockGroup*)( 
         (unsigned char*)dummy - offsetof(vdseBlockGroup,node));

      vdseSetBufferFree( pVerify->pBitmap, 
         SET_OFFSET( pGroup )/VDSE_BLOCK_SIZE*VDSE_BLOCK_SIZE, 
         pGroup->numBlocks*VDSE_BLOCK_SIZE );
      
      errGroup = vdseLinkedListPeakNext( &pMemObj->listBlockGroup,
                                         dummy,
                                         &dummy );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

