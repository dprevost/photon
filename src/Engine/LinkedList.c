/*
 * Copyright (C) 2006 Daniel Prevost <dprevost@users.sourceforge.net>
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

#include "LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Cleanup the list once we're done with it. */
void vdseLinkedListFini( vdseLinkedList* pList )
{   
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   
   /* We reset the node element to NULL_OFFSET. */
   vdseLinkNodeInit( &pList->head );

   pList->currentSize    = 0;
   pList->initialized    = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseLinkedListInit( vdseLinkedList* pList )
{
   VDS_PRE_CONDITION( pList != NULL );
   
   vdseLinkNodeInit( &pList->head );
   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );

   pList->initialized = VDSE_LIST_SIGNATURE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void vdseLinkedListReset( vdseLinkedList* pList )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );

   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool vdseLinkedListIsValid( vdseLinkedList* pList,
                            vdseLinkNode*   pUnknown )
{
   bool valid = false;
   
   vdseLinkNode* pItem;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pUnknown   != NULL );

   pItem = &pList->head;
   
   pItem = GET_PTR( pItem->nextOffset, vdseLinkNode );
   while ( pItem != &pList->head )
   {
      if ( pItem == pUnknown )
      {
         valid = true;
         break;
      }
      
      pItem = GET_PTR( pItem->nextOffset, vdseLinkNode );
   }

   return valid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

