/*
 * Copyright (C) 2006-2008 Daniel Prevost <dprevost@users.sourceforge.net>
 *
 * This file is part of Photon (photonsoftware.org).
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

#ifndef PSN_DLINKED_LIST_H
#define PSN_DLINKED_LIST_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/LinkNode.h"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Random generated signature... the first 8 bytes of the digest (md5)
 * of LinkedList.c at that time... The signature is used to indicate
 * that the struct was properly initialized.
 */
#define PSN_LIST_SIGNATURE  ((unsigned int)0x7a3dc233)

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
struct psnLinkedList
{
   /** The dummy node of the circular linked list. */
   psnLinkNode head;

   /** Current size of the list. */
   size_t currentSize;

   /** Set to PSN_LIST_SIGNATURE at initialization. */
   unsigned int initialized;
   
};

typedef struct psnLinkedList psnLinkedList;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Cleanup the list once we're done with it. */
PHOTON_ENGINE_EXPORT void 
psnLinkedListFini( psnLinkedList * pList );

/** Removes and returns the first item on the list. */
static inline 
bool psnLinkedListGetFirst( psnLinkedList  * pList,
                             psnLinkNode   ** ppItem );

/** Removes and returns the last item on the list. */ 
static inline 
bool psnLinkedListGetLast( psnLinkedList  * pList,
                            psnLinkNode   ** ppItem );

/** Initialize the linked list. */
PHOTON_ENGINE_EXPORT 
void psnLinkedListInit( psnLinkedList * pList );

/** Adds pNewItem to the beginning of the list. */
static inline 
void psnLinkedListPutFirst( psnLinkedList * pList,
                             psnLinkNode   * pNewItem );

/** Adds pNewItem to the tail of the list. */
static inline 
void psnLinkedListPutLast( psnLinkedList * pList,
                            psnLinkNode   * pNewItem );

/** Returns the first item on the list. The item is not removed. */
static inline
bool psnLinkedListPeakFirst( psnLinkedList  * pList,
                              psnLinkNode   ** ppItem );

/** Returns the last item on the list. The item is not removed. */
static inline
bool psnLinkedListPeakLast( psnLinkedList  * pList,
                             psnLinkNode   ** ppItem );

/** Returns the item just after pCurrent. The item is not removed. */
static inline
bool psnLinkedListPeakNext( psnLinkedList * pList, 
                             psnLinkNode   * pCurrent, 
                             psnLinkNode  ** ppNext );

/** Returns the item just before pCurrent. The item is not removed. */
static inline
bool psnLinkedListPeakPrevious( psnLinkedList * pList,
                                 psnLinkNode   * pCurrent, 
                                 psnLinkNode  ** ppPrevious );

/** 
 * Remove the item pointed to by pRemovedItem from the list (this 
 * assumes that pRemovedItem is in the list... otherwise big trouble!)
 */
static inline
void psnLinkedListRemoveItem( psnLinkedList * pList,
                               psnLinkNode   * pRemovedItem );

/** 
 * Replace the item pointed to by pOldItem with the item pNewItem (this 
 * assumes that pOldItem is in the list... otherwise big trouble!)
 */
static inline 
void psnLinkedListReplaceItem( psnLinkedList * pList,
                                psnLinkNode   * pOldItem,
                                psnLinkNode   * pNewItem );

/** Search in the list to see if pUnknown is in it or not - used by the
 *  crash recovery algorithm, psnFree, etc. 
 */
PHOTON_ENGINE_EXPORT 
bool psnLinkedListIsValid( psnLinkedList * pList,
                            psnLinkNode   * pUnknown );

/*
 * The next functions are part of the recovery algorithm and should not
 * be used in any other situations!
 */

/** Reset the list to be empty - used by the
 *  crash recovery algorithm
 */
PHOTON_ENGINE_EXPORT 
void psnLinkedListReset( psnLinkedList * pList );


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "LinkedList.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_DLINKED_LIST_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

