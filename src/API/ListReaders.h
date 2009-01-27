/*
 * Copyright (C) 2008-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSOA_LIST_READERS_H
#define PSOA_LIST_READERS_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Common/Common.h"
#include "API/api.h"

#define PSOA_LIST_READER_SIG 0x1e216507

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psoaReader
{
   struct psoaReader * previous;
   struct psoaReader * next;
   
   /** Pointer to the psoa* object */
   void * address;
   
   /** For the future: we'll likely have more than one type of read-only. */
   enum psoaObjetType type;
};

typedef struct psoaReader psoaReader;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psoaListReaders
{
   size_t currentSize;

   psoaReader head;

   /** Set to PSOA_LIST_READER_SIG at initialization. */
   unsigned int initialized;
};

typedef struct psoaListReaders psoaListReaders;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psoaListReadersFini( psoaListReaders * pList )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSOA_LIST_READER_SIG );

   pList->head.previous = pList->head.next = &pList->head;
   pList->currentSize = 0;
   pList->initialized = 0;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psoaListReadersInit( psoaListReaders * pList )
{
   PSO_PRE_CONDITION( pList != NULL );

   pList->head.previous = pList->head.next = &pList->head;
   pList->currentSize = 0;
   pList->initialized = PSOA_LIST_READER_SIG;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psoaListReadersPeakFirst( psoaListReaders * pList,
                              psoaReader     ** ppItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSOA_LIST_READER_SIG );
   PSO_PRE_CONDITION( ppItem != NULL );

   /* Check for empty queue. */
   if ( pList->currentSize == 0 ) return false;

   /* Get the pointer to the first node */
   *ppItem = pList->head.next;

   PSO_POST_CONDITION( *ppItem != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
bool psoaListReadersPeakNext( psoaListReaders * pList,
                             psoaReader      * pCurrent, 
                             psoaReader     ** ppNext )
{
   psoaReader * pNext;

   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSOA_LIST_READER_SIG );
   PSO_PRE_CONDITION( pCurrent   != NULL );
   PSO_PRE_CONDITION( ppNext     != NULL );
   PSO_PRE_CONDITION( pCurrent->previous != NULL );
   PSO_PRE_CONDITION( pCurrent->next     != NULL );

   pNext = pCurrent->next;
   if ( pNext == &pList->head ) return false;

   *ppNext = pNext;
   
   PSO_POST_CONDITION( *ppNext != NULL );

   return true;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psoaListReadersPut( psoaListReaders * pList,
                        psoaReader      * pNewItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSOA_LIST_READER_SIG );
   PSO_PRE_CONDITION( pNewItem != NULL );

   pNewItem->next = &pList->head;
   /* The order of the next two is important - don't change it! */
   pNewItem->previous = pList->head.previous;
   pList->head.previous = pNewItem;

   pNewItem->previous->next = pNewItem;
   
   pList->currentSize++;

   PSO_POST_CONDITION( pNewItem->previous != NULL );
   PSO_POST_CONDITION( pNewItem->next     != NULL );
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

static inline
void psoaListReadersRemove( psoaListReaders * pList,
                           psoaReader      * pRemovedItem )
{
   PSO_PRE_CONDITION( pList != NULL );
   /* Test to see if the list is initialized */
   PSO_INV_CONDITION( pList->initialized == PSOA_LIST_READER_SIG );
   PSO_PRE_CONDITION( pRemovedItem != NULL );
   PSO_PRE_CONDITION( pRemovedItem->previous != NULL );
   PSO_PRE_CONDITION( pRemovedItem->next     != NULL );
   PSO_PRE_CONDITION( pList->currentSize > 0 );

   pRemovedItem->next->previous = pRemovedItem->previous;
   pRemovedItem->previous->next = pRemovedItem->next;

   --pList->currentSize;
}

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSOA_LIST_READERS_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */
