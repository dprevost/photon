/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
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

inline
bool psnLinkedListGetFirst( psnLinkedList  * pList,
                             psnLinkNode   ** ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty queue. */
   if ( pList->currentSize == 0 ) return false;

   /* Get the pointer to the first node */
   *ppItem = GET_PTR_FAST( pList->head.nextOffset, psnLinkNode );

   /* Reset the next offset of the head and the previous offset
    * of the item after the item we are removing.
    */
   pList->head.nextOffset = (*ppItem)->nextOffset;
   GET_PTR_FAST( (*ppItem)->nextOffset, psnLinkNode)->previousOffset = 
      SET_OFFSET( &pList->head );

   --pList->currentSize;

   VDS_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psnLinkedListGetLast( psnLinkedList  * pList,
                            psnLinkNode   ** ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 ) return false;

   /* Get the pointer to the last node */
   *ppItem = GET_PTR_FAST( pList->head.previousOffset, psnLinkNode );

   /* Reset the previous offset of the head and the next offset
    * of the item before the item we are removing.
    */   
   pList->head.previousOffset = (*ppItem)->previousOffset;
   GET_PTR_FAST( (*ppItem)->previousOffset, psnLinkNode)->nextOffset = 
      SET_OFFSET( &pList->head );

   --pList->currentSize;

   VDS_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
psnLinkedListPutLast( psnLinkedList * pList,
                       psnLinkNode   * pNewItem )
{
   ptrdiff_t tmpOffset;
   
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pNewItem   != NULL );
   VDS_PRE_CONDITION( pNewItem->previousOffset == PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pNewItem->nextOffset     == PSN_NULL_OFFSET );

   tmpOffset = SET_OFFSET( pNewItem );

   pNewItem->nextOffset     = SET_OFFSET( &pList->head );
   /* The order of the next two is important - don't change it! */
   pNewItem->previousOffset   = pList->head.previousOffset;
   pList->head.previousOffset = tmpOffset;

   GET_PTR_FAST( pNewItem->previousOffset, psnLinkNode )->nextOffset = 
      tmpOffset;
   
   pList->currentSize++;

   VDS_POST_CONDITION( pNewItem->previousOffset != PSN_NULL_OFFSET );
   VDS_POST_CONDITION( pNewItem->nextOffset     != PSN_NULL_OFFSET );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
psnLinkedListPutFirst( psnLinkedList * pList,
                        psnLinkNode   * pNewItem )
{
   ptrdiff_t tmpOffset;
   
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pNewItem   != NULL );
   VDS_PRE_CONDITION( pNewItem->previousOffset == PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pNewItem->nextOffset     == PSN_NULL_OFFSET );

   tmpOffset = SET_OFFSET( pNewItem );

   pNewItem->previousOffset = SET_OFFSET( &pList->head );

   /* The order of the next two is important - don't change it! */
   pNewItem->nextOffset = pList->head.nextOffset;   
   pList->head.nextOffset = tmpOffset;

   GET_PTR_FAST( pNewItem->nextOffset, psnLinkNode )->previousOffset = 
      tmpOffset;
   
   pList->currentSize++;

   VDS_POST_CONDITION( pNewItem->previousOffset != PSN_NULL_OFFSET );
   VDS_POST_CONDITION( pNewItem->nextOffset     != PSN_NULL_OFFSET );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
psnLinkedListRemoveItem( psnLinkedList * pList,
                          psnLinkNode   * pRemovedItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pRemovedItem != NULL );
   VDS_PRE_CONDITION( pRemovedItem->previousOffset != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pRemovedItem->nextOffset     != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pList->currentSize > 0 );

   GET_PTR_FAST( pRemovedItem->nextOffset, psnLinkNode )->previousOffset = 
      pRemovedItem->previousOffset;

   GET_PTR_FAST( pRemovedItem->previousOffset, psnLinkNode )->nextOffset = 
      pRemovedItem->nextOffset;

   --pList->currentSize;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psnLinkedListPeakFirst( psnLinkedList *  pList,
                              psnLinkNode   ** ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 ) return false;

   *ppItem = GET_PTR_FAST( pList->head.nextOffset, psnLinkNode );

   VDS_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psnLinkedListPeakLast( psnLinkedList  * pList,
                             psnLinkNode   ** ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 ) return false;

   *ppItem = GET_PTR_FAST( pList->head.previousOffset, psnLinkNode );

   VDS_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psnLinkedListPeakNext( psnLinkedList * pList,
                             psnLinkNode   * pCurrent, 
                             psnLinkNode  ** ppNext )
{
   psnLinkNode* pNext;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pCurrent   != NULL );
   VDS_PRE_CONDITION( ppNext     != NULL );
   VDS_PRE_CONDITION( pCurrent->previousOffset != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pCurrent->nextOffset     != PSN_NULL_OFFSET );

   pNext = GET_PTR_FAST( pCurrent->nextOffset, psnLinkNode );
   if ( pNext == &pList->head ) return false;

   *ppNext = pNext;
   
   VDS_POST_CONDITION( *ppNext != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline
bool psnLinkedListPeakPrevious( psnLinkedList * pList,
                                 psnLinkNode   * pCurrent, 
                                 psnLinkNode  ** ppPrevious )
{
   psnLinkNode* pPrevious;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pCurrent   != NULL );
   VDS_PRE_CONDITION( ppPrevious != NULL );
   VDS_PRE_CONDITION( pCurrent->previousOffset != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pCurrent->nextOffset     != PSN_NULL_OFFSET );

   pPrevious = GET_PTR_FAST( pCurrent->previousOffset, psnLinkNode );
   if ( pPrevious == &pList->head ) return false;

   *ppPrevious = pPrevious;

   VDS_POST_CONDITION( *ppPrevious != NULL );
   
   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Replace the item pointed to by pOldItem with the item pNewItem (this 
 * assumes that pOldItem is in the list... otherwise big trouble!)
 */
inline void 
psnLinkedListReplaceItem( psnLinkedList * pList,
                           psnLinkNode   * pOldItem,
                           psnLinkNode   * pNewItem )
{
   ptrdiff_t tmpOffset;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == PSN_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pOldItem != NULL );
   VDS_PRE_CONDITION( pOldItem->previousOffset != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pOldItem->nextOffset     != PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pNewItem != NULL );
   VDS_PRE_CONDITION( pNewItem->previousOffset == PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pNewItem->nextOffset     == PSN_NULL_OFFSET );
   VDS_PRE_CONDITION( pList->currentSize > 0 );

   tmpOffset = SET_OFFSET( pNewItem );
   pNewItem->nextOffset     = pOldItem->nextOffset;
   pNewItem->previousOffset = pOldItem->previousOffset;

   GET_PTR_FAST( pOldItem->nextOffset, psnLinkNode )->previousOffset = 
      tmpOffset;

   GET_PTR_FAST( pOldItem->previousOffset, psnLinkNode )->nextOffset = 
      tmpOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

