/* -*- c -*- */
/* :mode=c:  - For jedit, previous line for emacs */
/*
 * Copyright (C) 2006, 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

inline enum ListErrors 
vdseLinkedListGetFirst( vdseLinkedList* pList,
                        vdseLinkNode**  ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty queue. */
   if ( pList->currentSize == 0 )
      return LIST_EMPTY;

   /* Get the pointer to the first node */
   *ppItem = GET_PTR( pList->head.nextOffset, vdseLinkNode );

   pList->currBuffOffset = pList->head.nextOffset;   

   /* Reset the next offset of the head and the previous offset
    * of the item after the item we are removing.
    */
   pList->head.nextOffset = (*ppItem)->nextOffset;
   GET_PTR( (*ppItem)->nextOffset, vdseLinkNode)->previousOffset = 
      SET_OFFSET( &pList->head );

   --pList->currentSize;

   VDS_POST_CONDITION( *ppItem != NULL );

   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline enum ListErrors 
vdseLinkedListGetLast( vdseLinkedList* pList,
                       vdseLinkNode**  ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 )
      return LIST_EMPTY;

   /* Get the pointer to the last node */
   *ppItem = GET_PTR( pList->head.previousOffset, vdseLinkNode );

   pList->currBuffOffset = pList->head.nextOffset;   
   
   /* Reset the previous offset of the head and the next offset
    * of the item before the item we are removing.
    */   pList->head.previousOffset = (*ppItem)->previousOffset;
   GET_PTR( (*ppItem)->previousOffset, vdseLinkNode)->nextOffset = 
      SET_OFFSET( &pList->head );

   --pList->currentSize;

   VDS_POST_CONDITION( *ppItem != NULL );

   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
vdseLinkedListPutLast( vdseLinkedList* pList,
                       vdseLinkNode *  pNewItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pNewItem   != NULL );

   pList->currBuffOffset = SET_OFFSET( pNewItem );

   pNewItem->nextOffset     = SET_OFFSET( &pList->head );
   /* The order of the next two is important - don't change it! */
   pNewItem->previousOffset   = pList->head.previousOffset;   
   pList->head.previousOffset = pList->currBuffOffset;
   GET_PTR( pNewItem->previousOffset, vdseLinkNode )->nextOffset = 
      pList->currBuffOffset;
   
   pList->currentSize++;

   pList->currBuffOffset = NULL_OFFSET;

   VDS_POST_CONDITION( pNewItem->previousOffset != NULL_OFFSET );
   VDS_POST_CONDITION( pNewItem->nextOffset     != NULL_OFFSET );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
vdseLinkedListPutFirst( vdseLinkedList* pList,
                        vdseLinkNode *  pNewItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pNewItem   != NULL );

   pList->currBuffOffset = SET_OFFSET( pNewItem );

   pNewItem->previousOffset = SET_OFFSET( &pList->head );

   /* The order of the next two is important - don't change it! */
   pNewItem->nextOffset = pList->head.nextOffset;   
   pList->head.nextOffset    = pList->currBuffOffset;
   GET_PTR( pNewItem->nextOffset, vdseLinkNode )->previousOffset = 
      pList->currBuffOffset;
   
   pList->currentSize++;

   pList->currBuffOffset = NULL_OFFSET;

   VDS_POST_CONDITION( pNewItem->previousOffset != NULL_OFFSET );
   VDS_POST_CONDITION( pNewItem->nextOffset     != NULL_OFFSET );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline void 
vdseLinkedListRemoveItem( vdseLinkedList* pList,
                          vdseLinkNode*   pRemovedItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pRemovedItem != NULL );
   VDS_PRE_CONDITION( pRemovedItem->previousOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( pRemovedItem->nextOffset     != NULL_OFFSET );
   VDS_PRE_CONDITION( pList->currentSize > 0 );

#ifdef USE_EXTREME_DBC
   /* A more complex precondition, that the removed buffer is indeed
    * in the list... (it is also like that a memory violation will 
    * occur if the buffer is indeed not in the list).
    */
   {
      int dbc_count = pList->currentSize;
      int dbc_ok = 0;
      vdseLinkNode* dbc_ptr;

      dbc_ptr = GET_PTR( pRemovedItem->nextOffset,
                         vdseLinkNode );
      while ( dbc_count > 0 )
      {
         if ( dbc_ptr == &pList->head )
         {
            dbc_ok = 1;
            break;
         }
         VDS_PRE_CONDITION( dbc_ptr->nextOffset != NULL_OFFSET );
         dbc_ptr = GET_PTR( dbc_ptr->nextOffset, vdseLinkNode );
         dbc_count--;
         
      }
      VDS_PRE_CONDITION( dbc_ok == 1 );
   }
#endif

   pList->currBuffOffset = SET_OFFSET( pRemovedItem );
   
   GET_PTR( pRemovedItem->nextOffset, vdseLinkNode )->previousOffset = 
      pRemovedItem->previousOffset;

   GET_PTR( pRemovedItem->previousOffset, vdseLinkNode )->nextOffset = 
      pRemovedItem->nextOffset;

   --pList->currentSize;

   pList->currBuffOffset = NULL_OFFSET;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline enum ListErrors 
vdseLinkedListPeakFirst( vdseLinkedList* pList,
                         vdseLinkNode**  ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 )
      return LIST_EMPTY;

   *ppItem = GET_PTR( pList->head.nextOffset, vdseLinkNode );

   VDS_POST_CONDITION( *ppItem != NULL );

   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline enum ListErrors 
vdseLinkedListPeakLast( vdseLinkedList* pList,
                        vdseLinkNode**  ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( ppItem     != NULL );

   /* Check for empty list. */
   if ( pList->currentSize == 0 )
      return LIST_EMPTY;

   *ppItem = GET_PTR( pList->head.previousOffset, vdseLinkNode );

   VDS_POST_CONDITION( *ppItem != NULL );

   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline enum ListErrors 
vdseLinkedListPeakNext( vdseLinkedList* pList,
                        vdseLinkNode*   pCurrent, 
                        vdseLinkNode**  ppNext )
{
   vdseLinkNode* pNext;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pCurrent   != NULL );
   VDS_PRE_CONDITION( ppNext     != NULL );
   VDS_PRE_CONDITION( pCurrent->previousOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( pCurrent->nextOffset     != NULL_OFFSET );

#ifdef USE_EXTREME_DBC
   /* A more complex precondition, that the current buffer is indeed
    * in the list... (it is also like that a memory violation will 
    * occur if the buffer is not in the list).
    */
   {
      int dbc_count = pList->currentSize;
      int dbc_ok = 0;
      vdseLinkNode* dbc_ptr;

      dbc_ptr = GET_PTR( pCurrent->nextOffset, vdseLinkNode );
      while ( dbc_count > 0 )
      {
         if ( dbc_ptr == &pList->head )
         {
            dbc_ok = 1;
            break;
         }
         VDS_PRE_CONDITION( dbc_ptr->nextOffset != NULL_OFFSET );
         dbc_ptr = GET_PTR( dbc_ptr->nextOffset, vdseLinkNode );
         dbc_count--;
      }
      VDS_PRE_CONDITION( dbc_ok == 1 );
   }
#endif

   pNext = GET_PTR( pCurrent->nextOffset, vdseLinkNode );
   if ( pNext == &pList->head )
      return LIST_END_OF_LIST;
   *ppNext = pNext;
   
   VDS_POST_CONDITION( *ppNext != NULL );

   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

inline enum ListErrors 
vdseLinkedListPeakPrevious( vdseLinkedList* pList,
                            vdseLinkNode*   pCurrent, 
                            vdseLinkNode**  ppPrevious )
{
   vdseLinkNode* pPrevious;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pCurrent   != NULL );
   VDS_PRE_CONDITION( ppPrevious != NULL );
   VDS_PRE_CONDITION( pCurrent->previousOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( pCurrent->nextOffset     != NULL_OFFSET );

#ifdef USE_EXTREME_DBC
   /* A more complex precondition, that the current buffer is indeed
    * in the list... (it is also like that a memory violation will 
    * occur if the buffer is not in the list).
    */
   {
      int dbc_count = pList->currentSize;
      int dbc_ok = 0;
      vdseLinkNode* dbc_ptr;

      dbc_ptr = GET_PTR( pCurrent->nextOffset, vdseLinkNode );
      while ( dbc_count > 0 )
      {
         if ( dbc_ptr == &pList->head )
         {
            dbc_ok = 1;
            break;
         }
         VDS_PRE_CONDITION( dbc_ptr->nextOffset != NULL_OFFSET );
         dbc_ptr = GET_PTR( dbc_ptr->nextOffset, vdseLinkNode );
         dbc_count--;
      }
      VDS_PRE_CONDITION( dbc_ok == 1 );
   }
#endif

   pPrevious = GET_PTR( pCurrent->previousOffset, vdseLinkNode );
   if ( pPrevious == &pList->head )
      return LIST_END_OF_LIST;
   *ppPrevious = pPrevious;

   VDS_POST_CONDITION( *ppPrevious != NULL );
   
   return LIST_OK;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** 
 * Replace the item pointed to by pOldItem with the item pNewItem (this 
 * assumes that pOldItem is in the list... otherwise big trouble!)
 */
inline void 
vdseLinkedListReplaceItem( vdseLinkedList* pList,
                           vdseLinkNode*   pOldItem,
                           vdseLinkNode*   pNewItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSE_LIST_SIGNATURE );
   VDS_PRE_CONDITION( pOldItem != NULL );
   VDS_PRE_CONDITION( pOldItem->previousOffset != NULL_OFFSET );
   VDS_PRE_CONDITION( pOldItem->nextOffset     != NULL_OFFSET );
   VDS_PRE_CONDITION( pNewItem != NULL );
   VDS_PRE_CONDITION( pList->currentSize > 0 );

#ifdef USE_EXTREME_DBC
   /* A more complex precondition, that the removed buffer is indeed
    * in the list... (it is also likely that a memory violation will 
    * occur if the buffer is indeed not in the list).
    */
   {
      int dbc_count = pList->currentSize;
      int dbc_ok = 0;
      vdseLinkNode* dbc_ptr;

      dbc_ptr = GET_PTR( pOldItem->nextOffset,
                         vdseLinkNode );
      while ( dbc_count > 0 )
      {
         if ( dbc_ptr == &pList->head )
         {
            dbc_ok = 1;
            break;
         }
         VDS_PRE_CONDITION( dbc_ptr->nextOffset != NULL_OFFSET );
         dbc_ptr = GET_PTR( dbc_ptr->nextOffset, vdseLinkNode );
         dbc_count--;
         
      }
      VDS_PRE_CONDITION( dbc_ok == 1 );
   }
#endif

   pList->currBuffOffset = SET_OFFSET( pNewItem );
   pNewItem->nextOffset     = pOldItem->nextOffset;
   pNewItem->previousOffset = pOldItem->previousOffset;

   GET_PTR( pOldItem->nextOffset, vdseLinkNode )->previousOffset = 
      pList->currBuffOffset;

   GET_PTR( pOldItem->previousOffset, vdseLinkNode )->nextOffset = 
      pList->currBuffOffset;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

