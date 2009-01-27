/*
 * Copyright (C) 2007-2009 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_TX_H
#define PSON_TX_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Nucleus/Engine.h"
#include "Nucleus/LinkedList.h"
#include "Nucleus/SessionContext.h"
#include "Nucleus/MemoryObject.h"
#include "Nucleus/BlockGroup.h"
#include "Nucleus/Hash.h"

#define PSON_TX_SIGNATURE 0xabc6c981

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonTxType
{
   /* ops on data */
   PSON_TX_ADD_DATA = 1,
   PSON_TX_REMOVE_DATA,

   /* ops on objects */
   PSON_TX_ADD_OBJECT = 0x81,
   PSON_TX_REMOVE_OBJECT,
   PSON_TX_ADD_EDIT_OBJECT

};

typedef enum psonTxType psonTxType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Each element of a transaction is kept in a doubly linked list
 * (for fast access). 
 */
struct psonTxOps
{
   psonTxType      transType;
   ptrdiff_t       parentOffset;  
   psonMemObjIdent parentType;
   ptrdiff_t       childOffset;
   psonMemObjIdent childType;

   psonLinkNode node;
   
};

typedef struct psonTxOps psonTxOps;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psonTx
{
   /** Always first */
   struct psonMemObject memObject;

   int signature;

   /** Linked list of all ops of the current transaction */   
   psonLinkedList listOfOps;

   /**
    * Internal hash array for locks held by the transactions.
    */
   struct psonTxMyHash
   {
      /** Offset to an array of offsets to psonTxMyHashItem objects */
      ptrdiff_t    arrayOffset; 
   
      /** Number of items stored in this hash map. */
      size_t       numberOfItems;
   
      /** The index into the array of lengths (aka the number of buckets). */
      int lengthIndex;

      /** The mimimum shrinking factor that we can tolerate to accommodate
       *  the reservedSize argument of psonHashInit() ) */
      int lengthIndexMinimum;

      /** Indicator of the current status of the array. */
      psonHashResizeEnum enumResize;
   
   } listOfLocks;

   /** Variable size struct - always put at the end */
   struct psonBlockGroup blockGroup;

};

typedef struct psonTx psonTx;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

PHOTON_ENGINE_EXPORT
bool psonTxAddOps( psonTx             * pTx,
                   psonTxType           txType,
                   ptrdiff_t            parentOffset, 
                   psonMemObjIdent      parentType,
                   ptrdiff_t            childOffset,
                   psonMemObjIdent      childType,
                   psonSessionContext * pContext );
   
PHOTON_ENGINE_EXPORT
void psonTxRemoveLastOps( psonTx * pTx, psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTxInit( psonTx             * pTx,
                 size_t               numberOfBlocks,
                 psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
void psonTxFini( psonTx             * pTx, 
                 psonSessionContext * pContext );

PHOTON_ENGINE_EXPORT
bool psonTxCommit( psonTx             * pTx,
                   psonSessionContext * pContext );
                  
PHOTON_ENGINE_EXPORT
bool psonTxRollback( psonTx             * pTx,
                     psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TX_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

