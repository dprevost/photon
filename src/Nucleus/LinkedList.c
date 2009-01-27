/*
 * Copyright (C) 2006-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#include "LinkedList.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Cleanup the list once we're done with it. */
void psonLinkedListFini( psonLinkedList* pList )
{   
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   
   /* We reset the node element to PSON_NULL_OFFSET. */
   psonLinkNodeInit( &pList->head );

   pList->currentSize    = 0;
   pList->initialized    = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonLinkedListInit( psonLinkedList* pList )
{
   PSO_PRE_CONDITION( pList != NULL );
   
   psonLinkNodeInit( &pList->head );
   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );

   pList->initialized = PSON_LIST_SIGNATURE;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

void psonLinkedListReset( psonLinkedList* pList )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );

   pList->currentSize = 0;

   /* Make the list circular by pointing it back to itself. */
   pList->head.previousOffset = pList->head.nextOffset = 
      SET_OFFSET( &pList->head );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

bool psonLinkedListIsValid( psonLinkedList* pList,
                            psonLinkNode*   pUnknown )
{
   bool valid = false;
   
   psonLinkNode* pItem;

   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pUnknown   != NULL );

   pItem = &pList->head;
   
   GET_PTR( pItem, pItem->nextOffset, psonLinkNode );
   while ( pItem != &pList->head ) {
      if ( pItem == pUnknown ) {
         valid = true;
         break;
      }
      
      GET_PTR( pItem, pItem->nextOffset, psonLinkNode );
   }

   return valid;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

