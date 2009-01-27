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

#ifndef PSON_DLINKED_LIST_H
#define PSON_DLINKED_LIST_H

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
#define PSON_LIST_SIGNATURE  ((unsigned int)0x7a3dc233)

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
struct psonLinkedList
{
   /** The dummy node of the circular linked list. */
   psonLinkNode head;

   /** Current size of the list. */
   size_t currentSize;

   /** Set to PSON_LIST_SIGNATURE at initialization. */
   unsigned int initialized;
   
};

typedef struct psonLinkedList psonLinkedList;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/** Cleanup the list once we're done with it. */
PHOTON_ENGINE_EXPORT void 
psonLinkedListFini( psonLinkedList * pList );

/** Removes and returns the first item on the list. */
static inline 
bool psonLinkedListGetFirst( psonLinkedList  * pList,
                             psonLinkNode   ** ppItem );

/** Removes and returns the last item on the list. */ 
static inline 
bool psonLinkedListGetLast( psonLinkedList  * pList,
                            psonLinkNode   ** ppItem );

/** Initialize the linked list. */
PHOTON_ENGINE_EXPORT 
void psonLinkedListInit( psonLinkedList * pList );

/** Adds pNewItem to the beginning of the list. */
static inline 
void psonLinkedListPutFirst( psonLinkedList * pList,
                             psonLinkNode   * pNewItem );

/** Adds pNewItem to the tail of the list. */
static inline 
void psonLinkedListPutLast( psonLinkedList * pList,
                            psonLinkNode   * pNewItem );

/** Returns the first item on the list. The item is not removed. */
static inline
bool psonLinkedListPeakFirst( psonLinkedList  * pList,
                              psonLinkNode   ** ppItem );

/** Returns the last item on the list. The item is not removed. */
static inline
bool psonLinkedListPeakLast( psonLinkedList  * pList,
                             psonLinkNode   ** ppItem );

/** Returns the item just after pCurrent. The item is not removed. */
static inline
bool psonLinkedListPeakNext( psonLinkedList * pList, 
                             psonLinkNode   * pCurrent, 
                             psonLinkNode  ** ppNext );

/** Returns the item just before pCurrent. The item is not removed. */
static inline
bool psonLinkedListPeakPrevious( psonLinkedList * pList,
                                 psonLinkNode   * pCurrent, 
                                 psonLinkNode  ** ppPrevious );

/** 
 * Remove the item pointed to by pRemovedItem from the list (this 
 * assumes that pRemovedItem is in the list... otherwise big trouble!)
 */
static inline
void psonLinkedListRemoveItem( psonLinkedList * pList,
                               psonLinkNode   * pRemovedItem );

/** 
 * Replace the item pointed to by pOldItem with the item pNewItem (this 
 * assumes that pOldItem is in the list... otherwise big trouble!)
 */
static inline 
void psonLinkedListReplaceItem( psonLinkedList * pList,
                                psonLinkNode   * pOldItem,
                                psonLinkNode   * pNewItem );

/** Search in the list to see if pUnknown is in it or not - used by the
 *  crash recovery algorithm, psonFree, etc. 
 */
PHOTON_ENGINE_EXPORT 
bool psonLinkedListIsValid( psonLinkedList * pList,
                            psonLinkNode   * pUnknown );

/*
 * The next functions are part of the recovery algorithm and should not
 * be used in any other situations!
 */

/** Reset the list to be empty - used by the
 *  crash recovery algorithm
 */
PHOTON_ENGINE_EXPORT 
void psonLinkedListReset( psonLinkedList * pList );


/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "LinkedList.inl"

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_DLINKED_LIST_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

