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

#include "Common/Common.h"
#include "Quasar/VerifyCommon.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum qsrRecoverError
qsrVerifyMemObject( struct qsrVerifyStruct   * pVerify,
                     struct psonMemObject      * pMemObj,
                     struct psonSessionContext * pContext )
{
   enum qsrRecoverError rc = QSR_REC_OK;
   struct psonMemAlloc * pAlloc = (psonMemAlloc *) pContext->pAllocator;
   psonLinkNode * dummy;
   psonBlockGroup * pGroup;
   size_t numBlocks = 0;
   bool ok;
   
   /*
    * Reset the bitmap and the the list of groups.
    */
   qsrResetBitmap( pVerify->pBitmap );
   psonSetBufferFree( pVerify->pBitmap, 0, pAlloc->totalLength );

   rc = qsrVerifyList( pVerify, &pMemObj->listBlockGroup );
   if ( rc > QSR_REC_START_ERRORS ) return rc;
   
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
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "Number of blocks is wrong" );
      if (pVerify->doRepair) {
         pMemObj->totalBlocks = numBlocks;
         qsrEcho( pVerify, "Number of blocks set to proper value" );
      }
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void qsrPopulateBitmap( struct qsrVerifyStruct   * pVerify,
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
   qsrResetBitmap( pVerify->pBitmap );
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

