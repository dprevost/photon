/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Cleanup the list once we're done with it. */
void psnLinkedListFini( psnLinkedList* pList )
{   
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   
   /* We reset the node element to PSN_NULL_OFFSET. */
   psnLinkNodeInit( &pList->head );

   pList->currentSize    = 0;
   pList->initialized    = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnLinkedListInit( psnLinkedList* pList )
{
   VDS_PRE_CONDITION( pList != NULL );
   
   psnLinkNodeInit( &pList->head );
   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );

   pList->initialized = PSN_LIST_SIGNATURE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psnLinkedListReset( psnLinkedList* pList )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );

   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psnLinkedListIsValid( psnLinkedList* pList,
                            psnLinkNode*   pUnknown )
{
   bool valid = false;
   
   psnLinkNode* pItem;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pUnknown   != NULL );

   pItem = &pList->head;
   
   GET_PTR( pItem, pItem->nextOffset, psnLinkNode );
   while ( pItem != &pList->head ) {
      if ( pItem == pUnknown ) {
         valid = true;
         break;
      }
      
      GET_PTR( pItem, pItem->nextOffset, psnLinkNode );
   }

   return valid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

