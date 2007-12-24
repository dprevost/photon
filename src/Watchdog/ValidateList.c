/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "Watchdog/Validate.h"
#include "Watchdog/ValidateList.h"

#include "Engine/LinkedList.h"
#include "Engine/LinkNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum repairMode
{
   NO_REPAIR,
   REPAIR_WITH_FW,
   REPAIR_WITH_BW,
   REPAIR_WITH_BOTH
};

struct repairKit
{
   enum repairMode mode;
   struct vdseLinkedList * pList;
   size_t forwardChainLen;
   size_t backwardChainLen;;
   bool breakInForwardChain;
   bool breakInBackwardChain;

};

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#if 0
int repairList( struct vdseLinkedList * pList, struct repairKit * kit )
{
   vdseLinkNode * next, * previous;
   ptrdiff_t headOffset;

   // So how did it go?... we have multiple possibilities here.
   if ( ! kit->breakInForwardChain && ! kit->breakInBackwardChain )
   {
      if ( kit->backwardChainLen == kit->forwardChainLen ) // all is well !
      {
         if ( pList->currentSize != kit->forwardChainLen )         
         {
            vdswEcho( spaces, "Small correction in the number of elements of linked list");
            pList->currentSize = kit->forwardChainLen;
         }
         return 0;
      }
      vdswEcho( spaces, "Warning - counts in foward and backward chains differ:" );
      vdswEcho( spaces, "          using the longest chain to rebuild" );
      if ( kit->backwardChainLen > kit->forwardChainLen )
         kit->breakInForwardChain = true;
      else
         kit->breakInBackwardChain = true;
   }
   
   if ( kit->breakInForwardChain && kit->breakInBackwardChain )
   {
      vdswEcho( spaces, "Both chains broken - don't know how to repair" );
      return -1;
   }
   
   if ( kit->breakInBackwardChain )
   {
      vdswEcho( spaces, "Repairing list using forward chain" );
      // We loop forward until we come back to head.
      
      previous = &pList->head;
      do
      {
         GET_PTR( next, previous->nextOffset, vdseLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit->forwardChainLen;
   }

   if ( kit->breakInForwardChain )
   {
      vdswEcho( spaces, "Repairing list using backward chain" );
      // We loop backward until we come back to head.
      
      previous = &pList->head;
      do
      {
         GET_PTR( next, previous->previousOffset, vdseLinkNode );
         next->nextOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit->backwardChainLen;
   }
   
   return 0;
}
#endif

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum vdswValidation 
vdswValidateList( struct vdseLinkedList * pList, int verbose, int spaces )
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
      if ( vdswValidateOffset( next->nextOffset ) )
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
      if ( vdswValidateOffset( next->previousOffset ) )
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
   if ( ! kit.breakInForwardChain && ! kit.breakInBackwardChain )
   {
      if ( kit.backwardChainLen == kit.forwardChainLen ) // all is well !
      {
         if ( pList->currentSize != kit.forwardChainLen )         
         {
            vdswEcho( spaces, "Small correction in the number of elements of linked list");
            pList->currentSize = kit.forwardChainLen;
         }
         return 0;
      }
      vdswEcho( spaces, "Warning - counts in foward and backward chains differ:" );
      vdswEcho( spaces, "          using the longest chain to rebuild" );
      if ( kit.backwardChainLen > kit.forwardChainLen )
         kit.breakInForwardChain = true;
      else
         kit.breakInBackwardChain = true;
   }
   
   if ( kit.breakInForwardChain && kit.breakInBackwardChain )
   {
      vdswEcho( spaces, "Both chains broken - don't know how to repair" );
      return -1;
   }
   
   if ( kit.breakInBackwardChain )
   {
      vdswEcho( spaces, "Repairing list using forward chain" );
      // We loop forward until we come back to head.
      
      previous = &pList->head;
      do
      {
         GET_PTR( next, previous->nextOffset, vdseLinkNode );
         next->previousOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.forwardChainLen;
   }

   if ( kit.breakInForwardChain )
   {
      vdswEcho( spaces, "Repairing list using backward chain" );
      // We loop backward until we come back to head.
      
      previous = &pList->head;
      do
      {
         GET_PTR( next, previous->previousOffset, vdseLinkNode );
         next->nextOffset = SET_OFFSET( previous );
         // prepare for next round
         previous = next;
         
      } while ( next != &pList->head );
      
      pList->currentSize = kit.backwardChainLen;
   }
   
   return 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

