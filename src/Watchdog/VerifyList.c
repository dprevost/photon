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

// --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--

#include "Common/Common.h"

#include "Watchdog/VerifyCommon.h"

#include "Engine/LinkedList.h"
#include "Engine/LinkNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum repairMode
{
   NO_REPAIR,
   REPAIR_LENGTH,
   REPAIR_WITH_FW,
   REPAIR_WITH_BW,
   REPAIR_WITH_BOTH
};

struct repairKit
{
   enum repairMode mode;
   struct vdseLinkedList * pList;
   size_t forwardChainLen;
   size_t backwardChainLen;
   bool breakInForwardChain;
   bool breakInBackwardChain;

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyList( vdswVerifyStruct      * pVerify,
                struct vdseLinkedList * pList )
{
   vdseLinkNode * next, * previous;
   ptrdiff_t headOffset;
   struct repairKit kit = { NO_REPAIR, pList, 0, 0, false, false };
   
   headOffset = SET_OFFSET( &pList->head );

   // We loop forward until we come back to head or until we clearly have
   // a discontinuity in the chain.
   next = &pList->head;
   while ( next->nextOffset != headOffset )
   {
      if ( vdswVerifyOffset( pVerify, next->nextOffset ) )
         kit.forwardChainLen++;
      else
      {
         next->nextOffset = headOffset;
         kit.breakInForwardChain = true;
         break;
      }
      GET_PTR( next, next->nextOffset, vdseLinkNode );
   }

   // Same as before but we loop backward.
   next = &pList->head;
   while ( next->previousOffset != headOffset )
   {
      if ( vdswVerifyOffset( pVerify, next->previousOffset ) )
         kit.backwardChainLen++;
      else
      {
         next->previousOffset = headOffset;
         kit.breakInBackwardChain = true;
         break;
      }
      GET_PTR( next, next->previousOffset, vdseLinkNode );
   }
   
   // So how did it go?... we have multiple possibilities here.
   if ( kit.breakInForwardChain && kit.breakInBackwardChain ) {
      kit.mode = REPAIR_WITH_BOTH;
      vdswEcho( pVerify, "Both chains broken" );
   }
   else if ( kit.breakInForwardChain && ! kit.breakInBackwardChain ) {
      kit.mode = REPAIR_WITH_BW;
      vdswEcho( pVerify, "Forward chain broken" );
   }
   else if ( ! kit.breakInForwardChain && kit.breakInBackwardChain ) {
      kit.mode = REPAIR_WITH_FW;
      vdswEcho( pVerify, "Backward chain broken" );
   }
   else {
      if ( kit.backwardChainLen == kit.forwardChainLen ) {// all is well !
         if ( pList->currentSize != kit.forwardChainLen ) {
            kit.mode = REPAIR_LENGTH;
            vdswEcho( pVerify, "Invalid number of elements in linked list");
         }
      }
      else {
         vdswEcho( pVerify, "Warning - counts in foward and backward chains differ:" );
         vdswEcho( pVerify, "          using the longest chain to rebuild" );
         if ( kit.backwardChainLen > kit.forwardChainLen )
            kit.mode = REPAIR_WITH_BW;
         else
            kit.mode = REPAIR_WITH_FW;
      }
   }
   
   if ( ! pVerify->doRepair ) return 0;
   
   switch( kit.mode ) {
   
   case NO_REPAIR:
      break;
      
   case REPAIR_LENGTH:
      vdswEcho( pVerify, "Small correction in the number of elements of linked list");
      pList->currentSize = kit.forwardChainLen;
      break;
         
   case REPAIR_WITH_FW:
   
      vdswEcho( pVerify, "Repairing list using forward chain" );
      // We loop forward until we come back to head.
      
      previous = &pList->head;
      do {
         GET_PTR( next, previous->nextOffset, vdseLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.forwardChainLen;
      break;
      
   case REPAIR_WITH_BW:
      
      vdswEcho( pVerify, "Repairing list using backward chain" );
      // We loop backward until we come back to head.
      
      previous = &pList->head;
      do {
         GET_PTR( next, previous->previousOffset, vdseLinkNode );
         next->nextOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.backwardChainLen;
      break;
      
   case REPAIR_WITH_BOTH:
   
      vdswEcho( pVerify, "Both chains broken - don't know how to repair" );
      return -1;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

