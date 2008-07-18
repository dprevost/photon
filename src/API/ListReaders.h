/*
 * Copyright (C) 2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSA_LIST_READERS_H
#define VDSA_LIST_READERS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "API/api.h"

#define VDSA_LIST_READER_SIG 0x1e216507

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdsaReader
{
   struct vdsaReader * previous;
   struct vdsaReader * next;
   
   /** Pointer to the vdsa* object */
   void * address;
   
   /** For the future: we'll likely have more than one type of read-only. */
   enum vdsaObjetType type;
};

typedef struct vdsaReader vdsaReader;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct vdsaListReaders
{
   size_t currentSize;

   vdsaReader head;

   /** Set to VDSA_LIST_READER_SIG at initialization. */
   unsigned int initialized;
};

typedef struct vdsaListReaders vdsaListReaders;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdsaListReadersFini( vdsaListReaders * pList )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSA_LIST_READER_SIG );

   pList->head.previous = pList->head.next = &pList->head;
   pList->currentSize = 0;
   pList->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdsaListReadersInit( vdsaListReaders * pList )
{
   VDS_PRE_CONDITION( pList != NULL );

   pList->head.previous = pList->head.next = &pList->head;
   pList->currentSize = 0;
   pList->initialized = VDSA_LIST_READER_SIG;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdsaListReadersPeakFirst( vdsaListReaders * pList,
                               vdsaReader     ** ppItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSA_LIST_READER_SIG );
   VDS_PRE_CONDITION( ppItem != NULL );

   /* Check for empty queue. */
   if ( pList->currentSize == 0 ) return false;

   /* Get the pointer to the first node */
   *ppItem = pList->head.next;

   VDS_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool vdsaListReadersPeakNext( vdsaListReaders * pList,
                              vdsaReader      * pCurrent, 
                              vdsaReader     ** ppNext )
{
   vdsaReader * pNext;

   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSA_LIST_READER_SIG );
   VDS_PRE_CONDITION( pCurrent   != NULL );
   VDS_PRE_CONDITION( ppNext     != NULL );
   VDS_PRE_CONDITION( pCurrent->previous != NULL );
   VDS_PRE_CONDITION( pCurrent->next     != NULL );

   pNext = pCurrent->next;
   if ( pNext == &pList->head ) return false;

   *ppNext = pNext;
   
   VDS_POST_CONDITION( *ppNext != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdsaListReadersPut( vdsaListReaders * pList,
                         vdsaReader      * pNewItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSA_LIST_READER_SIG );
   VDS_PRE_CONDITION( pNewItem != NULL );

   pNewItem->next = &pList->head;
   /* The order of the next two is important - don't change it! */
   pNewItem->previous = pList->head.previous;
   pList->head.previous = pNewItem;

   pNewItem->previous->next = pNewItem;
   
   pList->currentSize++;

   VDS_POST_CONDITION( pNewItem->previous != NULL );
   VDS_POST_CONDITION( pNewItem->next     != NULL );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void vdsaListReadersRemove( vdsaListReaders * pList,
                            vdsaReader      * pRemovedItem )
{
   VDS_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   VDS_INV_CONDITION( pList->initialized == VDSA_LIST_READER_SIG );
   VDS_PRE_CONDITION( pRemovedItem != NULL );
   VDS_PRE_CONDITION( pRemovedItem->previous != NULL );
   VDS_PRE_CONDITION( pRemovedItem->next     != NULL );
   VDS_PRE_CONDITION( pList->currentSize > 0 );

   pRemovedItem->next->previous = pRemovedItem->previous;
   pRemovedItem->previous->next = pRemovedItem->next;

   --pList->currentSize;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSA_LIST_READERS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
