/*
 * Copyright (C) 2007 Daniel Prevost <dprevost@users.sourceforge.net>
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

#ifndef VDSE_TX_H
#define VDSE_TX_H

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#include "Engine.h"
#include "LinkedList.h"
#include "SessionContext.h"
#include "MemoryObject.h"
#include "BlockGroup.h"

#define VDSE_TX_SIGNATURE 0xabc6c981

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

BEGIN_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef enum vdseTxType
{
   /* ops on data */
   VDSE_TX_ADD_DATA = 1,
   VDSE_TX_REMOVE_DATA,

   /* ops on objects */
   VDSE_TX_ADD_OBJECT = 0x81,
   VDSE_TX_REMOVE_OBJECT

} vdseTxType;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

/* Each element of a transaction is kept in a doubly linked list
 * (for fast access). 
 */

typedef struct vdseTxOps
{
   vdseTxType      transType;
   ptrdiff_t       parentOffset;  
   vdseMemObjIdent parentType;
   ptrdiff_t       childOffset;
   vdseMemObjIdent childType;

   vdseLinkNode node;
   
} vdseTxOps;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

typedef struct vdseTx
{
   /** Always first */
   struct vdseMemObject memObject;

   int signature;

   /** Linked list of all ops of the current transaction */   
   vdseLinkedList listOfOps;

   /** Variable size struct - always put at the end */
   struct vdseBlockGroup blockGroup;

} vdseTx;

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

VDSF_ENGINE_EXPORT
int vdseTxAddOps( vdseTx             * pTx,
                  vdseTxType           txType,
                  ptrdiff_t            parentOffset, 
                  vdseMemObjIdent      parentType,
                  ptrdiff_t            childOffset,
                  vdseMemObjIdent      childType,
                  vdseSessionContext * pContext );
   
VDSF_ENGINE_EXPORT
void vdseTxRemoveLastOps( vdseTx * pTx, vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseTxInit( vdseTx             * pTx,
                size_t               numberOfBlocks,
                vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
void vdseTxFini( vdseTx             * pTx, 
                 vdseSessionContext * pContext );

VDSF_ENGINE_EXPORT
int vdseTxCommit( vdseTx             * pTx,
                  vdseSessionContext * pContext );
                  
VDSF_ENGINE_EXPORT
void vdseTxRollback( vdseTx             * pTx,
                     vdseSessionContext * pContext );

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

END_C_DECLS

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

#endif /* VDSE_TX_H */

/* --+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+-- */

