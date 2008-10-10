/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@photonsoftware.org>
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

#ifndef PSON_TX_H
#define PSON_TX_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "LinkedList.h"
#include "SessionContext.h"
#include "MemoryObject.h"
#include "BlockGroup.h"

#define PSON_TX_SIGNATURE 0xabc6c981

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psonTxType
{
   /* ops on data */
   PSON_TX_ADD_DATA = 1,
   PSON_TX_REMOVE_DATA,
/*   PSON_TX_REPLACE_DATA, */

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
void psonTxCommit( psonTx             * pTx,
                   psonSessionContext * pContext );
                  
PHOTON_ENGINE_EXPORT
void psonTxRollback( psonTx             * pTx,
                     psonSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSON_TX_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

