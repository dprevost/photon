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

#ifndef VDSE_DLINKED_LIST_H
#define VDSE_DLINKED_LIST_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "ListErrors.h"
#include "LinkNode.h"
#include "MemoryAllocator.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Random generated signature... the first 8 bytes of the digest (md5)
 * of LinkedList.c at that time... The signature is used to indicate
 * that the struct was properly initialized.
 */
#define VDSE_LIST_SIGNATURE  ((unsigned int)0x7a3dc233)

/**
 * This module implements a simple and efficient doubled linked list.
 * The linked list is circular and build around a dummy node.
 *
 * Notes:
 * 1) The linked-list functions do not allocate memory (or free it). This 
 *    task is left to the "object" owning/using the list.
 * 2) They will not do synchronisation (locking). Again, this task is left
 *    to the object owning/using the list.
 */	

typedef struct vdseLinkedList
{
   /** The dummy node of the circular linked list. */
   vdseLinkNode head;

   /** Current size of the list. */
   size_t currentSize;

   /* Crash recovery attributes */

   ptrdiff_t   currBuffOffset;

   /** Set to VDSE_LIST_SIGNATURE at initialization. */
   unsigned int initialized;
   
} vdseLinkedList;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Cleanup the list once we're done with it. */
void 
vdseLinkedListFini( vdseLinkedList* pList );

/** Removes and returns the first item on the list. */
static inline enum ListErrors 
vdseLinkedListGetFirst( vdseLinkedList* pList,
                        vdseLinkNode**  ppItem );

/** Removes and returns the last item on the list. */ 
static inline enum ListErrors 
vdseLinkedListGetLast( vdseLinkedList* pList,
                       vdseLinkNode**  ppItem );

/** Initialize the linked list. */
void 
vdseLinkedListInit( vdseLinkedList* pList );

/** Adds pNewItem to the beginning of the list. */
static inline void 
vdseLinkedListPutFirst( vdseLinkedList* pList,
                        vdseLinkNode *  pNewItem );

/** Adds pNewItem to the tail of the list. */
static inline void 
vdseLinkedListPutLast( vdseLinkedList* pList,
                       vdseLinkNode *  pNewItem );

/** Returns the first item on the list. The item is not removed. */
static inline enum ListErrors 
vdseLinkedListPeakFirst( vdseLinkedList* pList,
                         vdseLinkNode**  ppItem );

/** Returns the last item on the list. The item is not removed. */
static inline enum ListErrors 
vdseLinkedListPeakLast( vdseLinkedList* pList,
                        vdseLinkNode**  ppItem );

/** Returns the item just after pCurrent. The item is not removed. */
static inline enum ListErrors 
vdseLinkedListPeakNext( vdseLinkedList* pList, 
                        vdseLinkNode*   pCurrent, 
                        vdseLinkNode**  ppNext );

/** Returns the item just before pCurrent. The item is not removed. */
static inline enum ListErrors 
vdseLinkedListPeakPrevious( vdseLinkedList* pList,
                            vdseLinkNode*   pCurrent, 
                            vdseLinkNode**  ppPrevious );

/** 
 * Remove the item pointed to by pRemovedItem from the list (this 
 * assumes that pRemovedItem is in the list... otherwise big trouble!)
 */
static inline void 
vdseLinkedListRemoveItem( vdseLinkedList* pList,
                          vdseLinkNode*   pRemovedItem );

/*
 * The next functions are part of the recovery algorithm and should not
 * be used in any other situations!
 */

/** Reset the list to be empty - used by the
 *  crash recovery algorithm
 */
void 
vdseLinkedListReset( vdseLinkedList* pList );

/** Search in the list to see if pUnknown is in it or not - used by the
 *  crash recovery algorithm 
 */
int 
vdseLinkedListIsValid( vdseLinkedList* pList,
                       vdseLinkNode*   pUnknown );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "LinkedList.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_DLINKED_LIST_H */
