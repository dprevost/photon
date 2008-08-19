/*
 * Copyright (C) 2007-2008 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef PSN_TX_H
#define PSN_TX_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "LinkedList.h"
#include "SessionContext.h"
#include "MemoryObject.h"
#include "BlockGroup.h"

#define PSN_TX_SIGNATURE 0xabc6c981

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

enum psnTxType
{
   /* ops on data */
   PSN_TX_ADD_DATA = 1,
   PSN_TX_REMOVE_DATA,
/*   PSN_TX_REPLACE_DATA, */

   /* ops on objects */
   PSN_TX_ADD_OBJECT = 0x81,
   PSN_TX_REMOVE_OBJECT,
   PSN_TX_ADD_EDIT_OBJECT

};

typedef enum psnTxType psnTxType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Each element of a transaction is kept in a doubly linked list
 * (for fast access). 
 */
struct psnTxOps
{
   psnTxType      transType;
   ptrdiff_t       parentOffset;  
   psnMemObjIdent parentType;
   ptrdiff_t       childOffset;
   psnMemObjIdent childType;

   psnLinkNode node;
   
};

typedef struct psnTxOps psnTxOps;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

struct psnTx
{
   /** Always first */
   struct psnMemObject memObject;

   int signature;

   /** Linked list of all ops of the current transaction */   
   psnLinkedList listOfOps;

   /** Variable size struct - always put at the end */
   struct psnBlockGroup blockGroup;

};

typedef struct psnTx psnTx;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
bool psnTxAddOps( psnTx             * pTx,
                   psnTxType           txType,
                   ptrdiff_t            parentOffset, 
                   psnMemObjIdent      parentType,
                   ptrdiff_t            childOffset,
                   psnMemObjIdent      childType,
                   psnSessionContext * pContext );
   
VDSF_ENGINE_EXPORT
void psnTxRemoveLastOps( psnTx * pTx, psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
bool psnTxInit( psnTx             * pTx,
                 size_t               numberOfBlocks,
                 psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnTxFini( psnTx             * pTx, 
                 psnSessionContext * pContext );

VDSF_ENGINE_EXPORT
void psnTxCommit( psnTx             * pTx,
                   psnSessionContext * pContext );
                  
VDSF_ENGINE_EXPORT
void psnTxRollback( psnTx             * pTx,
                     psnSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* PSN_TX_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

