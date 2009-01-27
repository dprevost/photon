/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
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

inline
bool psonLinkedListGetFirst( psonLinkedList  * pList,
                             psonLinkNode   ** ppItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty queue. */
   if ( pList->currentSize == 0 ) return false;

   /* Get the pointer to the first node */
   *ppItem = GET_PTR_FAST( pList->head.nextOffset, psonLinkNode );

   /* Reset the next offset of the head and the previous offset
    * of the item after the item we are removing.
    */
   pList->head.nextOffset = (*ppItem)->nextOffset;
   GET_PTR_FAST( (*ppItem)->nextOffset, psonLinkNode)->previousOffset = 
      SET_OFFSET( &pList->head );

   --pList->currentSize;

   PSO_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psonLinkedListGetLast( psonLinkedList  * pList,
                            psonLinkNode   ** ppItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 ) return false;

   /* Get the pointer to the last node */
   *ppItem = GET_PTR_FAST( pList->head.previousOffset, psonLinkNode );

   /* Reset the previous offset of the head and the next offset
    * of the item before the item we are removing.
    */   
   pList->head.previousOffset = (*ppItem)->previousOffset;
   GET_PTR_FAST( (*ppItem)->previousOffset, psonLinkNode)->nextOffset = 
      SET_OFFSET( &pList->head );

   --pList->currentSize;

   PSO_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
psonLinkedListPutLast( psonLinkedList * pList,
                       psonLinkNode   * pNewItem )
{
   ptrdiff_t tmpOffset;
   
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pNewItem   != NULL );
   PSO_PRE_CONDITION( pNewItem->previousOffset == PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pNewItem->nextOffset     == PSON_NULL_OFFSET );

   tmpOffset = SET_OFFSET( pNewItem );

   pNewItem->nextOffset     = SET_OFFSET( &pList->head );
   /* The order of the next two is important - don't change it! */
   pNewItem->previousOffset   = pList->head.previousOffset;
   pList->head.previousOffset = tmpOffset;

   GET_PTR_FAST( pNewItem->previousOffset, psonLinkNode )->nextOffset = 
      tmpOffset;
   
   pList->currentSize++;

   PSO_POST_CONDITION( pNewItem->previousOffset != PSON_NULL_OFFSET );
   PSO_POST_CONDITION( pNewItem->nextOffset     != PSON_NULL_OFFSET );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
psonLinkedListPutFirst( psonLinkedList * pList,
                        psonLinkNode   * pNewItem )
{
   ptrdiff_t tmpOffset;
   
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pNewItem   != NULL );
   PSO_PRE_CONDITION( pNewItem->previousOffset == PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pNewItem->nextOffset     == PSON_NULL_OFFSET );

   tmpOffset = SET_OFFSET( pNewItem );

   pNewItem->previousOffset = SET_OFFSET( &pList->head );

   /* The order of the next two is important - don't change it! */
   pNewItem->nextOffset = pList->head.nextOffset;   
   pList->head.nextOffset = tmpOffset;

   GET_PTR_FAST( pNewItem->nextOffset, psonLinkNode )->previousOffset = 
      tmpOffset;
   
   pList->currentSize++;

   PSO_POST_CONDITION( pNewItem->previousOffset != PSON_NULL_OFFSET );
   PSO_POST_CONDITION( pNewItem->nextOffset     != PSON_NULL_OFFSET );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
psonLinkedListRemoveItem( psonLinkedList * pList,
                          psonLinkNode   * pRemovedItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pRemovedItem != NULL );
   PSO_PRE_CONDITION( pRemovedItem->previousOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pRemovedItem->nextOffset     != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pList->currentSize > 0 );

   GET_PTR_FAST( pRemovedItem->nextOffset, psonLinkNode )->previousOffset = 
      pRemovedItem->previousOffset;

   GET_PTR_FAST( pRemovedItem->previousOffset, psonLinkNode )->nextOffset = 
      pRemovedItem->nextOffset;

   --pList->currentSize;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psonLinkedListPeakFirst( psonLinkedList *  pList,
                              psonLinkNode   ** ppItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 ) return false;

   *ppItem = GET_PTR_FAST( pList->head.nextOffset, psonLinkNode );

   PSO_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psonLinkedListPeakLast( psonLinkedList  * pList,
                             psonLinkNode   ** ppItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 ) return false;

   *ppItem = GET_PTR_FAST( pList->head.previousOffset, psonLinkNode );

   PSO_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psonLinkedListPeakNext( psonLinkedList * pList,
                             psonLinkNode   * pCurrent, 
                             psonLinkNode  ** ppNext )
{
   psonLinkNode* pNext;

   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pCurrent   != NULL );
   PSO_PRE_CONDITION( ppNext     != NULL );
   PSO_PRE_CONDITION( pCurrent->previousOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pCurrent->nextOffset     != PSON_NULL_OFFSET );

   pNext = GET_PTR_FAST( pCurrent->nextOffset, psonLinkNode );
   if ( pNext == &pList->head ) return false;

   *ppNext = pNext;
   
   PSO_POST_CONDITION( *ppNext != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psonLinkedListPeakPrevious( psonLinkedList * pList,
                                 psonLinkNode   * pCurrent, 
                                 psonLinkNode  ** ppPrevious )
{
   psonLinkNode* pPrevious;

   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pCurrent   != NULL );
   PSO_PRE_CONDITION( ppPrevious != NULL );
   PSO_PRE_CONDITION( pCurrent->previousOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pCurrent->nextOffset     != PSON_NULL_OFFSET );

   pPrevious = GET_PTR_FAST( pCurrent->previousOffset, psonLinkNode );
   if ( pPrevious == &pList->head ) return false;

   *ppPrevious = pPrevious;

   PSO_POST_CONDITION( *ppPrevious != NULL );
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Replace the item pointed to by pOldItem with the item pNewItem (this 
 * assumes that pOldItem is in the list... otherwise big trouble!)
 */
inline void 
psonLinkedListReplaceItem( psonLinkedList * pList,
                           psonLinkNode   * pOldItem,
                           psonLinkNode   * pNewItem )
{
   ptrdiff_t tmpOffset;

   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSON_LIST_SIGNATURE );
   PSO_PRE_CONDITION( pOldItem != NULL );
   PSO_PRE_CONDITION( pOldItem->previousOffset != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pOldItem->nextOffset     != PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pNewItem != NULL );
   PSO_PRE_CONDITION( pNewItem->previousOffset == PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pNewItem->nextOffset     == PSON_NULL_OFFSET );
   PSO_PRE_CONDITION( pList->currentSize > 0 );

   tmpOffset = SET_OFFSET( pNewItem );
   pNewItem->nextOffset     = pOldItem->nextOffset;
   pNewItem->previousOffset = pOldItem->previousOffset;

   GET_PTR_FAST( pOldItem->nextOffset, psonLinkNode )->previousOffset = 
      tmpOffset;

   GET_PTR_FAST( pOldItem->previousOffset, psonLinkNode )->nextOffset = 
      tmpOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

