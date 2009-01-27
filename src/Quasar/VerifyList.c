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
   struct psonLinkedList * pList;
   size_t forwardChainLen;
   size_t backwardChainLen;
   bool breakInForwardChain;
   bool breakInBackwardChain;
   bool foundNextBreak;
   /* The last valid node before the break */
   psonLinkNode * nextBreak;
   /* The node before the last valid node before the break */
   psonLinkNode * nextBreakPrevious;
   /* The last valid node before the break */
   psonLinkNode * previousBreak;
   /* The node before the last valid node before the break */
   psonLinkNode * previousBreakPrevious;

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum qsrRecoverError 
qsrVerifyList( qsrVerifyStruct      * pVerify,
                struct psonLinkedList * pList )
{
   psonLinkNode * next, * previous;
   ptrdiff_t headOffset;
   struct repairKit kit = { \
      NO_REPAIR, pList, 0, 0, false, false, false, NULL, NULL, NULL, NULL };
   bool foundNode;
   enum qsrRecoverError rc = QSR_REC_OK;
   
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
      if ( next->nextOffset == PSON_NULL_OFFSET ) {
         kit.nextBreak = next;
         kit.nextBreakPrevious = previous;
         kit.breakInForwardChain = true;
         break;
      }
      else {
         if ( qsrVerifyOffset( pVerify, next->nextOffset ) ) {
            kit.forwardChainLen++;
         }
         else {
            qsrEcho( pVerify, "Invalid offset - cannot repair" );
            return QSR_REC_UNRECOVERABLE_ERROR;
         }
      }
      previous = next;
      GET_PTR( next, next->nextOffset, psonLinkNode );
   }

   // Same as before but we loop backward.
   next = &pList->head;
   previous = next;
   while ( next->previousOffset != headOffset ) {
      if ( next->previousOffset == PSON_NULL_OFFSET ) {
         kit.previousBreak = next;
         kit.previousBreakPrevious = previous;
         kit.breakInBackwardChain = true;
         break;
      }
      else {
         if ( qsrVerifyOffset( pVerify, next->previousOffset ) ) {
            kit.backwardChainLen++;
         }
         else {
            qsrEcho( pVerify, "Invalid offset - cannot repair" );
            return QSR_REC_UNRECOVERABLE_ERROR;
         }
      }
      previous = next;
      GET_PTR( next, next->previousOffset, psonLinkNode );
   }
   
   // So how did it go?... we have multiple possibilities here.
   if ( kit.breakInForwardChain && kit.breakInBackwardChain ) {
      rc = QSR_REC_CHANGES;
      qsrEcho( pVerify, "Both chains broken" );
      if ( kit.nextBreak == kit.previousBreak ) {
         kit.mode = REPAIR_WITH_BOTH_EQUAL;
      }
      else {
         qsrEcho( pVerify, "Chains broken at different places - cannot repair" );
         return QSR_REC_UNRECOVERABLE_ERROR;
      }
   }
   else if ( kit.breakInForwardChain && ! kit.breakInBackwardChain ) {
      rc = QSR_REC_CHANGES;
      kit.mode = REPAIR_WITH_BW_BREAK;
      qsrEcho( pVerify, "Forward chain broken" );
   }
   else if ( ! kit.breakInForwardChain && kit.breakInBackwardChain ) {
      rc = QSR_REC_CHANGES;
      kit.mode = REPAIR_WITH_FW_BREAK;
      qsrEcho( pVerify, "Backward chain broken" );
   }
   else {
      if ( kit.backwardChainLen == kit.forwardChainLen ) { /* all is well ! */
         if ( pList->currentSize != kit.forwardChainLen ) {
            rc = QSR_REC_CHANGES;
            kit.mode = REPAIR_LENGTH;
            qsrEcho( pVerify, "Invalid number of elements in linked list");
         }
      }
      else {
         qsrEcho( pVerify, "Warning - counts in foward and backward chains differ:" );
         qsrEcho( pVerify, "          using the longest chain to rebuild" );
         rc = QSR_REC_CHANGES;
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
      qsrEcho( pVerify, "Small correction in the number of elements of linked list");
      pList->currentSize = kit.forwardChainLen;
      break;
         
   case REPAIR_WITH_FW_NO_BREAK:
   
      qsrEcho( pVerify, "Repairing list using forward chain" );
      
      /*
       * We loop forward until we come back to head. We reset the bw chain
       * using brute force (finding which node is present in one chain and
       * missing in the other one would likely consume more cpu cycles...).
       */
      previous = &pList->head;
      do {
         GET_PTR( next, previous->nextOffset, psonLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         /* prepare for next round */
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.forwardChainLen;
      break;
      
   case REPAIR_WITH_BW_NO_BREAK:
      
      qsrEcho( pVerify, "Repairing list using backward chain" );
      
      /*
       * We loop backward until we come back to head. We reset the fw chain
       * using brute force (finding which node is present in one chain and
       * missing in the other one would likely consume more cpu cycles...).
       */
      previous = &pList->head;
      do {
         GET_PTR( next, previous->previousOffset, psonLinkNode );
         next->nextOffset = SET_OFFSET( previous );

         /* prepare for next round */
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.backwardChainLen;
      break;
      
   case REPAIR_WITH_FW_BREAK:
   
      qsrEcho( pVerify, "Repairing list using forward chain" );
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
         GET_PTR( next, previous->nextOffset, psonLinkNode );
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
         GET_PTR( previous, kit.previousBreakPrevious->previousOffset, psonLinkNode );
         
         kit.previousBreak->nextOffset = SET_OFFSET( kit.previousBreakPrevious );
         kit.previousBreak->previousOffset = SET_OFFSET( previous );
         previous->nextOffset = SET_OFFSET( kit.previousBreak );
         kit.previousBreakPrevious->previousOffset = SET_OFFSET( kit.previousBreak );
         
         pList->currentSize = kit.forwardChainLen + 1;
      }
      
      break;
      
   case REPAIR_WITH_BW_BREAK:
      
      qsrEcho( pVerify, "Repairing list using backward chain" );
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
         GET_PTR( next, previous->previousOffset, psonLinkNode );
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
         GET_PTR( next, kit.nextBreakPrevious->nextOffset, psonLinkNode );
         
         kit.nextBreak->previousOffset = SET_OFFSET( kit.nextBreakPrevious );
         kit.nextBreak->nextOffset = SET_OFFSET( next );
         next->previousOffset = SET_OFFSET( kit.nextBreak );
         kit.nextBreakPrevious->nextOffset = SET_OFFSET( kit.nextBreak );
         
         pList->currentSize = kit.backwardChainLen + 1;
      }
      
      break;
      
   case REPAIR_WITH_BOTH_EQUAL:
   
      qsrEcho( pVerify, "Repairing both chains" );
      
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

