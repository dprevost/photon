/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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
#include "Quasar/VerifyCommon.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psoqRecoverError
psoqVerifyMemObject( struct psoqVerifyStruct   * pVerify,
                     struct psonMemObject      * pMemObj,
                     struct psonSessionContext * pContext )
{
   enum psoqRecoverError rc = PSOQ_REC_OK;
   struct psonMemAlloc * pAlloc = (psonMemAlloc *) pContext->pAllocator;
   psonLinkNode * dummy;
   psonBlockGroup * pGroup;
   size_t numBlocks = 0;
   bool ok;
   
   /*
    * Reset the bitmap and the the list of groups.
    */
   psoqResetBitmap( pVerify->pBitmap );
   psonSetBufferFree( pVerify->pBitmap, 0, pAlloc->totalLength );

   rc = psoqVerifyList( pVerify, &pMemObj->listBlockGroup );
   if ( rc > PSOQ_REC_START_ERRORS ) return rc;
   
   /*
    * We retrieve the first node
    */
   ok = psonLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   while ( ok ) {
      pGroup = (psonBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psonBlockGroup,node));
      numBlocks += pGroup->numBlocks;
      
      ok = psonLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   dummy,
                                   &dummy );
   }
   if ( numBlocks != pMemObj->totalBlocks ) {
      rc = PSOQ_REC_CHANGES;
      psoqEcho( pVerify, "Number of blocks is wrong" );
      if (pVerify->doRepair) {
         pMemObj->totalBlocks = numBlocks;
         psoqEcho( pVerify, "Number of blocks set to proper value" );
      }
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psoqPopulateBitmap( struct psoqVerifyStruct   * pVerify,
                         struct psonMemObject      * pMemObj,
                         struct psonSessionContext * pContext )
{
   psonLinkNode * dummy;
   psonBlockGroup * pGroup;
   bool ok;
   
   /*
    * Reset the bitmap and populate it for validating the content of the 
    * object itself.
    */
   psoqResetBitmap( pVerify->pBitmap );
   /*
    * We retrieve the first node
    */
   ok = psonLinkedListPeakFirst( &pMemObj->listBlockGroup, &dummy );
   while ( ok ) {
      pGroup = (psonBlockGroup*)( 
         (unsigned char*)dummy - offsetof(psonBlockGroup,node));

      psonSetBufferFree( pVerify->pBitmap, 
         SET_OFFSET( pGroup )/PSON_BLOCK_SIZE*PSON_BLOCK_SIZE, 
         pGroup->numBlocks*PSON_BLOCK_SIZE );
      
      ok = psonLinkedListPeakNext( &pMemObj->listBlockGroup,
                                   dummy,
                                   &dummy );
   }
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
