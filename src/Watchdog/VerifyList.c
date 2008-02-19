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

/*
 * What could possibly go wrong when a crash occurs during a call to the 
 * linked list?
 *
 *  - GetFirst/Last/RemoveItem
 *      The chains are not broken - one of the chain might be longer than
 *      the other (using the longest chain will recover the removed node).
 *  - PutFirst/Last
 *      The chains are not broken - one of the chain might be longer than
 *      the other (using the longest chain will recover the added node).
 *      
 *      However, if the load and stores are reordered by the cpu or the
 *      compiler... things can be different. Looking at all possible 
 *      scenarios, you could have one of the chains broken. The only
 *      scenario with both chains broken: the end of the chain is always
 *      the new node.
 *  - ReplaceItem
 *      This function is only called from the allocator part of the 
 *      memory object. Since the map and linked list of the allocator must
 *      be reset from scratch anyway...
 *
 * Additional notes:
 *  - the recovery might loose a new node when using the unbroken chain
 *    to recover the broken chain. Not really an issue as that change was
 *    not committed yet!
 *  - We need a way to examine the whole object and try to recover the
 *    data in case something really bad happen (a stray pointer in the 
 *    application programs overwriting part of the data could do that).
 */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum repairMode
{
   NO_REPAIR,
   REPAIR_LENGTH,
   REPAIR_WITH_FW,
   REPAIR_WITH_BW,
   REPAIR_WITH_BOTH_EQUAL,
   REPAIR_WITH_BOTH_FW_FIRST,
   REPAIR_WITH_BOTH_BW_FIRST
};

struct repairKit
{
   enum repairMode mode;
   struct vdseLinkedList * pList;
   size_t forwardChainLen;
   size_t backwardChainLen;
   bool breakInForwardChain;
   bool breakInBackwardChain;
   bool foundNextBreak;
   vdseLinkNode * nextBreak;
   vdseLinkNode * previousBreak;

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswVerifyList( vdswVerifyStruct      * pVerify,
                struct vdseLinkedList * pList )
{
   vdseLinkNode * next, * previous;
   ptrdiff_t headOffset;
   struct repairKit kit = { NO_REPAIR, pList, 0, 0, false, false, false, NULL, NULL };
   
   /*
    * Do not add a test for the size of the list here (as in 
    *    if ( currentSize == 0 ) return 0; ).
    *
    * The size of the list might not be accurate if the cpu can reorder
    * instructions - the real size could be one higher than the value of
    * pList->currentSize.
    */
   
   headOffset = SET_OFFSET( &pList->head );

   // We loop forward until we come back to head or until we clearly have
   // a discontinuity in the chain.
   next = &pList->head;
   while ( next->nextOffset != headOffset ) {
      if ( vdswVerifyOffset( pVerify, next->nextOffset ) )
         kit.forwardChainLen++;
      else {
         kit.nextBreak = next;
         kit.breakInForwardChain = true;
         break;
      }
      GET_PTR( next, next->nextOffset, vdseLinkNode );
   }

   // Same as before but we loop backward.
   next = &pList->head;
   while ( next->previousOffset != headOffset ) {
      if ( vdswVerifyOffset( pVerify, next->previousOffset ) ) {
         kit.backwardChainLen++;
      }
      else {
         kit.previousBreak = next;
         kit.breakInBackwardChain = true;
         break;
      }
      GET_PTR( next, next->previousOffset, vdseLinkNode );
      if ( kit.nextBreak == next )
         kit.foundNextBreak = true;
   }
   
   // So how did it go?... we have multiple possibilities here.
   if ( kit.breakInForwardChain && kit.breakInBackwardChain ) {
      vdswEcho( pVerify, "Both chains broken" );
fprintf( stderr, "breaks = %p %p\n", kit.nextBreak, kit.previousBreak );
      if ( kit.nextBreak == kit.previousBreak )
         kit.mode = REPAIR_WITH_BOTH_EQUAL;
      else
         kit.mode = REPAIR_WITH_BOTH_FW_FIRST;
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
         if ( kit.previousBreak == NULL || kit.previousBreak == next )
            next->previousOffset = SET_OFFSET( previous );
         else {
            if ( next->previousOffset != SET_OFFSET( previous ) ) {
               vdswEcho( pVerify, "Found another abnormal break in backward chain, repairing..." );
               next->previousOffset = SET_OFFSET( previous );
            }
         }
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

         if ( kit.nextBreak == NULL || kit.nextBreak == next )
            next->nextOffset = SET_OFFSET( previous );
         else {
            if ( next->nextOffset != SET_OFFSET( previous ) ) {
               vdswEcho( pVerify, "Found another abnormal break in forward chain, repairing..." );
               next->nextOffset = SET_OFFSET( previous );
            }
         }
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.backwardChainLen;
      break;
      
   case REPAIR_WITH_BOTH_EQUAL:
   
      vdswEcho( pVerify, "Both chains broken at same node" );
      
      // We loop forward until we find the broken link
      previous = &pList->head;
      do {
         GET_PTR( next, previous->nextOffset, vdseLinkNode );
         if ( kit.previousBreak == next ) {
            next->previousOffset = SET_OFFSET( previous );
         }
         // prepare for next round
         previous = next;
         
      } while ( next != kit.previousBreak );
      
      // We loop backward until we find the broken link 
      previous = &pList->head;
      do {
         GET_PTR( next, previous->previousOffset, vdseLinkNode );
         if ( kit.nextBreak == next ) {
            next->nextOffset = SET_OFFSET( previous );
         }
         // prepare for next round
         previous = next;
         
      } while ( next != kit.nextBreak );

      pList->currentSize = kit.forwardChainLen + kit.backwardChainLen - 1;
      break;

   case REPAIR_WITH_BOTH_FW_FIRST:
   
      vdswEcho( pVerify, "Both chains broken - don't know how to repair" );
//      return -1;
//break;

      // We loop forward until we find the broken link      
      previous = &pList->head;
      do {
         GET_PTR( next, previous->nextOffset, vdseLinkNode );
         if ( kit.nextBreak == next ) {
            /*
             * We just find our break point but we did not find the break
             * point 
             */
         }
         if ( kit.previousBreak == next ) {
         }

 //        next->previousOffset = SET_OFFSET( previous );
         else {
            if ( next->previousOffset != SET_OFFSET( previous ) ) {
               vdswEcho( pVerify, "Found another abnormal break in backward chain, repairing..." );
               next->previousOffset = SET_OFFSET( previous );
            }
         }
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );


      return -1;

   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

