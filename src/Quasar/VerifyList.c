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
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"

#include "Quasar/VerifyCommon.h"

#include "Nucleus/LinkedList.h"
#include "Nucleus/LinkNode.h"

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
   REPAIR_WITH_FW_BREAK,
   REPAIR_WITH_BW_BREAK,
   REPAIR_WITH_FW_NO_BREAK,
   REPAIR_WITH_BW_NO_BREAK,
   REPAIR_WITH_BOTH_EQUAL /*,
   REPAIR_WITH_BOTH_FW_FIRST,
   REPAIR_WITH_BOTH_BW_FIRST */
};

struct repairKit
{
   enum repairMode mode;
   struct psnLinkedList * pList;
   size_t forwardChainLen;
   size_t backwardChainLen;
   bool breakInForwardChain;
   bool breakInBackwardChain;
   bool foundNextBreak;
   /* The last valid node before the break */
   psnLinkNode * nextBreak;
   /* The node before the last valid node before the break */
   psnLinkNode * nextBreakPrevious;
   /* The last valid node before the break */
   psnLinkNode * previousBreak;
   /* The node before the last valid node before the break */
   psnLinkNode * previousBreakPrevious;

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswRecoverError 
vdswVerifyList( vdswVerifyStruct      * pVerify,
                struct psnLinkedList * pList )
{
   psnLinkNode * next, * previous;
   ptrdiff_t headOffset;
   struct repairKit kit = { \
      NO_REPAIR, pList, 0, 0, false, false, false, NULL, NULL, NULL, NULL };
   bool foundNode;
   enum vdswRecoverError rc = VDSWR_OK;
   
   /*
    * Do not add a test for the size of the list here (as in 
    *    if ( currentSize == 0 ) return rc; ).
    *
    * The size of the list might not be accurate if the cpu can reorder
    * instructions - the real size could be one higher than the value of
    * pList->currentSize.
    */
   
   headOffset = SET_OFFSET( &pList->head );

   // We loop forward until we come back to head or until we clearly have
   // a discontinuity in the chain.
   next = &pList->head;
   previous = next;
   while ( next->nextOffset != headOffset ) {
      if ( next->nextOffset == PSN_NULL_OFFSET ) {
         kit.nextBreak = next;
         kit.nextBreakPrevious = previous;
         kit.breakInForwardChain = true;
         break;
      }
      else {
         if ( vdswVerifyOffset( pVerify, next->nextOffset ) ) {
            kit.forwardChainLen++;
         }
         else {
            vdswEcho( pVerify, "Invalid offset - cannot repair" );
            return VDSWR_UNRECOVERABLE_ERROR;
         }
      }
      previous = next;
      GET_PTR( next, next->nextOffset, psnLinkNode );
   }

   // Same as before but we loop backward.
   next = &pList->head;
   previous = next;
   while ( next->previousOffset != headOffset ) {
      if ( next->previousOffset == PSN_NULL_OFFSET ) {
         kit.previousBreak = next;
         kit.previousBreakPrevious = previous;
         kit.breakInBackwardChain = true;
         break;
      }
      else {
         if ( vdswVerifyOffset( pVerify, next->previousOffset ) ) {
            kit.backwardChainLen++;
         }
         else {
            vdswEcho( pVerify, "Invalid offset - cannot repair" );
            return VDSWR_UNRECOVERABLE_ERROR;
         }
      }
      previous = next;
      GET_PTR( next, next->previousOffset, psnLinkNode );
   }
   
   // So how did it go?... we have multiple possibilities here.
   if ( kit.breakInForwardChain && kit.breakInBackwardChain ) {
      rc = VDSWR_CHANGES;
      vdswEcho( pVerify, "Both chains broken" );
      if ( kit.nextBreak == kit.previousBreak ) {
         kit.mode = REPAIR_WITH_BOTH_EQUAL;
      }
      else {
         vdswEcho( pVerify, "Chains broken at different places - cannot repair" );
         return VDSWR_UNRECOVERABLE_ERROR;
      }
   }
   else if ( kit.breakInForwardChain && ! kit.breakInBackwardChain ) {
      rc = VDSWR_CHANGES;
      kit.mode = REPAIR_WITH_BW_BREAK;
      vdswEcho( pVerify, "Forward chain broken" );
   }
   else if ( ! kit.breakInForwardChain && kit.breakInBackwardChain ) {
      rc = VDSWR_CHANGES;
      kit.mode = REPAIR_WITH_FW_BREAK;
      vdswEcho( pVerify, "Backward chain broken" );
   }
   else {
      if ( kit.backwardChainLen == kit.forwardChainLen ) { /* all is well ! */
         if ( pList->currentSize != kit.forwardChainLen ) {
            rc = VDSWR_CHANGES;
            kit.mode = REPAIR_LENGTH;
            vdswEcho( pVerify, "Invalid number of elements in linked list");
         }
      }
      else {
         vdswEcho( pVerify, "Warning - counts in foward and backward chains differ:" );
         vdswEcho( pVerify, "          using the longest chain to rebuild" );
         rc = VDSWR_CHANGES;
         if ( kit.backwardChainLen > kit.forwardChainLen ) {
            kit.mode = REPAIR_WITH_BW_NO_BREAK;
         }
         else {
            kit.mode = REPAIR_WITH_FW_NO_BREAK;
         }
      }
   }
   
   if ( ! pVerify->doRepair ) return rc;
   
   switch( kit.mode ) {
   
   case NO_REPAIR:
      break;
      
   case REPAIR_LENGTH:
      vdswEcho( pVerify, "Small correction in the number of elements of linked list");
      pList->currentSize = kit.forwardChainLen;
      break;
         
   case REPAIR_WITH_FW_NO_BREAK:
   
      vdswEcho( pVerify, "Repairing list using forward chain" );
      
      /*
       * We loop forward until we come back to head. We reset the bw chain
       * using brute force (finding which node is present in one chain and
       * missing in the other one would likely consume more cpu cycles...).
       */
      previous = &pList->head;
      do {
         GET_PTR( next, previous->nextOffset, psnLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         /* prepare for next round */
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.forwardChainLen;
      break;
      
   case REPAIR_WITH_BW_NO_BREAK:
      
      vdswEcho( pVerify, "Repairing list using backward chain" );
      
      /*
       * We loop backward until we come back to head. We reset the fw chain
       * using brute force (finding which node is present in one chain and
       * missing in the other one would likely consume more cpu cycles...).
       */
      previous = &pList->head;
      do {
         GET_PTR( next, previous->previousOffset, psnLinkNode );
         next->nextOffset = SET_OFFSET( previous );

         /* prepare for next round */
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.backwardChainLen;
      break;
      
   case REPAIR_WITH_FW_BREAK:
   
      vdswEcho( pVerify, "Repairing list using forward chain" );
      foundNode = false;
      
      /*
       * We loop forward until we come back to head. We reset the bw chain
       * using brute force (there is a small probability that the break node
       * is not in the valid chain - we can't just look for it and repair
       * the chain at that location).
       *
       * Warning: If the recovery is interrupted, we might loose the lost node.
       * \todo dump the data before proceeding.
       */
      previous = &pList->head;
      do {
         GET_PTR( next, previous->nextOffset, psnLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         if ( kit.previousBreak == next )
            foundNode = true;
         /* prepare for next round */
         previous = next;
         
      } while ( next != &pList->head );

      if ( foundNode ) {
         pList->currentSize = kit.forwardChainLen;
      }
      else {
         /* 
          * We need to add it to the chain - at its proper place. Don't
          * forget that the previous loop did "incorrectly" reset the 
          * value of kit.previousBreakPrevious->previousOffset (we use it to 
          * recover the value of previous but we also need to reset it).
          */
         GET_PTR( previous, kit.previousBreakPrevious->previousOffset, psnLinkNode );
         
         kit.previousBreak->nextOffset = SET_OFFSET( kit.previousBreakPrevious );
         kit.previousBreak->previousOffset = SET_OFFSET( previous );
         previous->nextOffset = SET_OFFSET( kit.previousBreak );
         kit.previousBreakPrevious->previousOffset = SET_OFFSET( kit.previousBreak );
         
         pList->currentSize = kit.forwardChainLen + 1;
      }
      
      break;
      
   case REPAIR_WITH_BW_BREAK:
      
      vdswEcho( pVerify, "Repairing list using backward chain" );
      foundNode = false;
      
      /*
       * We loop backward until we come back to head. We reset the fw chain
       * using brute force (there is a small probability that the break node
       * is not in the valid chain - we can't just look for it and repair
       * the chain at that location).
       *
       * Warning: If the recovery is interrupted, we might loose the lost node.
       * \todo dump the data before proceeding.
       */
      previous = &pList->head;
      do {
         GET_PTR( next, previous->previousOffset, psnLinkNode );
         next->nextOffset = SET_OFFSET( previous );
         if ( kit.nextBreak == next )
            foundNode = true;
         /* prepare for next round */
         previous = next;
         
      } while ( next != &pList->head );

      
      if ( foundNode ) {
         pList->currentSize = kit.backwardChainLen;
      }
      else { 
         /* 
          * We need to add it to the chain - at its proper place. Don't
          * forget that the previous loop did "incorrectly" reset the 
          * value of kit.nextBreakPrevious->nextOffset (we use it to recover
          * the value of next but we also need to reset it).
          */
         GET_PTR( next, kit.nextBreakPrevious->nextOffset, psnLinkNode );
         
         kit.nextBreak->previousOffset = SET_OFFSET( kit.nextBreakPrevious );
         kit.nextBreak->nextOffset = SET_OFFSET( next );
         next->previousOffset = SET_OFFSET( kit.nextBreak );
         kit.nextBreakPrevious->nextOffset = SET_OFFSET( kit.nextBreak );
         
         pList->currentSize = kit.backwardChainLen + 1;
      }
      
      break;
      
   case REPAIR_WITH_BOTH_EQUAL:
   
      vdswEcho( pVerify, "Repairing both chains" );
      
      /*
       * No loop needed here - we are just missing the links of the
       * new node.
       */
      kit.nextBreak->nextOffset = SET_OFFSET( kit.previousBreakPrevious );
      kit.nextBreak->previousOffset = SET_OFFSET( kit.nextBreakPrevious );
      
      pList->currentSize = kit.forwardChainLen + kit.backwardChainLen - 1;

      break;
   }
   
   return rc;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

